#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <stdexcept>

// value_reader: legge un file di coppie "chiave=valore" (una per riga)
// e permette di recuperarne i valori.
//
// Nota: std::string non può essere nullptr (non è un puntatore).
// Per questo get() restituisce const std::string*: sarà nullptr se
// la chiave non esiste, oppure se esiste ma il valore è vuoto
// (es. "birth=" senza nulla dopo l'uguale).
class value_reader {
public:
    // Carica (o ricarica) il file. Lancia std::runtime_error se non
    // riesce ad aprirlo.
    static void set_file(const std::string& filename) {
        data_.clear();

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Impossibile aprire il file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            // gestisce eventuali file con terminazione riga Windows (\r\n)
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) {
                continue;
            }

            auto pos = line.find('=');
            if (pos == std::string::npos) {
                continue; // riga senza '=' -> la ignoriamo
            }

            std::string key   = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (!value.empty()) {
                data_[key] = value;
            }
            // se value è vuoto NON viene inserito -> get() restituirà nullptr
        }
    }

    // Restituisce un puntatore al valore, oppure nullptr se la chiave
    // non esiste o il suo valore è vuoto.
    static const std::string* get(const std::string& key) {
        auto it = data_.find(key);
        if (it == data_.end()) {
            return nullptr;
        }
        return &it->second;
    }

private:
    static inline std::unordered_map<std::string, std::string> data_;
};
