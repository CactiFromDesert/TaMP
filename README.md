# Инструкция по запуску через докер

## 1. Клонировать проект
cd <folder>
git clone <url>

## 2. Запустить сервер и БД
docker compose up -d postgres calculation_server

## 3. Запустить клиент (новый терминал, старый остается открытым)
docker compose --profile client up calculation_client

## 4. Запустить аутентификацию (новый терминал)
docker compose run --rm auth

## 5. Остановить всё
docker compose down