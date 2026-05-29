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

struct PendingRegistration {
    std::string name;
    std::string login;
    std::string password;
    std::string email;
};

class Auth {
private:
    Database& db;

    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);

    std::string m_pendingLoginEmail;
    std::string m_pendingLoginName;

    PendingRegistration m_pendingReg;

public:
    Auth(Database& database);

    AuthResult loginUser(const std::string& login,
                         const std::string& password);

    bool verifyLoginCode(const std::string& code);

    bool registerRequest(const std::string& name,
                         const std::string& login,
                         const std::string& password,
                         const std::string& email);

    bool verifyRegister(const std::string& code);

    bool resetRequest(const std::string& email);
    bool resetVerify(const std::string& email, const std::string& code);
    bool resetFinish(const std::string& email, const std::string& new_password);
};
