#pragma once

#include "common.h"
#include "database.h"
#include "email_service.h"

class Auth {
private:
    Database& db; // Ссылка на объект Database 

    // Методы работы с паролями
    std::string hashPassword(const std::string& password); // Хэширование пароля
    bool verifyPassword(const std::string& password, const std::string& hash); // Проверка пароля

public:
    Auth(Database& database); // Конструктор принимающий ссылку на Database

    // Данные пользователя при регистрации и заходе
    bool registerUser(const std::string& name,
                      const std::string& login,
                      const std::string& password,
                      const std::string& email);
    
    bool loginUser(const std::string& login,
                   const std::string& password);
};