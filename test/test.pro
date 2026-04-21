# ─────────────────────────────────────────────
# Qt modules
# ─────────────────────────────────────────────
QT += core gui widgets

# ─────────────────────────────────────────────
# Compiler config
# ─────────────────────────────────────────────
CONFIG += c++17
CONFIG += warn_on

# приложение с GUI
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# ─────────────────────────────────────────────
# Project info
# ─────────────────────────────────────────────
TARGET = client
TEMPLATE = app

# ─────────────────────────────────────────────
# Sources
# ─────────────────────────────────────────────
SOURCES += \
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

# можно включить ещё жёстче при желании:
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# ─────────────────────────────────────────────
# Output directory (безопасный вариант)
# ─────────────────────────────────────────────
# если хочешь кастомную папку сборки — раскомментируй
# DESTDIR = $$PWD/../build

# ─────────────────────────────────────────────
# Install section (optional)
# ─────────────────────────────────────────────
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target