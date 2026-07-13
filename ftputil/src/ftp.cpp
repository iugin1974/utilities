#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include "ftp.h"
using namespace std;

bool ftp::initialized = false;

void ftp::init() {
    if (!initialized) {
        curl_global_init(CURL_GLOBAL_ALL);
        initialized = true;
    }
}

void ftp::close() {
    if (initialized) {
        curl_global_cleanup();
        initialized = false;
    }
}

size_t ftp::read_callback(char *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t retcode = fread(ptr, size, nmemb, stream);
    return retcode;
}

/*
 * Passata una path come argomento, ritorna il nome del file.
 * Es: /foo/bar/moo.txt
 * ritorna moo.txt
 */
std::string ftp::get_local_file_name(const std::string& n) {
    return std::filesystem::path(n).filename().string();
}

/*
 * Unisce due stringhe passate come argomento.
 */
std::string ftp::combine_string(const std::string& str1, const std::string& str2) const {
    return str1 + str2;
}

/*
 * Costruisce l'URL finale applicando lo schema corretto (ftp/ftps/sftp)
 * indipendentemente da come è stato scritto l'host con setHost().
 */
std::string ftp::buildUrl(const std::string& remainder) const {
    std::string host = ftp_host;
    auto schemePos = host.find("://");
    if (schemePos != std::string::npos) {
        host = host.substr(schemePos + 3); // rimuove uno schema eventualmente già presente
    }

    std::string scheme;
    switch (protocol) {
        case Protocol::FTP:            scheme = "ftp://";  break;
        case Protocol::FTPS_EXPLICIT:  scheme = "ftp://";  break; // AUTH TLS: schema resta ftp://
        case Protocol::FTPS_IMPLICIT:  scheme = "ftps://"; break;
        case Protocol::SFTP:           scheme = "sftp://"; break;
    }
    return combine_string(combine_string(scheme, host), remainder);
}

/*
 * Applica all'handle curl le opzioni necessarie per il protocollo scelto
 * (TLS esplicito/implicito per FTPS, host-key e chiavi per SFTP).
 */
void ftp::configureProtocol(void* curlHandle) const {
    CURL* curl = static_cast<CURL*>(curlHandle);

    if (protocol == Protocol::FTPS_EXPLICIT || protocol == Protocol::FTPS_IMPLICIT) {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verifySslPeer ? 1L : 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verifySslHost ? 2L : 0L);
    }

    if (protocol == Protocol::SFTP) {
        if (!sshKnownHostsFile.empty()) {
            // rifiuta la connessione se l'host non è nel known_hosts o la chiave non combacia
            curl_easy_setopt(curl, CURLOPT_SSH_KNOWNHOSTS, sshKnownHostsFile.c_str());
        }
        if (!sshPrivateKeyPath.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES,
                              CURLSSH_AUTH_PUBLICKEY | CURLSSH_AUTH_PASSWORD);
            curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, sshPrivateKeyPath.c_str());
            if (!sshPublicKeyPath.empty())
                curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, sshPublicKeyPath.c_str());
            if (!sshPassphrase.empty())
                curl_easy_setopt(curl, CURLOPT_KEYPASSWD, sshPassphrase.c_str());
        }
    }
}

int ftp::uploadFile(const std::string& f) {
    init(); // idempotente: non fa nulla se già inizializzato

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (!curl) {
        std::cout << "Error. Exit\n";
        return 1;
    }

    std::string un = createUserName();
    curl_easy_setopt(curl, CURLOPT_USERPWD, un.c_str());

    FILE* source_file = fopen(f.c_str(), "rb");
    if (!source_file) {
        std::cerr << "Impossibile aprire il file locale: " << f << "\n";
        curl_easy_cleanup(curl);
        return 1;
    }

    std::string file_name = get_local_file_name(f);
    if (verbose) std::cout << "Upload file " << file_name << " to ";

    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    std::string remote_url = buildUrl(file_name);
    if (verbose) std::cout << remote_url << "\n";

    curl_easy_setopt(curl, CURLOPT_URL, remote_url.c_str());
    configureProtocol(curl);

    // Necessario su Windows / con libcurl come DLL, innocuo altrove
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, source_file);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        fclose(source_file);
        curl_easy_cleanup(curl);
        return 1;
    }

    fclose(source_file);
    curl_easy_cleanup(curl);
    return 0;
}

int ftp::downloadFile(const std::string& local_file) {
    std::string file_name = get_local_file_name(local_file);
    return downloadFile(file_name, local_file);
}

int ftp::downloadFile(const std::string& ftp_file, const std::string& local_file) {
    init(); // idempotente: non fa nulla se già inizializzato

    CURL *curl;
    CURLcode res = CURLE_FAILED_INIT;
    struct FtpFile ftpfile = {
        local_file.c_str(), NULL
    };

    curl = curl_easy_init();
    if (curl) {
        std::string remote_file = buildUrl(ftp_file);
        curl_easy_setopt(curl, CURLOPT_URL, remote_file.c_str());
        configureProtocol(curl);
        std::string un = createUserName();
        curl_easy_setopt(curl, CURLOPT_USERPWD, un.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

        if (verbose)
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (CURLE_OK != res) {
            fprintf(stderr, "curl told us %d\n", res);
        }
    }

    if (ftpfile.stream)
        fclose(ftpfile.stream);

    return res;
}

size_t ftp::my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpFile *out = (struct FtpFile *)stream;
    if (!out->stream) {
        out->stream = fopen(out->filename, "wb");
        if (!out->stream)
            return -1;
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

void ftp::setPassword(std::string pwd) {
    this->password = pwd;
    passwordSetted = true;
}

bool ftp::isPasswordSet() {
    return passwordSetted;
}

void ftp::setUserName(std::string userName) {
    this->userName = userName;
}

void ftp::setHost(std::string ftp_host) {
    this->ftp_host = ftp_host;
}

void ftp::setVerbose(bool v) {
    verbose = v;
}

void ftp::setProtocol(Protocol p) {
    protocol = p;
}

void ftp::setVerifySsl(bool verifyPeer, bool verifyHost) {
    verifySslPeer = verifyPeer;
    verifySslHost = verifyHost;
}

void ftp::setSshKnownHostsFile(std::string path) {
    sshKnownHostsFile = std::move(path);
}

void ftp::setSshPrivateKey(std::string privateKeyPath, std::string publicKeyPath, std::string passphrase) {
    sshPrivateKeyPath = std::move(privateKeyPath);
    sshPublicKeyPath = std::move(publicKeyPath);
    sshPassphrase = std::move(passphrase);
}

std::string ftp::createUserName() {
    return this->userName + ":" + password;
}