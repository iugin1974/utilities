# kvutil

Utility C++ header-only per leggere coppie `chiave=valore` da file di configurazione semplici.

Fa parte della stessa famiglia di utility di `configutil` e `ftputil`.

## Caratteristiche

- Header-only, nessuna dipendenza esterna (solo STL).
- Parsing semplice di file testuali nel formato `chiave=valore`, una coppia per riga.
- Distingue in modo esplicito tra:
  - chiave **assente** nel file,
  - chiave presente con valore **vuoto**,
  - chiave presente con valore valido.

  In entrambi i primi due casi `get()` restituisce `nullptr`.

## Formato del file

```
name=mario
birth=
```

- Le righe vuote vengono ignorate.
- Le righe senza `=` vengono ignorate.
- Un valore vuoto dopo `=` (es. `birth=`) viene trattato come chiave assente.
- Gestisce automaticamente file con terminazione riga sia Unix (`\n`) sia Windows (`\r\n`).

## API

```cpp
#include "value_reader.hpp"

// Carica (o ricarica) un file. Lancia std::runtime_error se non riesce ad aprirlo.
value_reader::set_file("file.txt");

// Restituisce un puntatore al valore, oppure nullptr se la chiave
// non esiste o il suo valore è vuoto.
const std::string* value_reader::get(const std::string& key);
```

## Esempio d'uso

File `file.txt`:
```
name=mario
birth=
```

Codice:
```cpp
#include "value_reader.hpp"
#include <iostream>

int main() {
    value_reader::set_file("file.txt");

    const std::string* name  = value_reader::get("name");
    const std::string* birth = value_reader::get("birth"); // vuoto -> nullptr
    const std::string* city  = value_reader::get("city");  // assente -> nullptr

    std::cout << "name: "  << (name  ? *name  : "(nullptr)") << '\n';
    std::cout << "birth: " << (birth ? *birth : "(nullptr)") << '\n';
    std::cout << "city: "  << (city  ? *city  : "(nullptr)") << '\n';
}
```

Output:
```
name: mario
birth: (nullptr)
city: (nullptr)
```

## Note sull'uso del puntatore

`get()` restituisce un puntatore che fa riferimento a dati interni della classe. Il puntatore rimane valido finché non si richiama `set_file()`, che ricarica (e quindi invalida) i dati precedenti. Non conservare il puntatore oltre una successiva chiamata a `set_file()`.

## Requisiti

- C++17 o superiore.

## Compilazione (esempio)

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o app
```

## Possibili estensioni future

- Supporto a `std::optional<std::string>` come alternativa al puntatore.
- Supporto a sezioni in stile INI (`[section]`).
- Lettura da `std::istream` oltre che da percorso file.
