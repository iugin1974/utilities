#include "value_reader.hpp"
#include <iostream>

int main() {
    value_reader::set_file("file.txt");

    const std::string* name  = value_reader::get("name");
    const std::string* birth = value_reader::get("birth"); // valore vuoto -> nullptr
    const std::string* city  = value_reader::get("city");  // chiave assente -> nullptr

    std::cout << "name: "  << (name  ? *name  : "(nullptr)") << '\n';
    std::cout << "birth: " << (birth ? *birth : "(nullptr)") << '\n';
    std::cout << "city: "  << (city  ? *city  : "(nullptr)") << '\n';

    return 0;
}
