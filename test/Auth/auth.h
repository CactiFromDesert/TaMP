#pragma once

#include "common.h"
#include "database.h"
#include "email_service.h"

enum class AuthResult {
    Success,
    WrongPassword,
    UserNotFound,
    NeedVerification,
    Error
};

class Auth {
private:
    Database& db;
    bool verifyPassword(const std::string& password, const std::string& hash);

    std::string m_pendingEmail;
    std::string m_pendingName;

public:
    Auth(Database& database);
    std::string hashPassword(const std::string& password);
    AuthResult loginUser(const std::string& login,
                         const std::string& password);

    bool verifyLoginCode(const std::string& code);
};