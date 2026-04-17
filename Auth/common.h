#pragma once

#include <iostream>
#include <string>

// Для БД, авторизация и регистрация
#include <pqxx/pqxx> // Подключение к БД 
#include <sodium.h> // Хэширование паролей (Argon2)
#include <stdexcept> // Исключения для ошибок
#include <limits> // Лимиты числовых типов

// Добавлено для email функционала
#include <random> // Для кода верификации
#include <chrono> // Для времени кода верификации
#include <iomanip> // Форматирование вывода
#include <sstream> // Работа с потоками строк
#include <regex> // проверка формата email
#include <curl/curl.h> // Отправка email через SMTP