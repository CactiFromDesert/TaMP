#include "email_service.h"
#include <map>
#include <mutex>

// Статическое хранилище кодов в памяти
std::map<std::string, VerificationCode> EmailService::pending_codes;
static std::mutex codes_mutex; // Для потокобезопасности

// Генерация 6-значного кода верификации
std::string EmailService::generateVerificationCode() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);
    
    std::string code;
    for (int i = 0; i < 6; ++i) {
        code += std::to_string(dis(gen));
    }
    return code;
}

// Очистка просроченных кодов
void EmailService::cleanupExpiredCodes() {
    std::lock_guard<std::mutex> lock(codes_mutex);
    
    auto now = std::chrono::system_clock::now();
    auto it = pending_codes.begin();
    
    while (it != pending_codes.end()) {
        if (it->second.expires_at < now) {
            it = pending_codes.erase(it);
        } else {
            ++it;
        }
    }
}

// Отправка email с кодом верификации
bool EmailService::sendVerificationEmail(const std::string& to_email, 
                                         const std::string& user_name) {
    // Очищаем старые коды
    cleanupExpiredCodes();
    
    // Генерируем новый код
    std::string code = generateVerificationCode();
    
    // Сохраняем код в памяти на 10 минут
    {
        std::lock_guard<std::mutex> lock(codes_mutex);
        VerificationCode vc;
        vc.code = code;
        vc.expires_at = std::chrono::system_clock::now() + std::chrono::minutes(10);
        pending_codes[to_email] = vc;
    }
    
    // Формируем письмо
    EmailMessage msg;
    msg.to_email = to_email;
    msg.to_name = user_name;
    msg.subject = "Your Verification Code - Auth System";
    
    // Формируем тело письма
    std::ostringstream body;
    body << "Hello";
    if (!user_name.empty()) {
        body << " " << user_name;
    }
    body << ",\n\n";
    body << "Thank you for using our Auth System!\n\n";
    body << "Your verification code is: " << code << "\n\n";
    body << "This code will expire in 10 minutes.\n\n";
    body << "If you didn't request this code, please ignore this email.\n\n";
    body << "Best regards,\n";
    body << "Auth System Team\n";
    body << "This is an automated message, please do not reply.";
    
    msg.body = body.str();
    
    return sendEmail(msg);
}

// Проверка кода верификации
bool EmailService::verifyCode(const std::string& email, const std::string& code) {
    std::lock_guard<std::mutex> lock(codes_mutex);
    
    auto it = pending_codes.find(email);
    if (it == pending_codes.end()) {
        std::cout << "No verification code found for this email\n";
        return false;
    }
    
    // Проверяем не истек ли код
    auto now = std::chrono::system_clock::now();
    if (it->second.expires_at < now) {
        pending_codes.erase(it);
        std::cout << "Verification code has expired\n";
        return false;
    }
    
    // Проверяем код
    if (it->second.code != code) {
        std::cout << "Invalid verification code\n";
        return false;
    }
    
    // Код верный - удаляем его
    pending_codes.erase(it);
    return true;
}

// Проверка формата email с помощью регулярного выражения (упрощенная)
bool EmailService::isValidEmail(const std::string& email) {
    // Разрешены буквы, цифры, точки, подчеркивания, проценты, плюсы, дефисы
    // После @ - домен с точками
    const std::regex pattern(
        R"(^[a-zA-Z0-9][a-zA-Z0-9._%+-]*@[a-zA-Z0-9][a-zA-Z0-9.-]*\.[a-zA-Z]{2,}$)"
    );
    
    if (!std::regex_match(email, pattern)) {
        return false;
    }
    
    // Проверка на две точки подряд
    if (email.find("..") != std::string::npos) {
        return false;
    }
    
    // Проверка что @ только одна
    if (std::count(email.begin(), email.end(), '@') != 1) {
        return false;
    }
    
    size_t at_pos = email.find('@');
    std::string local_part = email.substr(0, at_pos);
    std::string domain_part = email.substr(at_pos + 1);
    
    // Локальная часть не может быть пустой или слишком длинной
    if (local_part.empty() || local_part.length() > 64) {
        return false;
    }
    
    // Домен не может быть пустым или слишком длинным
    if (domain_part.empty() || domain_part.length() > 255) {
        return false;
    }
    
    // Проверка что домен содержит хотя бы одну точку
    if (domain_part.find('.') == std::string::npos) {
        return false;
    }
    
    // Проверка что точка не в начале и не в конце домена
    if (domain_part.front() == '.' || domain_part.back() == '.') {
        return false;
    }
    
    return true;
}

// Callback для получения ответа от сервера
size_t EmailService::write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

// Callback для отправки данных на сервер
size_t EmailService::read_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    UploadStatus* status = static_cast<UploadStatus*>(userdata);
    
    if (!status->data || status->bytes_read >= status->data->size()) {
        return 0;
    }
    
    size_t available = status->data->size() - status->bytes_read;
    size_t max_write = size * nmemb;
    size_t to_write = (available < max_write) ? available : max_write;
    
    memcpy(ptr, status->data->data() + status->bytes_read, to_write);
    status->bytes_read += to_write;
    
    return to_write;
}

// Кодирование в Base64 для SMTP аутентификации
std::string EmailService::base64_encode(const std::string& input) {
    static const char* base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::string output;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    size_t input_len = input.length();
    const unsigned char* bytes_to_encode = reinterpret_cast<const unsigned char*>(input.c_str());
    
    while (input_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (i = 0; i < 4; i++)
                output += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        
        for (j = 0; j < i + 1; j++)
            output += base64_chars[char_array_4[j]];
        
        while (i++ < 3)
            output += '=';
    }
    
    return output;
}

// Получение текущей даты в формате RFC 822 для заголовка email
std::string EmailService::getRFC822Date() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&time);
    
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %z", tm);
    return std::string(buffer);
}

// Форматирование email сообщения в соответствии с RFC 822
std::string EmailService::formatEmailMessage(const EmailMessage& msg) {
    std::ostringstream email;
    
    // Заголовки
    email << "From: " << EmailConfig::FROM_NAME << " <" << EmailConfig::FROM_EMAIL << ">\r\n";
    email << "To: " << (msg.to_name.empty() ? msg.to_email : msg.to_name + " <" + msg.to_email + ">") << "\r\n";
    email << "Subject: " << msg.subject << "\r\n";
    email << "Date: " << getRFC822Date() << "\r\n";
    email << "MIME-Version: 1.0\r\n";
    email << "Content-Type: text/plain; charset=UTF-8\r\n";
    email << "Content-Transfer-Encoding: 8bit\r\n";
    email << "\r\n";
    
    // Тело сообщения
    email << msg.body << "\r\n";
    email << "\r\n.\r\n";
    
    return email.str();
}

// Отправка email сообщения через SMTP
bool EmailService::sendEmail(const EmailMessage& message) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }
    
    std::string formatted_message = formatEmailMessage(message);
    UploadStatus upload_status(&formatted_message);
    std::string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, ("smtp://" + EmailConfig::SMTP_SERVER + ":" + 
                                          std::to_string(EmailConfig::SMTP_PORT)).c_str());
    
    curl_easy_setopt(curl, CURLOPT_USERNAME, EmailConfig::SMTP_USERNAME.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, EmailConfig::SMTP_PASSWORD.c_str());
    
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + EmailConfig::FROM_EMAIL + ">").c_str());
    
    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, ("<" + message.to_email + ">").c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    
    if (EmailConfig::USE_TLS) {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    }
    
    if (!EmailConfig::VERIFY_SSL) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)EmailConfig::TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_status);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(curl);
    
    bool success = (res == CURLE_OK);
    if (!success) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "Email sent successfully to: " << message.to_email << std::endl;
    }
    
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    
    return success;
}

// Проверка соединения с SMTP сервером
bool EmailService::testSmtpConnection() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, ("smtp://" + EmailConfig::SMTP_SERVER + ":" + 
                                          std::to_string(EmailConfig::SMTP_PORT)).c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, EmailConfig::SMTP_USERNAME.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, EmailConfig::SMTP_PASSWORD.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK);
}