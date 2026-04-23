#include "regwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

static QString styleBase =
    "background:#0b1e2d;color:#e6f2ff;font-family:'Segoe UI';";

RegWidget::RegWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(styleBase);
    setupUI();
}

void RegWidget::setupUI()
{
    auto *outer = new QVBoxLayout(this);
    outer->addStretch();

    auto *row = new QHBoxLayout();
    row->addStretch();

    QWidget *card = new QWidget(this);
    card->setFixedWidth(360);
    card->setStyleSheet("background:#102a43;border:1px solid #1f4e79;border-radius:10px;");

    auto *layout = new QVBoxLayout(card);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText("login");

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("password");

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("email");

    errorLabel = new QLabel();
    errorLabel->setStyleSheet("border:none; color:#ff6b6b;");

    okLabel = new QLabel();
    okLabel->setStyleSheet("border:none; color:#4cd4b0;");

    btnRegister = new QPushButton("Register");
    btnCode = new QPushButton("Send code");
    btnVerify = new QPushButton("Verify");

    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(emailEdit);
    layout->addWidget(errorLabel);
    layout->addWidget(okLabel);
    layout->addWidget(btnRegister);
    layout->addWidget(btnCode);

    layout->addWidget(btnVerify); // если что добавляем ее этим
    //btnVerify = new QPushButton("Verify");
    //btnVerify->hide();

    connect(btnRegister, &QPushButton::clicked, this, [this]() {
        if (loginEdit->text().length() < 4) {
            errorLabel->setText("login too short");
            return;
        }
        okLabel->setText("login OK");
    });

    connect(btnCode, &QPushButton::clicked, this, [this]() {

        errorLabel->clear();
        okLabel->clear();

        // здесь позже будет реальная отправка email-кода
    });

    connect(btnVerify, &QPushButton::clicked, this, [this]() {
        okLabel->setText("SUCCESS (mock)");

        QTimer::singleShot(500, this, [this]() {
            emit registrationSuccess(loginEdit->text());
        });
    });

    row->addWidget(card);
    row->addStretch();

    outer->addLayout(row);
    outer->addStretch();
}

void RegWidget::onShowAuthClicked()
{
    emit showAuth();
}