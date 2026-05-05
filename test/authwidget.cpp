#include "authwidget.h"
#include <QVBoxLayout>
#include <QTimer>

#define UI_BG "#0b1e2d"
#define UI_TEXT "#e6f2ff"
#define UI_PRIMARY "#3da9fc"
#define UI_PRIMARY_H "#74c0fc"
#define UI_BORDER "#1f4e79"

AuthWidget::AuthWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

AuthWidget::~AuthWidget() = default;

static QString inputStyle()
{
    return "QLineEdit {"
           "background:#0b1e2d;"
           "color:#e6f2ff;"
           "border:1px solid #1f4e79;"
           "border-radius:8px;"
           "padding:6px;"
           "font-weight:bold;"
           "}";
}

static QString btnStyle()
{
    return "QPushButton {"
           "background:#3da9fc;"
           "color:white;"
           "border:none;"
           "border-radius:8px;"
           "padding:8px;"
           "font-weight:bold;"
           "}"
           "QPushButton:hover {background:#74c0fc;}";
}

void AuthWidget::setupUI()
{
    auto *layout = new QVBoxLayout(this);

    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Login");
    loginEdit->setStyleSheet(inputStyle());

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(inputStyle());

    loginBtn = new QPushButton("Login", this);
    loginBtn->setStyleSheet(btnStyle());

    statusLabel = new QLabel(this);
    statusLabel->hide();

    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginBtn);
    layout->addWidget(statusLabel);

    connect(loginBtn, &QPushButton::clicked,
            this, &AuthWidget::onLoginClicked);
}

void AuthWidget::onLoginClicked()
{
    QString login = loginEdit->text();
    QString pass = passwordEdit->text();

    if (login.isEmpty() || pass.isEmpty()) {
        statusLabel->setText("Fill fields");
        statusLabel->show();
        return;
    }

    statusLabel->setText("Logging in...");
    statusLabel->show();

    QTimer::singleShot(500, this, [this, login]() {
        emit loginSuccess(login);
    });
}

void AuthWidget::onTogglePassword()
{
    passwordEdit->setEchoMode(
        passwordEdit->echoMode() == QLineEdit::Password
            ? QLineEdit::Normal
            : QLineEdit::Password
        );
}