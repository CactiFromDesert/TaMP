#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("ТиМП — Подгруппа 2");
    a.setApplicationVersion("1.0");

    // Make all text bold in the application
    a.setStyleSheet("font-weight: bold;");

    MainWindow w;
    w.setWindowTitle("UI Test Mode");
    w.resize(900, 600);
    w.show();

    return a.exec();
}