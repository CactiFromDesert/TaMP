#pragma once

#include "common.h"
#include "email_config.h"

/**
 * @brief Структура для хранения данных email сообщения.
 */
struct EmailMessage {
    std::string to_email;  ///< Email получателя
    std::string to_name;   ///< Имя получателя
    std::string subject;   ///< Тема письма
    std::string body;      ///< Тело письма
};

/**
 * @brief Структура для временного хранения кода верификации.
 */
struct VerificationCode {
    std::string code;                                      ///< 6-значный код
    std::chrono::system_clock::time_point expires_at;      ///< Время истечения (10 минут)
};

/**
 * @brief Структура для callback функции CURL при отправке.
 */
struct UploadStatus {
    const std::string* data;      ///< Данные для отправки
    size_t bytes_read;            ///< Количество прочитанных байт
    
    UploadStatus(const std::string* d) : data(d), bytes_read(0) {}
};

/**
 * @brief Сервис для отправки email через SMTP и верификации кодов.
 * 
 * Использует библиотеку CURL для взаимодействия с SMTP-сервером.
 * Коды верификации хранятся в оперативной памяти с ограничением времени жизни.
 */
class EmailService {
private:
    /// Хранилище кодов в памяти (email -> код)
    static std::map<std::string, VerificationCode> pending_codes;
    
    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output);
    static size_t read_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
    static std::string formatEmailMessage(const EmailMessage& msg);
    static std::string getRFC822Date();
    static std::string base64_encode(const std::string& input);

public:
    /**
     * @brief Генерирует 6-значный случайный код верификации.
     * @return std::string Код из 6 цифр
     */
    static std::string generateVerificationCode();
    
    /**
     * @brief Отправляет email с кодом верификации на указанный адрес.
     * @param to_email Email получателя
     * @param user_name Имя получателя (для персонализации)
     * @return true Письмо успешно отправлено
     * @return false Ошибка отправки
     */
    static bool sendVerificationEmail(const std::string& to_email, 
                                      const std::string& user_name = "");
    
    /**
     * @brief Проверяет код верификации для указанного email.
     * @param email Email, на который был отправлен код
     * @param code Введённый пользователем код
     * @return true Код верен и не истек
     * @return false Код неверен или истек
     */
    static bool verifyCode(const std::string& email, const std::string& code);
    
    /**
     * @brief Проверяет корректность формата email.
     * @param email Проверяемый email
     * @return true Формат корректен
     * @return false Некорректный email
     */
    static bool isValidEmail(const std::string& email);
    
    static bool sendEmail(const EmailMessage& message);
    static bool testSmtpConnection();
    static void cleanupExpiredCodes();
    static void clearAllCodes();
};