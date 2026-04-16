#pragma once

#include <iostream>
#include <string>

#include <pqxx/pqxx> // Подключение к БД 
#include <sodium.h> // Хэширование паролей (Argon2)
#include <stdexcept> // Исключения для ошибок
#include <limits> // Лимиты числовых типов

// Добавлено для email функционала
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>
#include <curl/curl.h>