#include <iostream>
#include "ftp.h"

int main() {
    // init/close vanno chiamati una volta sola per l'intero programma
    ftp::init();

    // --- Esempio 1: FTP semplice (nessuna cifratura) ---
    {
        ftp client;
        client.setHost("example.com/remote/dir/"); // schema aggiunto automaticamente
        client.setUserName("utente");
        client.setPassword("password");
        client.setProtocol(Protocol::FTP); // default, esplicitato per chiarezza

        client.uploadFile("/percorso/locale/file.txt");
    }

    // --- Esempio 2: FTPS esplicito (AUTH TLS su porta 21) ---
    {
        ftp client;
        client.setHost("example.com/remote/dir/");
        client.setUserName("utente");
        client.setPassword("password");
        client.setProtocol(Protocol::FTPS_EXPLICIT);
        client.setVerifySsl(true, true); // consigliato in produzione

        client.uploadFile("/percorso/locale/file.txt");
    }

    // --- Esempio 3: SFTP con autenticazione a password ---
    {
        ftp client;
        client.setHost("example.com/remote/dir/");
        client.setUserName("utente");
        client.setPassword("password");
        client.setProtocol(Protocol::SFTP);
        client.setSshKnownHostsFile("/home/utente/.ssh/known_hosts"); // consigliato

        client.downloadFile("file.txt", "/percorso/locale/copia_file.txt");
    }

    // --- Esempio 4: SFTP con autenticazione a chiave ---
    {
        ftp client;
        client.setHost("example.com/remote/dir/");
        client.setUserName("utente");
        client.setProtocol(Protocol::SFTP);
        client.setSshKnownHostsFile("/home/utente/.ssh/known_hosts");
        client.setSshPrivateKey("/home/utente/.ssh/id_rsa", "/home/utente/.ssh/id_rsa.pub");

        client.uploadFile("/percorso/locale/file.txt");
    }

    ftp::close();
    return 0;
}
