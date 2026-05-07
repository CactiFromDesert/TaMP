#include "resetwidget.h"
#include "auth.h"
#include "database.h"
#include "email_service.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

// ── Palette ─────────────────────────────────────────────────
#define GH_BG          "#0b1e2d"
#define GH_CARD        "#102a43"
#define GH_BORDER      "#1f4e79"
#define GH_TEXT        "#e6f2ff"
#define GH_MUTED       "#9ecae6"
#define GH_GREEN       "#4cd4b0"
#define GH_RED         "#ff6b6b"
#define GH_BLUE        "#3da9fc"
#define GH_BLUE_H      "#74c0fc"

// ── Styles ───────────────────────────────────────────────────
static QString inputStyle()
{
    return QString(
        "QLineEdit{"
        "background:%1; color:%2; border:1px solid %3;"
        "border-radius:6px; padding:6px;"
        "font-family:'Segoe UI'; font-weight: bold; font-size: 12pt;"
        "}"
        "QLineEdit:focus{border-color:%4;}"
    ).arg(GH_BG, GH_TEXT, GH_BORDER, GH_BLUE);
}

static QString btnPrimary()
{
    return QString(
        "QPushButton{"
        "background:%1; color:white; border:none; border-radius:6px;"
        "padding:7px; font-weight: bold; font-size: 12pt;"
        "}"
        "QPushButton:hover{background:%2;}"
        "QPushButton:disabled{background:rgba(61,169,252,0.35); color:rgba(255,255,255,0.4);}"
    ).arg(GH_BLUE, GH_BLUE_H);
}

static QString btnSecondary()
{
    return QString(
        "QPushButton{"
        "background:transparent; color:%1; border:1px solid %2;"
        "border-radius:6px; padding:6px;"
        "font-weight: bold; font-size: 12pt;"
        "}"
        "QPushButton:hover{background:%2;}"
    ).arg(GH_TEXT, GH_BORDER);
}

// ── Constructor ──────────────────────────────────────────────
ResetWidget::ResetWidget(Database &db, Auth *auth, QWidget *parent)
    : QWidget(parent), m_db(db), m_auth(auth)
{
    setStyleSheet(QString(
        "background:%1; color:%2; font-family:'Segoe UI'; font-weight: bold; font-size: 12pt;"
    ).arg(GH_BG, GH_TEXT));

    setupUI();
}

ResetWidget::~ResetWidget() {}

// ── UI SETUP ─────────────────────────────────────────────────
void ResetWidget::setupUI()
{
    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->addStretch();

    QWidget *card = new QWidget(this);
    card->setMinimumWidth(340);
    card->setMaximumWidth(500);
    card->setStyleSheet(QString(
        "background:%1; border:1px solid %2; border-radius:12px; font-weight: bold;"
    ).arg(GH_CARD, GH_BORDER));

    QVBoxLayout *l = new QVBoxLayout(card);
    l->setSpacing(10);
    l->setContentsMargins(20, 20, 20, 20);

    QLabel *title = new QLabel("Password recovery", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("border:none; font-size:16pt; font-weight: bold;");
    l->addWidget(title);

    // ── STEP 1: EMAIL ────────────────────────
    step1Widget = new QWidget(card);
    QVBoxLayout *s1 = new QVBoxLayout(step1Widget);
    s1->setSpacing(10);

    emailEdit = new QLineEdit(step1Widget);
    emailEdit->setPlaceholderText("Email");
    emailEdit->setStyleSheet(inputStyle());
    connect(emailEdit, &QLineEdit::textChanged, this, &ResetWidget::onEmailTextChanged);
    s1->addWidget(emailEdit);

    emailErrorLabel = new QLabel(step1Widget);
    emailErrorLabel->setStyleSheet(QString("border:none; color:%1; font-weight: bold; font-size: 12pt;").arg(GH_RED));
    emailErrorLabel->hide();
    s1->addWidget(emailErrorLabel);

    continueBtn = new QPushButton("Continue", step1Widget);
    continueBtn->setEnabled(false);
    continueBtn->setStyleSheet(btnPrimary());
    connect(continueBtn, &QPushButton::clicked, this, &ResetWidget::onContinueClicked);
    s1->addWidget(continueBtn);

    l->addWidget(step1Widget);

    // ── STEP 2: CODE ────────────────────────
    step2Widget = new QWidget(card);
    QVBoxLayout *s2 = new QVBoxLayout(step2Widget);
    s2->setSpacing(10);

    codeStatusLabel = new QLabel(step2Widget);
    codeStatusLabel->setStyleSheet(QString("border:none; color:%1; font-weight: bold; font-size: 12pt;").arg(GH_GREEN));
    codeStatusLabel->hide();
    codeStatusLabel->setWordWrap(true);
    s2->addWidget(codeStatusLabel);

    codeEdit = new QLineEdit(step2Widget);
    codeEdit->setPlaceholderText("Code");
    codeEdit->setMaxLength(6);
    codeEdit->setAlignment(Qt::AlignCenter);
    codeEdit->setStyleSheet(inputStyle() + "QLineEdit { font-size:15pt; letter-spacing:4px; }");
    connect(codeEdit, &QLineEdit::textChanged, this, &ResetWidget::onCodeTextChanged);
    s2->addWidget(codeEdit);

    codeErrorLabel = new QLabel(step2Widget);
    codeErrorLabel->setStyleSheet(QString("border:none; color:%1; font-weight: bold; font-size: 12pt;").arg(GH_RED));
    codeErrorLabel->hide();
    s2->addWidget(codeErrorLabel);

    verifyCodeBtn = new QPushButton("Verify", step2Widget);
    verifyCodeBtn->setEnabled(false);
    verifyCodeBtn->setStyleSheet(btnPrimary());
    connect(verifyCodeBtn, &QPushButton::clicked, this, &ResetWidget::onVerifyCodeClicked);
    s2->addWidget(verifyCodeBtn);

    l->addWidget(step2Widget);

    // ── STEP 3: PASSWORD ────────────────────
    step3Widget = new QWidget(card);
    QVBoxLayout *s3 = new QVBoxLayout(step3Widget);
    s3->setSpacing(10);

    newPasswordEdit = new QLineEdit(step3Widget);
    newPasswordEdit->setPlaceholderText("New password");
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setStyleSheet(inputStyle());
    connect(newPasswordEdit, &QLineEdit::textChanged, this, &ResetWidget::onNewPasswordTextChanged);
    s3->addWidget(newPasswordEdit);

    confirmPasswordEdit = new QLineEdit(step3Widget);
    confirmPasswordEdit->setPlaceholderText("Confirm password");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setStyleSheet(inputStyle());
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &ResetWidget::onConfirmPasswordTextChanged);
    s3->addWidget(confirmPasswordEdit);

    saveBtn = new QPushButton("Save", step3Widget);
    saveBtn->setEnabled(false);
    saveBtn->setStyleSheet(btnPrimary());
    connect(saveBtn, &QPushButton::clicked, this, &ResetWidget::onSavePasswordClicked);
    s3->addWidget(saveBtn);

    l->addWidget(step3Widget);

    // ── BACK ────────────────────────────────
    backBtn = new QPushButton("Back", card);
    backBtn->setStyleSheet(btnSecondary());
    connect(backBtn, &QPushButton::clicked, this, &ResetWidget::onBackClicked);
    l->addWidget(backBtn);

    // Центрирование
    QHBoxLayout *row = new QHBoxLayout();
    row->addStretch();
    row->addWidget(card);
    row->addStretch();
    outer->addLayout(row);
    outer->addStretch();

    setLayout(outer);
    showStep(1);
}

// ── STEP SWITCH ──────────────────────────────────────────────
void ResetWidget::showStep(int step)
{
    m_currentStep = step;
    step1Widget->setVisible(step == 1);
    step2Widget->setVisible(step == 2);
    step3Widget->setVisible(step == 3);
}

// ── STEP 1: EMAIL ────────────────────────────────────────────
void ResetWidget::onEmailTextChanged(const QString &text)
{
    continueBtn->setEnabled(text.contains('@') && text.contains('.'));
}

void ResetWidget::onContinueClicked()
{
    QString email = emailEdit->text().trimmed();
    m_pendingEmail = email.toStdString();

    // Проверяем, есть ли такой email в БД
    try {
        pqxx::work txn(m_db.getConnection());
        pqxx::result r = txn.exec_params(
            "SELECT login, name FROM users WHERE email=$1", m_pendingEmail
        );
        if (r.empty()) {
            emailErrorLabel->setText("Email not found");
            emailErrorLabel->show();
            return;
        }
        m_pendingLogin = r[0]["login"].c_str();
        std::string name = r[0]["name"].c_str();

        // Отправляем код
        if (!EmailService::sendVerificationEmail(m_pendingEmail, name)) {
            emailErrorLabel->setText("Failed to send code");
            emailErrorLabel->show();
            return;
        }
    } catch (const std::exception &e) {
        emailErrorLabel->setText("Database error");
        emailErrorLabel->show();
        return;
    }

    emailErrorLabel->hide();
    codeStatusLabel->setText("Code sent to " + email);
    codeStatusLabel->show();
    showStep(2);
}

// ── STEP 2: CODE ────────────────────────────────────────────
void ResetWidget::onCodeTextChanged(const QString &text)
{
    verifyCodeBtn->setEnabled(!text.isEmpty());
}

void ResetWidget::onVerifyCodeClicked()
{
    QString code = codeEdit->text().trimmed();
    if (code.isEmpty()) {
        codeErrorLabel->setText("Enter code");
        codeErrorLabel->show();
        return;
    }

    if (!EmailService::verifyCode(m_pendingEmail, code.toStdString())) {
        codeErrorLabel->setText("Wrong code");
        codeErrorLabel->show();
        return;
    }

    codeErrorLabel->hide();
    showStep(3);
}

// ── STEP 3: PASSWORD ────────────────────────────────────────
void ResetWidget::onNewPasswordTextChanged(const QString &)
{
    validatePasswords();
}

void ResetWidget::onConfirmPasswordTextChanged(const QString &)
{
    validatePasswords();
}

void ResetWidget::validatePasswords()
{
    bool ok = newPasswordEdit->text().length() >= 4 &&
              newPasswordEdit->text() == confirmPasswordEdit->text();
    saveBtn->setEnabled(ok);
}

void ResetWidget::onSavePasswordClicked()
{
    std::string newPass = newPasswordEdit->text().toStdString();
    std::string hash = m_auth->hashPassword(newPass);

    try {
        pqxx::work txn(m_db.getConnection());
        txn.exec_params("UPDATE users SET hash=$1 WHERE email=$2", hash, m_pendingEmail);
        txn.commit();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error", "Failed to update password");
        return;
    }

    QMessageBox::information(this, "Success", "Password changed successfully! You can now log in.");
    emit resetSuccess();
}

// ── BACK ─────────────────────────────────────────────────────
void ResetWidget::onBackClicked()
{
    emailEdit->clear();
    codeEdit->clear();
    newPasswordEdit->clear();
    confirmPasswordEdit->clear();
    showStep(1);
    emit backToAuth();
}

