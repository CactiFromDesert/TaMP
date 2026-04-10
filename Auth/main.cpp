#include "common.h"
#include "database.h"
#include "auth.h"

int main() {
    std::cout << "=== Starting Auth System ===" << std::endl;
    
    // Инициализация libsodium
    if (sodium_init() < 0) {
        std::cerr << "ERROR: Failed to initialize libsodium" << std::endl;
        return 1;
    }

    // Строка подключения к PostgreSQL ПОМЕНЯТЬ НА IP сервера (сейчас локально)
    std::string conn_str = 
    "postgresql://postgres:%20%20%20%20@127.0.0.1:5432/usersauth";
    try {
        std::cout << "Connecting to database..." << std::endl;
        Database db(conn_str);
        Auth auth(db);

        int choice;
        std::cout << "\n=== Menu ===" << std::endl;
        std::cout << "1 - Register new user" << std::endl;
        std::cout << "2 - Login" << std::endl;
        std::cout << "Choice: ";
        std::cin >> choice;
        
        // Очистка буфера ввода
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string name, login, password, email;

        if (choice == 1) {
            std::cout << "\n=== Registration ===" << std::endl;
            std::cout << "Enter name: ";
            std::getline(std::cin, name);
            std::cout << "Enter login: ";
            std::getline(std::cin, login);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            std::cout << "Enter email: ";
            std::getline(std::cin, email);
            
            if (auth.registerUser(name, login, password, email)) {
                std::cout << "Registration successful!" << std::endl;
            } else {
                std::cout << "Registration failed" << std::endl;
            }
        }
        else if (choice == 2) {
            std::cout << "\n=== Login ===" << std::endl;
            std::cout << "Enter login: ";
            std::getline(std::cin, login);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            
            if (auth.loginUser(login, password)) {
                std::cout << "Login successful!" << std::endl;
            } else {
                std::cout << "Login failed" << std::endl;
            }
        }
        else {
            std::cout << "Invalid choice" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}