# ─────────────────────────────────────────────
# Qt modules
# ─────────────────────────────────────────────
QT += core gui widgets network

# ─────────────────────────────────────────────
# Compiler config
# ─────────────────────────────────────────────
CONFIG += c++17
CONFIG += warn_on
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# ─────────────────────────────────────────────
# Project info
# ─────────────────────────────────────────────
TARGET = client
TEMPLATE = app

LIBS += -lcurl
# PostgreSQL
INCLUDEPATH += C:/libpqxx/include
LIBS += -LC:/libpqxx/lib -lpqxx -lpq

# libsodium
INCLUDEPATH += C:/libsodium/include
LIBS += -LC:/libsodium/lib -lsodium

# ─────────────────────────────────────────────
# 🔥 INCLUDE PATH (ВАЖНО)
# ─────────────────────────────────────────────
INCLUDEPATH += \
    $$PWD/Auth

# ─────────────────────────────────────────────
# Sources
# ─────────────────────────────────────────────
SOURCES += \
    Auth/auth.cpp \
    Auth/database.cpp \
    Auth/email_service.cpp \
    main.cpp \
    mainwindow.cpp \
    authwidget.cpp \
    regwidget.cpp \
    verifywidget.cpp \
    graphwidget.cpp \
    taskdialog.cpp \
    schemadialog.cpp \
    resetwidget.cpp \
    clientsingleton.cpp

# ─────────────────────────────────────────────
# Headers
# ─────────────────────────────────────────────
HEADERS += \
    Auth/auth.h \
    Auth/common.h \
    Auth/database.h \
    Auth/email_config.h \
    Auth/email_service.h \
    mainwindow.h \
    authwidget.h \
    regwidget.h \
    verifywidget.h \
    graphwidget.h \
    taskdialog.h \
    schemadialog.h \
    resetwidget.h \
    clientsingleton.h

# ─────────────────────────────────────────────
# Warnings
# ─────────────────────────────────────────────
DEFINES += QT_DEPRECATED_WARNINGS

# ─────────────────────────────────────────────
# Output directory (optional)
# ─────────────────────────────────────────────
# DESTDIR = $$PWD/../build

# ─────────────────────────────────────────────
# Install section (optional)
# ─────────────────────────────────────────────
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target