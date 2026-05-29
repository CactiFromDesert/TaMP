#include "auth.h"
#include "database.h"
#include "email_service.h"
#include "common.h"

#include <cstring>
#include <sstream>
#include <map>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 11998
#define BUFFER_SIZE 4096

static std::string conn_str =
    "postgresql://postgres:1234@postgres:5432/usersauth";

static std::string jsonStr(const std::string& key, const std::string& value)
{
    return "\"" + key + "\":\"" + value + "\"";
}

static std::string jsonStatus(const std::string& status)
{
    return "{" + jsonStr("status", status) + "}";
}

static std::string jsonError(const std::string& message)
{
    return "{\"status\":\"error\"," + jsonStr("message", message) + "}";
}

static std::string jsonOk()
{
    return jsonStatus("ok");
}

static std::string extractStr(const std::string& json, const std::string& key)
{
    std::string search = "\"" + key + "\":\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos += search.size();
    size_t end = json.find("\"", pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

struct Request {
    std::string type;
    std::map<std::string, std::string> data;
};

static Request parseRequest(const std::string& json)
{
    Request req;
    req.type = extractStr(json, "type");

    size_t dataPos = json.find("\"data\":{");
    if (dataPos != std::string::npos) {
        size_t start = dataPos + 8;
        int depth = 1;
        size_t end = start;
        while (end < json.size() && depth > 0) {
            if (json[end] == '{') depth++;
            else if (json[end] == '}') depth--;
            end++;
        }
        std::string dataPart = json.substr(start, end - start - 1);
        std::string knownKeys[] = {"name","login","password","email","code","new_password"};
        for (const auto& k : knownKeys) {
            std::string v = extractStr(dataPart, k);
            if (!v.empty()) req.data[k] = v;
        }
    }
    return req;
}

struct ThreadArg {
    int sock;
};

static void* clientThread(void* arg)
{
    ThreadArg* ta = static_cast<ThreadArg*>(arg);
    int clientSock = ta->sock;
    delete ta;

    try {
        Database db(conn_str);
        Auth auth(db);

        char buffer[BUFFER_SIZE];
        std::string accumulated;

        while (true) {
            ssize_t n = read(clientSock, buffer, BUFFER_SIZE - 1);
            if (n <= 0) break;

            buffer[n] = 0;
            accumulated += buffer;

            size_t pos;
            while ((pos = accumulated.find('\n')) != std::string::npos) {
                std::string line = accumulated.substr(0, pos);
                accumulated.erase(0, pos + 1);

                if (line.empty()) continue;

                Request req = parseRequest(line);
                std::string response;

                std::cout << "[REQ] type=" << req.type << std::endl;
                for (auto& [k,v] : req.data) {
                    std::cout << "  " << k << "=" << v << std::endl;
                }

                if (req.type == "login") {
                    AuthResult r = auth.loginUser(req.data["login"], req.data["password"]);
                    if (r == AuthResult::NeedVerification) {
                        response = jsonStatus("need_verification");
                    } else if (r == AuthResult::UserNotFound) {
                        response = jsonError("User not found");
                    } else if (r == AuthResult::WrongPassword) {
                        response = jsonError("Wrong password");
                    } else {
                        response = jsonError("Login failed");
                    }
                }
                else if (req.type == "verify_login") {
                    if (auth.verifyLoginCode(req.data["code"])) {
                        response = jsonOk();
                    } else {
                        response = jsonError("Invalid or expired code");
                    }
                }
                else if (req.type == "register") {
                    if (auth.registerRequest(
                            req.data["name"],
                            req.data["login"],
                            req.data["password"],
                            req.data["email"])) {
                        response = jsonOk();
                    } else {
                        response = jsonError("Registration failed (login or email may already exist)");
                    }
                }
                else if (req.type == "verify_register") {
                    if (auth.verifyRegister(req.data["code"])) {
                        response = jsonOk();
                    } else {
                        response = jsonError("Invalid or expired code");
                    }
                }
                else if (req.type == "reset_request") {
                    if (auth.resetRequest(req.data["email"])) {
                        response = jsonOk();
                    } else {
                        response = jsonError("Email not found");
                    }
                }
                else if (req.type == "reset_verify") {
                    if (auth.resetVerify(req.data["email"], req.data["code"])) {
                        response = jsonOk();
                    } else {
                        response = jsonError("Invalid or expired code");
                    }
                }
                else if (req.type == "reset_finish") {
                    if (auth.resetFinish(req.data["email"], req.data["new_password"])) {
                        response = jsonOk();
                    } else {
                        response = jsonError("Password reset failed");
                    }
                }
                else {
                    response = jsonError("Unknown request type");
                }

                std::cout << "[RES] " << response.substr(0, 200) << std::endl;

                response += "\n";
                write(clientSock, response.c_str(), response.size());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Client thread error: " << e.what() << std::endl;
    }

    close(clientSock);
    return nullptr;
}

int main()
{
    std::cout << "=== Auth TCP Server starting on port " << PORT << " ===" << std::endl;

    if (sodium_init() < 0) {
        std::cerr << "ERROR: Failed to initialize libsodium" << std::endl;
        return 1;
    }

    EmailService::clearAllCodes();

    try {
        Database db(conn_str);
        Auth auth(db);

        std::cout << "Testing email service..." << std::endl;
        if (!EmailService::testSmtpConnection()) {
            std::cout << "WARNING: Email service not available." << std::endl;
        } else {
            std::cout << "Email service ready." << std::endl;
        }

        int serverSock = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSock < 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return 1;
        }

        int opt = 1;
        setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(PORT);

        if (bind(serverSock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Failed to bind port " << PORT << std::endl;
            return 1;
        }

        listen(serverSock, 5);
        std::cout << "Auth TCP Server listening on port " << PORT << std::endl;

        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientLen);
            if (clientSock < 0) {
                std::cerr << "Accept failed" << std::endl;
                continue;
            }

            std::cout << "Client connected from: "
                      << inet_ntoa(clientAddr.sin_addr) << std::endl;

            ThreadArg* ta = new ThreadArg;
            ta->sock = clientSock;
            pthread_t thread;
            pthread_create(&thread, nullptr, clientThread, ta);
            pthread_detach(thread);
        }

        close(serverSock);
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
