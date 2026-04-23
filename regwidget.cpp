#include "regwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

// ── Sky Blue UI palette ───────────────────────

#define UI_BG         "#0b1e2d"
#define UI_CARD       "#102a43"
#define UI_BORDER     "#1f4e79"
#define UI_TEXT       "#e6f2ff"
#define UI_MUTED      "#9ecae6"

#define UI_PRIMARY    "#3da9fc"
#define UI_PRIMARY_H  "#74c0fc"

#define UI_ERROR      "#ff6b6b"
#define UI_SUCCESS    "#4cd4b0"

#define FONT_FAMILY   "Segoe UI"

static QString inputStyle()
{
    return QString(
               "QLineEdit {"
               "background:%1;"
               "color:%2;"
               "border:1px solid %3;"
               "border-radius:8px;"
               "padding:6px;"
               "font-family:'Segoe UI';"
               "font-weight: bold;"
               "font-size: 12pt;"
               "}"
               "QLineEdit:focus {"
               "border:1px solid %4;"
               "}")
        .arg(UI_BG)
        .arg(UI_TEXT)
        .arg(UI_BORDER)
        .arg(UI_PRIMARY);
}

static QString btnStylePrimary()
{
    return QString(
               "QPushButton {"
               "background:%1;"
               "color:white;"
               "border:none;"
               "border-radius:8px;"
               "padding:7px;"
               "font-weight: bold;"
               "font-size: 12pt;"
               "}"
               "QPushButton:hover {"
               "background:%2;"
               "}")
        .arg(UI_PRIMARY)
        .arg(UI_PRIMARY_H);
}

static QString btnStyleSecondary()
{
    return QString(
               "QPushButton {"
               "background:transparent;"
               "color:%1;"
               "border:1px solid %2;"
               "border-radius:8px;"
               "padding:6px;"
               "font-weight: bold;"
               "font-size: 12pt;"
               "}"
               "QPushButton:hover {"
               "background:%3;"
               "}")
        .arg(UI_TEXT)
        .arg(UI_BORDER)
        .arg(UI_BORDER);
}

RegWidget::RegWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(QString(
                      "background:%1;"
                      "color:%2;"
                      "font-family:'Segoe UI';"
                      "font-weight: bold;"
                      "font-size: 12pt;"
                      ).arg(UI_BG).arg(UI_TEXT));
    setupUI();
}

void RegWidget::setupUI()
{
    auto *outer = new QVBoxLayout(this);
    outer->addStretch();

    auto *row = new QHBoxLayout();
    row->addStretch();

    QWidget *card = new QWidget(this);
    card->setFixedWidth(340);

    card->setStyleSheet(QString(
                            "background:%1;"
                            "border:1px solid %2;"
                            "border-radius:12px;"
                            "font-weight: bold;"
                            ).arg(UI_CARD).arg(UI_BORDER));

    auto *layout = new QVBoxLayout(card);
    layout->setSpacing(10);
    layout->setContentsMargins(20,20,20,20);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText("Login");
    loginEdit->setStyleSheet(inputStyle());

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setStyleSheet(inputStyle());

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("Email");
    emailEdit->setStyleSheet(inputStyle());

    errorLabel = new QLabel();
    errorLabel->setStyleSheet(QString(
                                  "border:none;"
                                  "color:%1;"
                                  "font-size:12pt;"
                                  "font-weight: bold;"
                                  ).arg(UI_ERROR));
    errorLabel->hide();

    okLabel = new QLabel();
    okLabel->setStyleSheet(QString(
                               "border:none;"
                               "color:%1;"
                               "font-size:12pt;"
                               "font-weight: bold;"
                               ).arg(UI_SUCCESS));
    okLabel->hide();

    btnRegister = new QPushButton("Register");
    btnRegister->setStyleSheet(btnStylePrimary());

    btnCode = new QPushButton("Send code");
    btnCode->setStyleSheet(btnStyleSecondary());

    btnVerify = new QPushButton("Verify");
    btnVerify->setStyleSheet(btnStylePrimary());
    btnVerify->hide();

    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(emailEdit);
    layout->addWidget(errorLabel);
    layout->addWidget(okLabel);
    layout->addWidget(btnRegister);
    layout->addWidget(btnCode);
    layout->addWidget(btnVerify);

    connect(btnRegister, &QPushButton::clicked, this, [this]() {
        if (loginEdit->text().length() < 4) {
            errorLabel->setText("Login too short (min 4 chars)");
            errorLabel->show();
            okLabel->hide();
            return;
        }
        errorLabel->hide();
        okLabel->setText("Login OK");
        okLabel->show();
    });

    connect(btnCode, &QPushButton::clicked, this, [this]() {
        errorLabel->hide();
        okLabel->hide();
        okLabel->setText("Code sent to email (mock)");
        okLabel->show();
        btnVerify->show();
    });

    connect(btnVerify, &QPushButton::clicked, this, [this]() {
        okLabel->setText("Registration successful!");
        okLabel->show();

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