#ifndef FTPUTIL_FTP_H
#define FTPUTIL_FTP_H

#include <string>
#include <cstdio>

// Struttura di supporto usata dal callback di download
struct FtpFile {
    const char *filename;
    FILE *stream;
};

// Protocollo/modalità di trasferimento
enum class Protocol {
    FTP,           // ftp:// in chiaro, nessuna cifratura
    FTPS_EXPLICIT, // ftp:// + AUTH TLS (porta 21, upgrade esplicito a TLS)
    FTPS_IMPLICIT, // ftps:// diretto (tipicamente porta 990, TLS fin dall'inizio)
    SFTP           // sftp:// su SSH (richiede libcurl compilato con libssh2/libssh)
};

class ftp {
public:
    // init() è idempotente: chiamarla quando è già inizializzata non fa
    // nulla. Viene chiamata automaticamente dalla classe stessa prima di
    // ogni operazione (uploadFile/downloadFile), quindi normalmente NON
    // devi chiamarla tu a mano.
    static void init();

    // close() chiude curl SOLO se init() era stata effettivamente eseguita
    // (flag interno); chiamarla senza aver mai inizializzato, o più volte
    // di fila, è innocuo. Da chiamare una volta, quando il programma non
    // farà più operazioni FTP (tipicamente a fine main()).
    static void close();

    void setHost(std::string ftp_host);
    void setUserName(std::string userName);
    void setPassword(std::string pwd);
    bool isPasswordSet();
    void setVerbose(bool v);

    // Sceglie il protocollo di trasferimento. Default: Protocol::FTP
    void setProtocol(Protocol p);

    // Verifica del certificato TLS per FTPS. Default: entrambi true (consigliato).
    // Disattivare la verifica va bene solo per test locali, MAI in produzione.
    void setVerifySsl(bool verifyPeer, bool verifyHost = true);

    // --- Opzioni specifiche SFTP ---
    // File known_hosts per validare la chiave pubblica del server (fortemente consigliato)
    void setSshKnownHostsFile(std::string path);
    // Autenticazione a chiave invece che a password (facoltativa)
    void setSshPrivateKey(std::string privateKeyPath,
                           std::string publicKeyPath = "",
                           std::string passphrase = "");

    // Carica il file locale f sul server, nella root configurata con setHost()
    int uploadFile(const std::string& f);

    // Scarica un file remoto il cui nome coincide col nome del file locale
    int downloadFile(const std::string& local_file);
    // Scarica ftp_file dal server e lo salva come local_file
    int downloadFile(const std::string& ftp_file, const std::string& local_file);

private:
    std::string ftp_host;
    std::string userName;
    std::string password;
    bool passwordSetted = false;
    bool verbose = false;

    Protocol protocol = Protocol::FTP;
    bool verifySslPeer = true;
    bool verifySslHost = true;

    std::string sshKnownHostsFile;
    std::string sshPrivateKeyPath;
    std::string sshPublicKeyPath;
    std::string sshPassphrase;

    std::string createUserName();
    std::string get_local_file_name(const std::string& n);
    std::string combine_string(const std::string& str1, const std::string& str2) const;

    // Costruisce l'URL completo (schema corretto in base al protocollo + host + remainder)
    std::string buildUrl(const std::string& remainder) const;
    // Applica a un handle curl le opzioni relative a protocollo/TLS/SSH scelte
    void configureProtocol(void* curlHandle) const;

    static size_t read_callback(char *ptr, size_t size, size_t nmemb, FILE *stream);
    static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream);

    // true se curl_global_init è stata effettivamente eseguita e non ancora chiusa.
    static bool initialized;
};

#endif // FTPUTIL_FTP_H