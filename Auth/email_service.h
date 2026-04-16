#pragma once

#include "common.h"
#include "email_config.h"

// Структура для хранения данных email сообщения
struct EmailMessage {
    std::string to_email;
    std::string to_name;
    std::string subject;
    std::string body;
};

// Структура для временного хранения кода верификации (в памяти)
struct VerificationCode {
    std::string code;
    std::chrono::system_clock::time_point expires_at;
};

// Структура для callback функции CURL при отправке
struct UploadStatus {
    const std::string* data;
    size_t bytes_read;
    
    UploadStatus(const std::string* d) : data(d), bytes_read(0) {}
};

class EmailService {
private:
    // Временное хранилище кодов в памяти (email -> код)
    static std::map<std::string, VerificationCode> pending_codes;
    
    // Статические callback функции для CURL
    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output);
    static size_t read_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
    
    // Форматирование email сообщения в RFC 822 формат
    static std::string formatEmailMessage(const EmailMessage& msg);
    
    // Получение текущей даты в формате RFC 822
    static std::string getRFC822Date();
    
    // Кодирование строки в Base64 (для аутентификации)
    static std::string base64_encode(const std::string& input);

public:
    // Генерация 6-значного кода
    static std::string generateVerificationCode();
    
    // Отправка email с кодом верификации
    static bool sendVerificationEmail(const std::string& to_email, 
                                      const std::string& user_name = "");
    
    // Проверка кода верификации
    static bool verifyCode(const std::string& email, const std::string& code);
    
    // Проверка формата email
    static bool isValidEmail(const std::string& email);
    
    // Отправка произвольного email
    static bool sendEmail(const EmailMessage& message);
    
    // Проверка соединения с SMTP сервером
    static bool testSmtpConnection();
    
    // Очистка просроченных кодов
    static void cleanupExpiredCodes();
};