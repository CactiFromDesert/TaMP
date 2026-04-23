#include "authwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
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

#define FONT_FAMILY   "Segoe UI"
#define FONT_SIZE_SMALL 9


AuthWidget::AuthWidget(QWidget *parent)
    : QWidget(parent),
    failedAttempts(0),
    lockLevel(0),
    isLocked(false),
    m_waitingForAuth(false)
{
    lockTimer = new QTimer(this);
    lockTimer->setSingleShot(true);
    connect(lockTimer, &QTimer::timeout,
            this, &AuthWidget::onLockTimerFired);

    setupUI();
}

AuthWidget::~AuthWidget() = default;


// ─────────────────────────────────────────────
// UI STYLE HELPERS
// ─────────────────────────────────────────────

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


// ─────────────────────────────────────────────
// UI
// ─────────────────────────────────────────────

void AuthWidget::setupUI()
{
    setStyleSheet(QString(
                      "background:%1;"
                      "color:%2;"
                      "font-family:'Segoe UI';"
                      "font-weight: bold;"
                      "font-size: 12pt;"
                      ).arg(UI_BG).arg(UI_TEXT));

    auto *outer = new QVBoxLayout(this);
    outer->addStretch();


    auto *card = new QWidget(this);
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


    loginEdit = new QLineEdit(card);
    loginEdit->setPlaceholderText("Login");
    loginEdit->setStyleSheet(inputStyle());


    passwordEdit = new QLineEdit(card);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(inputStyle());


    loginBtn = new QPushButton("Login", card);
    loginBtn->setStyleSheet(btnStylePrimary());


    registerBtn = new QPushButton("Register", card);
    registerBtn->setStyleSheet(btnStyleSecondary());


    forgotBtn = new QPushButton("Forgot password", card);
    forgotBtn->setStyleSheet(btnStyleSecondary());


    statusLabel = new QLabel(card);
    statusLabel->setStyleSheet(QString(
                                   "border:none;"
                                   "color:%1;"
                                   "font-size:12pt;"
                                   "font-weight: bold;"
                                   ).arg(UI_ERROR));

    statusLabel->hide();


    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginBtn);
    layout->addWidget(registerBtn);
    layout->addWidget(forgotBtn);
    layout->addWidget(statusLabel);


    connect(loginBtn,
            &QPushButton::clicked,
            this,
            &AuthWidget::onLoginClicked);

    connect(registerBtn,
            &QPushButton::clicked,
            this,
            &AuthWidget::onRegisterClicked);

    connect(forgotBtn,
            &QPushButton::clicked,
            this,
            &AuthWidget::onForgotClicked);


    outer->addWidget(card, 0, Qt::AlignCenter);
    outer->addStretch();
}


// ─────────────────────────────────────────────
// LOGIN (FAKE)
// ─────────────────────────────────────────────

void AuthWidget::onLoginClicked()
{
    QString login = loginEdit->text().trimmed();
    QString pass  = passwordEdit->text();

    if (login.isEmpty() || pass.isEmpty())
    {
        statusLabel->setText("Enter login & password");
        statusLabel->show();
        return;
    }

    loginBtn->setEnabled(false);

    statusLabel->setText("Logging in (UI mock)...");
    statusLabel->show();

    m_waitingForAuth = true;

    QTimer::singleShot(800, this, [this, login]()
                       {
                           m_waitingForAuth = false;
                           loginBtn->setEnabled(true);

                           statusLabel->setText("Login success (UI)");
                           statusLabel->show();

                           emit loginSuccess(login);
                       });
}


// ─────────────────────────────────────────────
// PASSWORD TOGGLE
// ─────────────────────────────────────────────

void AuthWidget::onTogglePassword()
{
    passwordEdit->setEchoMode(
        passwordEdit->echoMode() == QLineEdit::Password
            ? QLineEdit::Normal
            : QLineEdit::Password
        );
}


// ─────────────────────────────────────────────
// LOCK (stub)
// ─────────────────────────────────────────────

void AuthWidget::applyLock(int, const QString &) {}
void AuthWidget::onLockTimerFired() {}


// ─────────────────────────────────────────────
// NAVIGATION
// ─────────────────────────────────────────────

void AuthWidget::onRegisterClicked()
{
    emit showRegister();
}

void AuthWidget::onForgotClicked()
{
    emit showReset();
}


// ─────────────────────────────────────────────
// RESPONSE (stub)
// ─────────────────────────────────────────────

void AuthWidget::onAuthResponseReceived(const QString &) {}