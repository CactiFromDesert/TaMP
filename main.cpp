#include <QApplication>
#include <QMessageBox>

#include "mainwindow.h"
#include "database.h"
#include "email_service.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("ТиМП — Подгруппа 2");
    a.setApplicationVersion("1.0");
    a.setStyleSheet("font-weight: bold;");

    // ─── INIT BACKEND ─────────────────────────

    if (sodium_init() < 0) {
        QMessageBox::critical(nullptr, "Error", "libsodium init failed");
        return -1;
    }

    std::string conn_str =
        "postgresql://postgres:1234@postgres:5432/usersauth";

    Database *db = nullptr;

    try {
        db = new Database(conn_str);

        if (!EmailService::testSmtpConnection()) {
            qWarning("Email service not available");
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "DB Error", e.what());
        return -1;
    }

    // ─── UI ───────────────────────────────────

    MainWindow w(*db);
    w.resize(1000, 700);
    w.show();

    return a.exec();
}