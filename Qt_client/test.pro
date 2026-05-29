QT += core gui widgets network

CONFIG += c++17

TEMPLATE = app
TARGET = client

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
    clientsingleton.cpp \
    authclient.cpp

HEADERS += \
    mainwindow.h \
    authwidget.h \
    regwidget.h \
    verifywidget.h \
    graphwidget.h \
    taskdialog.h \
    schemadialog.h \
    resetwidget.h \
    clientsingleton.h \
    authclient.h

win32 {
    SCHEMA_SRC = $$shell_path($$PWD/Schema.png)
    CONFIG(debug, debug|release) {
        SCHEMA_DST = $$shell_path($$OUT_PWD/debug/)
    } else {
        SCHEMA_DST = $$shell_path($$OUT_PWD/release/)
    }
    QMAKE_POST_LINK += copy /Y \"$$SCHEMA_SRC\" \"$$SCHEMA_DST\" 2>&1
}
