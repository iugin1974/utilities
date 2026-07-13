#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>

// Gestisce la lettura di un file di configurazione "~/.<programma>rc"
// (formato key=value, righe che iniziano con '#' ignorate) e la richiesta
// interattiva di password da terminale senza eco.
//
// Uso tipico in main():
//
//   AppConfig::init("pm");   // legge/scrive ~/.pmrc
//   std::string user, host, pwd;
//   if (!AppConfig::read("user_ftp", &user)) { ... errore ... }
//   if (!AppConfig::read("host_ftp", &host)) { ... errore ... }
//   if (!AppConfig::read("pass_ftp", &pwd))
//       pwd = AppConfig::getpass("FTP-Password", true);
//
class AppConfig {
public:
    // Da chiamare una sola volta, all'inizio di main().
    // programName: nome usato per comporre "~/.<programName>rc" (es. "pm" -> ~/.pmrc)
    // explicitPath: se non vuoto, sovrascrive completamente il path calcolato
    //               (utile per test o per un --config passato da riga di comando)
    static void init(const std::string& programName, const std::string& explicitPath = "");

    // Cerca "key" nel file di configurazione. Ritorna false se il file non
    // esiste o la chiave non è presente (senza sollevare errori/eccezioni).
    static bool read(const std::string& key, std::string* value);

    // Chiede una password da terminale.
    // mask=true  -> l'input non viene mostrato a schermo (consigliato)
    // mask=false -> l'input resta visibile (solo per debug/test)
    static std::string getpass(const std::string& prompt, bool mask = true);

    // Path completo del file di configurazione risolto, utile nei messaggi d'errore.
    static std::string configFilePath();

private:
    static std::string progName;
    static std::string overridePath;
};

#endif // APPCONFIG_H
