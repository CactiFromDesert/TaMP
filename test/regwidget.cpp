#include "regwidget.h"
#include "database.h"
#include "auth.h"
#include "email_service.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#define UI_BG         "#0b1e2d"
#define UI_CARD       "#102a43"
#define UI_BORDER     "#1f4e79"
#define UI_TEXT       "#e6f2ff"
#define UI_PRIMARY    "#3da9fc"
#define UI_PRIMARY_H  "#74c0fc"
#define UI_ERROR      "#ff6b6b"
#define UI_SUCCESS    "#4cd4b0"

static QString inputStyle()
{
    return QString(
        "QLineEdit {"
        "background:%1; color:%2; border:1px solid %3;"
        "border-radius:8px; padding:6px;"
        "font-family:'Segoe UI'; font-weight: bold; font-size: 12pt;"
        "}"
        "QLineEdit:focus { border:1px solid %4; }"
    ).arg(UI_BG, UI_TEXT, UI_BORDER, UI_PRIMARY);
}

static QString btnPrimary()
{
    return QString(
        "QPushButton { background:%1; color:white; border:none;"
        "border-radius:8px; padding:7px; font-weight: bold; font-size: 12pt; }"
        "QPushButton:hover { background:%2; }"
    ).arg(UI_PRIMARY, UI_PRIMARY_H);
}

static QString btnSecondary()
{
    return QString(
        "QPushButton { background:transparent; color:%1;"
        "border:1px solid %2; border-radius:8px; padding:6px;"
        "font-weight: bold; font-size: 12pt; }"
        "QPushButton:hover { background:%3; }"
    ).arg(UI_TEXT, UI_BORDER, UI_BORDER);
}

RegWidget::RegWidget(Database &db, QWidget *parent)
    : QWidget(parent), m_db(db), m_auth(db)
{
    setStyleSheet(QString("background:%1; color:%2; font-family:'Segoe UI'; font-weight: bold; font-size: 12pt;")
                  .arg(UI_BG, UI_TEXT));
    setupUI();
}

void RegWidget::setupUI()
{
    auto *outer = new QVBoxLayout(this);
    outer->addStretch();

    auto *card = new QWidget(this);
    card->setMinimumWidth(340);
    card->setMaximumWidth(500);
    card->setStyleSheet(QString("background:%1; border:1px solid %2; border-radius:12px; font-weight: bold;")
                        .arg(UI_CARD, UI_BORDER));

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(8);
    cardLayout->setContentsMargins(20, 20, 20, 20);

    // ── STEP 1 ──────────────────────────────
    step1Widget = new QWidget(card);
    auto *s1 = new QVBoxLayout(step1Widget);
    s1->setSpacing(8);

    auto *title1 = new QLabel("Registration", step1Widget);
    title1->setAlignment(Qt::AlignCenter);
    title1->setStyleSheet("border:none; font-size:14pt; font-weight: bold;");
    s1->addWidget(title1);

    nameEdit = new QLineEdit(step1Widget);
    nameEdit->setPlaceholderText("Name");
    nameEdit->setStyleSheet(inputStyle());
    s1->addWidget(nameEdit);

    loginEdit = new QLineEdit(step1Widget);
    loginEdit->setPlaceholderText("Login");
    loginEdit->setStyleSheet(inputStyle());
    s1->addWidget(loginEdit);

    passwordEdit = new QLineEdit(step1Widget);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(inputStyle());
    s1->addWidget(passwordEdit);

    emailEdit = new QLineEdit(step1Widget);
    emailEdit->setPlaceholderText("Email");
    emailEdit->setStyleSheet(inputStyle());
    s1->addWidget(emailEdit);

    errorLabel = new QLabel(step1Widget);
    errorLabel->setStyleSheet(QString("border:none; color:%1; font-size:11pt; font-weight: bold;").arg(UI_ERROR));
    errorLabel->hide();
    s1->addWidget(errorLabel);

    btnRegister = new QPushButton("Register", step1Widget);
    btnRegister->setStyleSheet(btnPrimary());
    s1->addWidget(btnRegister);

    cardLayout->addWidget(step1Widget);

    // ── STEP 2 ──────────────────────────────
    step2Widget = new QWidget(card);
    auto *s2 = new QVBoxLayout(step2Widget);
    s2->setSpacing(8);

    auto *title2 = new QLabel("Verify email", step2Widget);
    title2->setAlignment(Qt::AlignCenter);
    title2->setStyleSheet("border:none; font-size:14pt; font-weight: bold;");
    s2->addWidget(title2);

    codeStatusLabel = new QLabel("Code sent to your email", step2Widget);
    codeStatusLabel->setAlignment(Qt::AlignCenter);
    codeStatusLabel->setStyleSheet("border:none; color:#9ecae6; font-weight: bold;");
    codeStatusLabel->setWordWrap(true);
    s2->addWidget(codeStatusLabel);

    codeEdit = new QLineEdit(step2Widget);
    codeEdit->setPlaceholderText("Code");
    codeEdit->setMaxLength(6);
    codeEdit->setAlignment(Qt::AlignCenter);
    codeEdit->setStyleSheet(inputStyle() + "QLineEdit { font-size:15pt; letter-spacing:4px; }");
    s2->addWidget(codeEdit);

    codeErrorLabel = new QLabel(step2Widget);
    codeErrorLabel->setStyleSheet(QString("border:none; color:%1; font-size:11pt; font-weight: bold;").arg(UI_ERROR));
    codeErrorLabel->hide();
    s2->addWidget(codeErrorLabel);

    btnVerify = new QPushButton("Verify", step2Widget);
    btnVerify->setStyleSheet(btnPrimary());
    s2->addWidget(btnVerify);

    cardLayout->addWidget(step2Widget);

    // ── Back ────────────────────────────────
    btnBack = new QPushButton("Back", card);
    btnBack->setStyleSheet(btnSecondary());
    cardLayout->addWidget(btnBack);

    // Центрирование
    auto *cardRow = new QHBoxLayout();
    cardRow->addStretch();
    cardRow->addWidget(card);
    cardRow->addStretch();
    outer->addLayout(cardRow);
    outer->addStretch();

    showStep(1);

    connect(btnRegister, &QPushButton::clicked, this, &RegWidget::onRegisterClicked);
    connect(btnVerify, &QPushButton::clicked, this, &RegWidget::onVerifyClicked);
    connect(btnBack, &QPushButton::clicked, this, &RegWidget::onBackClicked);
}

void RegWidget::showStep(int step)
{
    m_currentStep = step;
    step1Widget->setVisible(step == 1);
    step2Widget->setVisible(step == 2);
}

void RegWidget::onRegisterClicked()
{
    QString name  = nameEdit->text().trimmed();
    QString login = loginEdit->text().trimmed();
    QString pass  = passwordEdit->text();
    QString email = emailEdit->text().trimmed();

    if (name.isEmpty() || login.isEmpty() || pass.isEmpty() || email.isEmpty()) {
        errorLabel->setText("Fill all fields");
        errorLabel->show();
        return;
    }
    if (!email.contains('@') || !email.contains('.')) {
        errorLabel->setText("Invalid email");
        errorLabel->show();
        return;
    }

    // Проверим, нет ли уже такого логина/почты
    try {
        pqxx::work txn(m_db.getConnection());
        pqxx::result r = txn.exec_params(
            "SELECT id FROM users WHERE login=$1 OR email=$2",
            login.toStdString(), email.toStdString()
        );
        if (!r.empty()) {
            errorLabel->setText("Login or email already exists");
            errorLabel->show();
            return;
        }
    } catch (const std::exception &e) {
        errorLabel->setText("Database error");
        errorLabel->show();
        return;
    }

    // Отправляем код на почту
    if (!EmailService::sendVerificationEmail(email.toStdString(), name.toStdString())) {
        errorLabel->setText("Failed to send email");
        errorLabel->show();
        return;
    }

    // Сохраняем данные для финальной регистрации
    m_pendingName     = name.toStdString();
    m_pendingLogin    = login.toStdString();
    m_pendingPassword = pass.toStdString();
    m_pendingEmail    = email.toStdString();

    errorLabel->hide();
    codeStatusLabel->setText("Code sent to " + email);
    showStep(2);
}

void RegWidget::onVerifyClicked()
{
    QString code = codeEdit->text().trimmed();
    if (code.isEmpty()) {
        codeErrorLabel->setText("Enter code");
        codeErrorLabel->show();
        return;
    }

    // Проверяем код
    if (!EmailService::verifyCode(m_pendingEmail, code.toStdString())) {
        codeErrorLabel->setText("Wrong code");
        codeErrorLabel->show();
        return;
    }

    // Создаём пользователя в БД
    try {
        std::string hash = m_auth.hashPassword(m_pendingPassword);

        pqxx::work txn(m_db.getConnection());
        txn.exec_params(
            "INSERT INTO users (name, login, hash, email) VALUES ($1, $2, $3, $4)",
            m_pendingName, m_pendingLogin, hash, m_pendingEmail
        );
        txn.commit();
    } catch (const std::exception &e) {
        codeErrorLabel->setText("Registration failed");
        codeErrorLabel->show();
        return;
    }

    QMessageBox::information(this, "Success", "Registration successful! You can now log in.");
    emit registrationSuccess(QString::fromStdString(m_pendingLogin));
}

void RegWidget::onBackClicked()
{
    showStep(1);
    emit showAuth();
}

void RegWidget::clearFields()
{
    nameEdit->clear();
    loginEdit->clear();
    passwordEdit->clear();
    emailEdit->clear();
    errorLabel->hide();
    codeEdit->clear();
    codeErrorLabel->hide();
    showStep(1);
}