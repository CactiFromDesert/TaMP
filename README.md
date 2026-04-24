# Инструкция по запуску через докер

## 1. Клонировать проект
cd folder

git clone url

## 2. Запустить сервер и БД
docker compose up -d postgres

docker compose up calculation_server
## 3. Запустить клиент (новый терминал, старый остается открытым)
docker compose --profile client up calculation_client

## 4. Запустить аутентификацию (новый терминал)
docker compose run --rm auth

## 3. Остановить всё (не удаляя)
Остановить выполнение программы - ctrl + C 

docker compose stop calc_server

docker compose stop calc_server

docker compose stop auth_postgres

## 6. Остановить и удалить всё
docker compose down
