#include <QApplication>
#include <QMessageBox>
#include "clientsingleton.h"
#include "authclient.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!AuthClient::instance().connectToServer("127.0.0.1", 11998)) {
        QMessageBox::critical(nullptr, "Error",
            "Failed to connect to auth server (127.0.0.1:11998).\n"
            "Make sure Docker containers are running.");
    }

    if (!ClientSingleton::instance().connectToServer("127.0.0.1", 11999)) {
        QMessageBox::critical(nullptr, "Error",
            "Failed to connect to calculation server (127.0.0.1:11999).\n"
            "Make sure Docker containers are running.");
    }

    MainWindow w;
    w.resize(1000, 700);
    w.show();

    return a.exec();
}
