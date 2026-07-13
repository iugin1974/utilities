# ftputil

Modulo riutilizzabile per upload/download di file via FTP, FTPS
(esplicito e implicito) e SFTP, basato su libcurl.

Nato per essere copiato/linkato identico in ogni nuovo progetto che
deve trasferire file da/verso un server remoto.

## Dipendenze

- libcurl con development headers (`libcurl4-openssl-dev` su Debian/Ubuntu)
- Per SFTP: libcurl deve essere compilata con supporto libssh2/libssh.
  Verifica con:
  ```bash
  curl -V | grep -i sftp
  ```
  Se `sftp` non compare tra i protocolli, installa `libssh2-dev` e
  ricompila libcurl, oppure usa una distribuzione che la fornisce già
  abilitata (es. Ubuntu/Debian standard).

## Integrazione in un nuovo progetto

1. Copia (o aggiungi come submodule) la cartella `ftputil/` dentro il
   tuo progetto, es. in `libs/ftputil`.
2. Nel tuo `CMakeLists.txt` principale:

   ```cmake
   add_subdirectory(libs/ftputil)
   target_link_libraries(mio_programma PRIVATE ftputil)
   ```

## Uso base (FTP semplice)

```cpp
#include "ftp.h"

ftp::init(); // una sola volta, a inizio programma

ftp client;
client.setHost("example.com/remote/dir/");
client.setUserName("utente");
client.setPassword("password");

client.uploadFile("/percorso/locale/file.txt");
client.downloadFile("file.txt", "/percorso/locale/copia.txt");

ftp::close(); // una sola volta, a fine programma
```

## Protocolli supportati

Impostabili con `setProtocol(Protocol p)`:

| Valore | Significato |
|---|---|
| `Protocol::FTP` | FTP in chiaro, nessuna cifratura (default) |
| `Protocol::FTPS_EXPLICIT` | `ftp://` + AUTH TLS, porta 21 |
| `Protocol::FTPS_IMPLICIT` | `ftps://` diretto, tipicamente porta 990 |
| `Protocol::SFTP` | SSH, tipicamente porta 22 |

Lo schema dell'URL (`ftp://`/`ftps://`/`sftp://`) viene applicato
automaticamente in base al protocollo scelto: puoi passare a
`setHost()` l'host con o senza schema, non fa differenza.

### FTPS

```cpp
client.setProtocol(Protocol::FTPS_EXPLICIT);
client.setVerifySsl(true, true); // verifica certificato, consigliato in produzione
```

Disattivare la verifica (`setVerifySsl(false, false)`) va bene solo
per test locali con certificati self-signed, **mai in produzione**.

### SFTP

```cpp
client.setProtocol(Protocol::SFTP);
client.setSshKnownHostsFile("/home/utente/.ssh/known_hosts"); // consigliato

// autenticazione a password (default, usa setUserName/setPassword)
// oppure a chiave:
client.setSshPrivateKey("/home/utente/.ssh/id_rsa", "/home/utente/.ssh/id_rsa.pub");
```

`setSshKnownHostsFile` valida la chiave pubblica del server contro un
file `known_hosts`: senza, la connessione SFTP è vulnerabile ad
attacchi man-in-the-middle.

## Sicurezza — checklist da rifare per ogni nuovo progetto

- Usa `FTP` in chiaro solo su reti fidate/interne. Su internet
  preferisci sempre `FTPS_EXPLICIT`/`FTPS_IMPLICIT` o `SFTP`.
- Non disattivare `setVerifySsl` in produzione.
- Per SFTP, imposta sempre `setSshKnownHostsFile`.
- Le credenziali (utente/password) tipicamente arrivano da
  [`configutil`](../configutil/README.md), non vanno mai hardcodate
  nel sorgente.

## Esempio completo

Vedi [`example/main.cpp`](example/main.cpp) per tutti e quattro i casi
(FTP, FTPS esplicito, SFTP a password, SFTP a chiave).
