#pragma once

#include "common.h"
#include "database.h"
#include "email_service.h"

/**
 * @brief Результат попытки авторизации.
 */
enum class AuthResult {
    Success,           ///< Авторизация успешна
    WrongPassword,     ///< Неверный пароль
    UserNotFound,      ///< Пользователь не найден
    NeedVerification,  ///< Требуется двухфакторная верификация
    Error              ///< Внутренняя ошибка сервера
};

/**
 * @brief Класс для управления аутентификацией и регистрацией пользователей.
 * 
 * Использует libsodium для хэширования паролей (Argon2).
 * Поддерживает двухфакторную авторизацию через email.
 */
class Auth {
private:
    Database& db;  ///< Ссылка на объект базы данных

    /**
     * @brief Проверяет соответствие пароля хэшу.
     * @param password Введённый пароль
     * @param hash Хэш из базы данных
     * @return true Пароль верен
     * @return false Пароль неверен
     */
    bool verifyPassword(const std::string& password, const std::string& hash);

    std::string m_pendingEmail;  ///< Email, ожидающий верификации
    std::string m_pendingName;   ///< Имя пользователя, ожидающего верификации

public:
    /**
     * @brief Конструктор класса авторизации.
     * @param database Ссылка на объект базы данных
     */
    Auth(Database& database);

    /**
     * @brief Хэширует пароль с использованием Argon2.
     * @param password Исходный пароль
     * @return std::string Хэш пароля
     * @throws std::runtime_error Ошибка хэширования
     */
    std::string hashPassword(const std::string& password);

    /**
     * @brief Выполняет вход пользователя в систему.
     * 
     * Проверяет логин и пароль. При успехе отправляет код подтверждения на email.
     * 
     * @param login Логин пользователя
     * @param password Пароль пользователя
     * @return AuthResult Результат авторизации
     *   - UserNotFound: Пользователь не найден
     *   - WrongPassword: Неверный пароль
     *   - NeedVerification: Пароль верен, отправлен код
     *   - Error: Ошибка БД или отправки email
     */
    AuthResult loginUser(const std::string& login,
                         const std::string& password);

    /**
     * @brief Проверяет код двухфакторной авторизации.
     * @param code Код, полученный по email
     * @return true Код верен и не истек
     * @return false Код неверен или истек
     */
    bool verifyLoginCode(const std::string& code);
};