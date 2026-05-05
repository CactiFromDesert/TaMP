# Инструкция по запуску через докер

## 1. Клонировать проект
cd путь до папки где лежит корень проекта

git clone https://github.com/CactiFromDesert/TaMP.git

ИЛИ (если уже был скачан проект)

git pull origin main

## 2. Запустить сервер и БД
docker compose up -d postgres

docker compose up calculation_server
## 3. Запустить клиент (новый терминал, старый остается открытым)
docker compose --profile client up calculation_client

## 4. Запустить аутентификацию (новый терминал)
docker compose run --rm auth

## 5. Остановить всё (не удаляя)
Остановить выполнение программы - ctrl + C 

docker compose stop calc_server

docker compose stop calc_server

docker compose stop auth_postgres


## 5.1. Остановить и удалить всё
docker compose down


main/
├── .gitignore
├── DEVL0G.md
├── README.md
├── docker-compose.yml
├── init.sql
│
├── Auth/                        # Сервис аутентификации (C++, Docker)
│   ├── Dockerfile.auth
│   ├── auth.cpp / auth.h
│   ├── common.h
│   ├── database.cpp / database.h
│   ├── email_config.h
│   ├── email_service.cpp / email_service.h
│   └── main.cpp
│
├── Client_Server/               # TCP клиент-сервер (C++, Docker)
│   ├── for_tests/
│   ├── Dockerfile.client
│   ├── Dockerfile.server
│   ├── Makefile
│   ├── TCPClient.cpp / TCPClient.hpp
│   ├── TCPServer.cpp / TCPServer.hpp
│   ├── client/
│   ├── server/
│   ├── main_client.cpp
│   └── main_server.cpp
│
├── Funcoin/                     # Динамическая библиотека (C++)
│   ├── Makefile
│   ├── func.cpp / func.hpp
│   ├── main.cpp
│   └── main(all).cpp
│
└── test/                        # GUI-приложение (Qt, виджеты)
    ├── .qtcreator/
    ├── test.pro
    ├── main.cpp
    ├── mainwindow.cpp / mainwindow.h
    ├── authwidget.cpp / authwidget.h
    ├── regwidget.cpp / regwidget.h
    ├── resetwidget.cpp / resetwidget.h
    ├── verifywidget.cpp / verifywidget.h
    ├── graphwidget.cpp / graphwidget.h
    ├── taskdialog.cpp / taskdialog.h
    ├── schemalog.cpp / schemalog.h
    └── clientsingleton.cpp / clientsingleton.h
