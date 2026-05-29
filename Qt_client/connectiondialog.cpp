#include "connectiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpression>
#include <QIntValidator>

ConnectionDialog::ConnectionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Подключение к серверу");
    setFixedSize(380, 200);
    setStyleSheet(
        "QDialog { background:#0b1e2d; color:#e6f2ff; font-family:'Segoe UI'; font-weight:bold; }"
        "QLineEdit {"
        " background:#102a43; color:#e6f2ff; border:1px solid #1f4e79;"
        " border-radius:8px; padding:8px; font-size:13pt; font-weight:bold;"
        "}"
        "QLineEdit:focus { border:1px solid #3da9fc; }"
        "QPushButton {"
        " background:#3da9fc; color:white; border:none;"
        " border-radius:8px; padding:8px; font-size:12pt; font-weight:bold;"
        "}"
        "QPushButton:hover { background:#74c0fc; }"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(24, 24, 24, 24);

    auto *title = new QLabel("Введите IP-адрес сервера", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:13pt; border:none;");
    layout->addWidget(title);

    ipEdit = new QLineEdit(this);
    ipEdit->setPlaceholderText("192.168.1.100");
    ipEdit->setText("127.0.0.1");
    ipEdit->setMaxLength(15);
    layout->addWidget(ipEdit);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color:#ff6b6b; font-size:10pt; border:none;");
    errorLabel->hide();
    layout->addWidget(errorLabel);

    connectBtn = new QPushButton("Подключиться", this);
    connect(connectBtn, &QPushButton::clicked, this, &ConnectionDialog::onConnect);
    connect(ipEdit, &QLineEdit::returnPressed, this, &ConnectionDialog::onConnect);
    layout->addWidget(connectBtn);
}

QString ConnectionDialog::serverIP() const
{
    return ipEdit->text().trimmed();
}

void ConnectionDialog::onConnect()
{
    QString ip = ipEdit->text().trimmed();

    QRegularExpression re("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
    if (!re.match(ip).hasMatch()) {
        errorLabel->setText("Неверный формат IP-адреса");
        errorLabel->show();
        return;
    }

    accept();
}
