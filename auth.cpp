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

        // Хэшурем пароль и сохраняем его
        std::string hash = hashPassword(password);

        txn.exec_params(
            "INSERT INTO users (name, login, hash, email) VALUES ($1, $2, $3, $4)",
            name, login, hash, email
        );

        // Фиксируется полное выполнение транзакции и происходит запись данных в БД 
        txn.commit(); 

        std::cout << "User registered\n";
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
            "SELECT hash FROM users WHERE login=$1",
            login
        );

        // Не нашли
        if (r.empty()) {
            std::cout << "User not found\n";
            return false;
        }

        // Проверка пароля
        std::string stored_hash = r[0]["hash"].c_str();

        if (verifyPassword(password, stored_hash)) {
            std::cout << "Login successful\n";
            return true;
        }

        std::cout << "Wrong password\n";
        return false;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}