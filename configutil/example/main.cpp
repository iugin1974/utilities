#include <cstdlib>
#include <iostream>
#include "appconfig.h"

int main() {
    // Nome del programma -> legge ~/.pmrc
    AppConfig::init("pm");

    std::string user_ftp, host_ftp, pwd;
    bool found = false;

    found = AppConfig::read("user_ftp", &user_ftp);
    if (!found) {
        std::cerr << "The " << AppConfig::configFilePath()
                  << " file was not found or the key 'user_ftp' does not exist.\n"
                  << "Edit the file by adding the string 'user_ftp=<username>'" << std::endl;
        exit(1);
    }

    found = AppConfig::read("host_ftp", &host_ftp);
    if (!found) {
        std::cerr << "The " << AppConfig::configFilePath()
                  << " file was not found or the key 'host_ftp' does not exist.\n"
                  << "Edit the file by adding the string 'host_ftp=<hostname>'" << std::endl;
        exit(1);
    }

    found = AppConfig::read("pass_ftp", &pwd);
    if (!found) {
        pwd = AppConfig::getpass("FTP-Password", true);
    }

    std::cout << "user=" << user_ftp << " host=" << host_ftp
              << " pwd_len=" << pwd.size() << std::endl;

    return 0;
}
