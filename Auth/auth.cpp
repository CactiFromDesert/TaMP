#include "auth.h"

Auth::Auth(Database& database) : db(database) {} // Конструктор сохраняет ссылку на Database

// Хэширование пароля 
std::string Auth::hashPassword(const std::string& password) {
    char hash[crypto_pwhash_STRBYTES]; // Буфер для хэша

    if (crypto_pwhash_str(
            hash,
            password.c_str(),
            password.length(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        throw std::runtime_error("Hashing failed");
    }

    return std::string(hash); // Возвращаем уже хэш
}

// Проверка пароля
bool Auth::verifyPassword(const std::string& password, const std::string& hash) {
    return crypto_pwhash_str_verify(
               hash.c_str(),
               password.c_str(),
               password.length()) == 0;
}

// Регистрация нового пользователя
bool Auth::registerUser(const std::string& name,
                        const std::string& login,
                        const std::string& password,
                        const std::string& email)
{
    try {
        // Проверка формата email
        if (!EmailService::isValidEmail(email)) {
            std::cout << "Invalid email format\n";
            return false;
        }
        
        pqxx::work txn(db.getConnection()); // Используем подключение из db

        // Проверка уникальности
        pqxx::result r = txn.exec_params(
            "SELECT 1 FROM users WHERE login=$1 OR email=$2",
            login, email
        );

        // Если пользователь существует
        if (!r.empty()) {
            std::cout << "User already exists\n";
            return false;
        }

        // Отправляем код верификации на email ДО создания пользователя
        std::cout << "Sending verification code to " << email << "...\n";
        if (!EmailService::sendVerificationEmail(email, name)) {
            std::cout << "Failed to send verification email. Registration cancelled.\n";
            return false;
        }
        
        std::cout << "Verification code sent! Please check your email.\n";
        
        // Запрашиваем код у пользователя
        std::string code;
        int attempts = 0;
        const int MAX_ATTEMPTS = 3;
        bool verified = false;
        
        while (attempts < MAX_ATTEMPTS && !verified) {
            std::cout << "\nEnter verification code from email (or 'cancel' to abort): ";
            std::getline(std::cin, code);
            
            if (code == "cancel") {
                std::cout << "Registration cancelled.\n";
                return false;
            }
            
            if (EmailService::verifyCode(email, code)) {
                verified = true;
                std::cout << "Email verified successfully!\n";
            } else {
                attempts++;
                if (attempts < MAX_ATTEMPTS) {
                    std::cout << "Invalid code. " << (MAX_ATTEMPTS - attempts) << " attempts remaining.\n";
                }
            }
        }
        
        if (!verified) {
            std::cout << "Too many failed attempts. Registration failed.\n";
            return false;
        }
        
        // Только после успешной верификации создаем пользователя
        std::string hash = hashPassword(password);

        txn.exec_params(
            "INSERT INTO users (name, login, hash, email) VALUES ($1, $2, $3, $4)",
            name, login, hash, email
        );

        // Фиксируется полное выполнение транзакции и происходит запись данных в БД 
        txn.commit(); 

        std::cout << "User registered successfully!\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// Вход пользователя
bool Auth::loginUser(const std::string& login,
                     const std::string& password)
{
    try {
        pqxx::work txn(db.getConnection()); // Используем подключение

        // Ищем по логину
        pqxx::result r = txn.exec_params(
            "SELECT hash, email, name FROM users WHERE login=$1",
            login
        );

        // Не нашли
        if (r.empty()) {
            std::cout << "User not found\n";
            return false;
        }

        // Проверка пароля
        std::string stored_hash = r[0]["hash"].c_str();
        std::string email = r[0]["email"].c_str();
        std::string name = r[0]["name"].c_str();

        if (!verifyPassword(password, stored_hash)) {
            std::cout << "Wrong password\n";
            return false;
        }
        
        // Требуем верификацию email
        std::cout << "\n=== Email Verification Required ===\n";
        std::cout << "A verification code has been sent to: " << email << "\n";
        
        // Отправляем код
        if (!EmailService::sendVerificationEmail(email, name)) {
            std::cout << "Failed to send verification code. Please try again.\n";
            return false;
        }
        
        // Запрашиваем код
        std::string code;
        int attempts = 0;
        const int MAX_ATTEMPTS = 3;
        
        while (attempts < MAX_ATTEMPTS) {
            std::cout << "\nEnter verification code (or 'cancel' to abort): ";
            std::getline(std::cin, code);
            
            if (code == "cancel") {
                std::cout << "Login cancelled\n";
                return false;
            }
            
            if (EmailService::verifyCode(email, code)) {
                std::cout << "Login successful!\n";
                return true;
            }
            
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                std::cout << "Invalid code. " << (MAX_ATTEMPTS - attempts) << " attempts remaining.\n";
            }
        }
        
        std::cout << "Too many failed attempts. Login failed.\n";
        return false;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}