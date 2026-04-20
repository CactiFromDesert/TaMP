#pragma once

#include <string>

// НАСТРОЙКИ SMTP СЕРВЕРА 
namespace EmailConfig {
    // SMTP сервер 
    const std::string SMTP_SERVER = "smtp.gmail.com";
    const int SMTP_PORT = 587; // 587 для TLS, 465 для SSL
    
    // Учетные данные отправителя
    const std::string SMTP_USERNAME = "dmitriy.mikhanoshin@gmail.com"; // ВАША ПОЧТА
    const std::string SMTP_PASSWORD = "zgsgprotqjvxgrnf";    // ПАРОЛЬ ПРИЛОЖЕНИЯ
    
    // Информация об отправителе
    const std::string FROM_EMAIL = "dmitriy.mikhanoshin@gmail.com";
    const std::string FROM_NAME = "SMTP";
    
    // Дополнительные настройки
    const bool USE_TLS = true; // Использовать TLS шифрование
    const bool VERIFY_SSL = true; // Проверять SSL сертификат
    const int TIMEOUT = 30; // Таймаут в секундах
}