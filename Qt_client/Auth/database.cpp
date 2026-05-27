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

// database.cpp
std::string Database::getConnectionString()
{
    const char* host = std::getenv("DB_HOST");
    const char* port = std::getenv("DB_PORT");
    const char* user = std::getenv("DB_USER");
    const char* password = std::getenv("DB_PASSWORD");
    const char* dbname = std::getenv("DB_NAME");
    
    std::string conn = "postgresql://";
    conn += (user ? user : "postgres");
    conn += ":";
    conn += (password ? password : "1234");
    conn += "@";
    conn += (host ? host : "postgres");
    conn += ":";
    conn += (port ? port : "5432");
    conn += "/";
    conn += (dbname ? dbname : "usersauth");
    
    return conn;
}