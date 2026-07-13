# configutil

Modulo riutilizzabile per leggere un file di configurazione utente
`~/.<nomeprogramma>rc` e per chiedere password da terminale senza eco.

Nato per essere copiato/linkato identico in ogni nuovo progetto, così
la convenzione ".xxxrc" non va reinventata né ristudiata ogni volta.

## Integrazione in un nuovo progetto

1. Copia (o aggiungi come submodule) la cartella `configutil/` dentro il
   tuo progetto, es. in `libs/configutil`.
2. Nel tuo `CMakeLists.txt` principale:

   ```cmake
   add_subdirectory(libs/configutil)
   target_link_libraries(mio_programma PRIVATE configutil)
   ```

## Convenzione del file `.xxxrc`

- Percorso: `~/.<nome_programma>rc` (es. per un programma "pm" ->
  `~/.pmrc`). Il nome è quello passato a `AppConfig::init(...)`, non il
  nome dell'eseguibile: puoi quindi rinominare il binario senza rompere
  la config esistente degli utenti.
- Formato: una coppia `chiave=valore` per riga.
- Righe vuote o che iniziano con `#` sono ignorate (commenti).
- Spazi intorno a chiave e valore vengono rimossi automaticamente.

Esempio di `~/.pmrc`:

```
# Configurazione FTP per pm
user_ftp=mario
host_ftp=ftp.example.com/remote/dir/
# pass_ftp volutamente omessa: verrà chiesta a terminale ad ogni avvio
```

## Uso in `main()`

```cpp
#include "appconfig.h"

int main() {
    AppConfig::init("pm"); // legge ~/.pmrc

    std::string user, host, pwd;

    if (!AppConfig::read("user_ftp", &user)) {
        std::cerr << "Manca 'user_ftp' in " << AppConfig::configFilePath() << "\n";
        exit(1);
    }
    if (!AppConfig::read("host_ftp", &host)) {
        std::cerr << "Manca 'host_ftp' in " << AppConfig::configFilePath() << "\n";
        exit(1);
    }
    if (!AppConfig::read("pass_ftp", &pwd)) {
        pwd = AppConfig::getpass("FTP-Password", true); // niente eco a schermo
    }
}
```

## Sicurezza — checklist da rifare per ogni nuovo progetto

- **Permessi del file**: consiglia (o forza) `chmod 600 ~/.pmrc`, così
  solo il proprietario può leggere le eventuali password in chiaro.
  `configutil` non lo impone automaticamente: se vuoi, aggiungi un
  controllo con `stat()` in `AppConfig::read` che avvisi se i permessi
  sono troppo permissivi.
- **Non committare mai un file `.xxxrc` reale** nel repository. Se tieni
  un file di esempio, chiamalo `.pmrc.example` e aggiungi `.pmrc`
  al `.gitignore` del progetto.
- **Preferisci `getpass()` a una entry nel file** quando possibile: una
  password richiesta a runtime non finisce mai su disco.

## Estendere il modulo

Se in futuro ti serve, ad esempio, un path di configurazione passato
da riga di comando (`--config /path/personalizzato`), usa il secondo
parametro di `init()`:

```cpp
AppConfig::init("pm", cliConfigPathOrEmpty);
```

Se `explicitPath` non è vuoto, ha sempre la precedenza su
`~/.<nome>rc`.
