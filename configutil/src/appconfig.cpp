#include "appconfig.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <unistd.h>

std::string AppConfig::progName;
std::string AppConfig::overridePath;

void AppConfig::init(const std::string& programName, const std::string& explicitPath) {
    progName = programName;
    overridePath = explicitPath;
}

std::string AppConfig::configFilePath() {
    if (!overridePath.empty())
        return overridePath;

    const char* home = std::getenv("HOME");
    if (!home) home = "."; // fallback se HOME non è definita

    return std::string(home) + "/." + progName + "rc";
}

static inline std::string trim(const std::string& s) {
    const char* ws = " \t\r\n";
    auto start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

bool AppConfig::read(const std::string& key, std::string* value) {
    std::ifstream file(configFilePath());
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#')
            continue; // riga vuota o commento

        auto eq = trimmed.find('=');
        if (eq == std::string::npos)
            continue; // riga malformata, la ignora

        std::string k = trim(trimmed.substr(0, eq));
        std::string v = trim(trimmed.substr(eq + 1));

        if (k == key) {
            *value = v;
            return true;
        }
    }
    return false;
}

std::string AppConfig::getpass(const std::string& prompt, bool mask) {
    std::cout << prompt << ": " << std::flush;

    termios oldSettings{};
    bool termiosOk = false;

    if (mask) {
        if (tcgetattr(STDIN_FILENO, &oldSettings) == 0) {
            termios newSettings = oldSettings;
            newSettings.c_lflag &= ~ECHO; // disabilita l'eco dei caratteri
            termiosOk = (tcsetattr(STDIN_FILENO, TCSAFLUSH, &newSettings) == 0);
        }
    }

    std::string input;
    std::getline(std::cin, input);

    if (termiosOk) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldSettings); // ripristina l'eco
        std::cout << std::endl; // il terminale non ha mostrato l'invio dell'utente
    }

    return input;
}
