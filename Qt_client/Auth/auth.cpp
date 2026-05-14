#include "auth.h"

Auth::Auth(Database& database) : db(database) {}

std::string Auth::hashPassword(const std::string& password) {
    char hash[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(
            hash,
            password.c_str(),
            password.length(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        throw std::runtime_error("Hashing failed");
    }

    return std::string(hash);
}

bool Auth::verifyPassword(const std::string& password, const std::string& hash) {
    return crypto_pwhash_str_verify(
               hash.c_str(),
               password.c_str(),
               password.length()) == 0;
}

AuthResult Auth::loginUser(const std::string& login,
                           const std::string& password)
{
    try {
        pqxx::work txn(db.getConnection());

        pqxx::result r = txn.exec_params(
            "SELECT hash, email, name FROM users WHERE login=$1",
            login
            );

        if (r.empty()) {
            return AuthResult::UserNotFound;
        }

        std::string stored_hash = r[0]["hash"].c_str();
        std::string email = r[0]["email"].c_str();
        std::string name = r[0]["name"].c_str();

        if (!verifyPassword(password, stored_hash)) {
            return AuthResult::WrongPassword;
        }

        // отправка кода
        if (!EmailService::sendVerificationEmail(email, name)) {
            return AuthResult::Error;
        }

        m_pendingEmail = email;
        m_pendingName  = name;

        return AuthResult::NeedVerification;

    } catch (...) {
        return AuthResult::Error;
    }
}

bool Auth::verifyLoginCode(const std::string& code)
{
    if (m_pendingEmail.empty())
        return false;

    return EmailService::verifyCode(m_pendingEmail, code);
}