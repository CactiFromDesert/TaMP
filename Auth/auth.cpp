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

        if (!EmailService::sendVerificationEmail(email, name)) {
            return AuthResult::Error;
        }

        m_pendingLoginEmail = email;
        m_pendingLoginName  = name;

        return AuthResult::NeedVerification;

    } catch (...) {
        return AuthResult::Error;
    }
}

bool Auth::verifyLoginCode(const std::string& code)
{
    if (m_pendingLoginEmail.empty())
        return false;

    return EmailService::verifyCode(m_pendingLoginEmail, code);
}

bool Auth::registerRequest(const std::string& name,
                           const std::string& login,
                           const std::string& password,
                           const std::string& email)
{
    try {
        pqxx::work txn(db.getConnection());
        pqxx::result r = txn.exec_params(
            "SELECT id FROM users WHERE login=$1 OR email=$2", login, email
        );
        if (!r.empty()) return false;

        if (!EmailService::sendVerificationEmail(email, name)) return false;

        m_pendingReg.name = name;
        m_pendingReg.login = login;
        m_pendingReg.password = password;
        m_pendingReg.email = email;

        return true;
    } catch (...) {
        return false;
    }
}

bool Auth::verifyRegister(const std::string& code)
{
    if (m_pendingReg.email.empty()) return false;

    if (!EmailService::verifyCode(m_pendingReg.email, code)) return false;

    try {
        std::string hash = hashPassword(m_pendingReg.password);
        pqxx::work txn(db.getConnection());
        txn.exec_params(
            "INSERT INTO users (name, login, hash, email) VALUES ($1, $2, $3, $4)",
            m_pendingReg.name, m_pendingReg.login, hash, m_pendingReg.email
        );
        txn.commit();

        std::string email = m_pendingReg.email;
        m_pendingReg = PendingRegistration();
        return true;
    } catch (...) {
        return false;
    }
}

bool Auth::resetRequest(const std::string& email)
{
    try {
        pqxx::work txn(db.getConnection());
        pqxx::result r = txn.exec_params(
            "SELECT name FROM users WHERE email=$1", email
        );
        if (r.empty()) return false;

        std::string name = r[0]["name"].c_str();
        return EmailService::sendVerificationEmail(email, name);
    } catch (...) {
        return false;
    }
}

bool Auth::resetVerify(const std::string& email, const std::string& code)
{
    return EmailService::verifyCode(email, code);
}

bool Auth::resetFinish(const std::string& email, const std::string& new_password)
{
    try {
        std::string hash = hashPassword(new_password);
        pqxx::work txn(db.getConnection());
        pqxx::result r = txn.exec_params(
            "UPDATE users SET hash=$1 WHERE email=$2", hash, email
        );
        txn.commit();
        return r.affected_rows() > 0;
    } catch (...) {
        return false;
    }
}
