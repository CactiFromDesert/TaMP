#include "database.h"

Database::Database(const std::string& conn_str) // Создаем подключение при создании объекта Database
    : conn(conn_str)
{
    try {
        if (conn.is_open()) {
            std::cout << "Connected to database successfully" << std::endl;
            std::cout << "Server version: " << conn.server_version() << std::endl;
        } else {
            std::cerr << "Failed to connect - connection not open" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        throw;
    }
}

// Создаем ссылку на подключение для использования в других местах
pqxx::connection& Database::getConnection() { 
    return conn;
}