#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("ТМП — Подгруппа 2");
    a.setApplicationVersion("1.0");

    MainWindow w;
    w.setWindowTitle("UI Test Mode");
    w.resize(900, 600);
    w.show();

    return a.exec();
}