# Инструкция по запуску через докер

## Предварительные требования

### Установка XLaunch (VcXsrv Windows X Server)

Для запуска Qt клиента в Docker на Windows необходимо установить X-сервер:

1. Скачайте XLaunch с официального сайта: https://sourceforge.net/projects/vcxsrv/
2. Установите VcXsrv (выберите версию, соответствующую вашей системе: x64)
3. После установки запустите **XLaunch**
4. В настройках укажите:
   - Display settings: Multiple Windows
   - Display number: 0
   - Start no client
   - Дополнительно: отключите Access Control (Native opengl может понадобиться)
5. Нажмите "Save configuration" если хотите сохранить настройки
6. Завершите настройку и нажмите "Finish"


## Часть 1: Первый запуск (с нуля)

### 1. Клонировать проект

cd /путь/до/папки/где/будет/проект

git clone https://github.com/CactiFromDesert/TaMP.git

### 2. Билд

docker compose build

### 3. Запустить сервер и БД - В одном терминале (порядок важен)
docker compose up -d postgres

docker compose up calculation_server

### 4. Запустить клиент (новый терминал, старый остается открытым)

docker compose up qt_client


## Часть 2. Не первый запуск (уже было установлено, но выключено)

### 1. Заходим в папку

cd /путь/до/папки/где/проект

### 2. Запустить сервер и БД - В одном терминале (порядок важен)
docker compose up -d postgres

docker compose up calculation_server

### 3. Запустить клиент (новый терминал, старый остается открытым)

docker compose up qt_client

## Часть 3. Остановить всё (не удаляя)
Остановить выполнение программы - ctrl + C 

docker compose stop calc_server

docker compose stop calc_server

docker compose stop auth_postgres


## Часть 3.1 Остановить и удалить всё
docker compose down
