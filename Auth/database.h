#pragma once

#include "common.h"

class Database {
private:
    pqxx::connection conn; // Подключение к PostgreSQL

public:
    Database(const std::string& conn_str); // Конструктор
    pqxx::connection& getConnection(); // Геттер для доступа к conn
};