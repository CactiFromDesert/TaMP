#include <QApplication>
#include <QMessageBox>
#include "clientsingleton.h"
#include "authclient.h"
#include "mainwindow.h"
#include "connectiondialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString serverIP;

    if (argc > 1) {
        serverIP = QString::fromLocal8Bit(argv[1]);
    } else {
        ConnectionDialog dlg;
        if (dlg.exec() != QDialog::Accepted)
            return 0;
        serverIP = dlg.serverIP();
    }

    if (!AuthClient::instance().connectToServer(serverIP, 11998)) {
        QMessageBox::critical(nullptr, "Ошибка",
            "Не удалось подключиться к auth-серверу (" + serverIP + ":11998).\n"
            "Убедитесь, что Docker-контейнеры запущены.");
    }

    if (!ClientSingleton::instance().connectToServer(serverIP, 11999)) {
        QMessageBox::critical(nullptr, "Ошибка",
            "Не удалось подключиться к calc-серверу (" + serverIP + ":11999).\n"
            "Убедитесь, что Docker-контейнеры запущены.");
    }

    MainWindow w;
    w.resize(1000, 700);
    w.show();

    return a.exec();
}
