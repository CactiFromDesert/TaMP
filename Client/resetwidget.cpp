#include "resetwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include "authclient.h"

#define GH_BG          "#0b1e2d"
#define GH_CARD        "#102a43"
#define GH_BORDER      "#1f4e79"
#define GH_TEXT        "#e6f2ff"
#define GH_MUTED       "#9ecae6"
#define GH_GREEN       "#4cd4b0"
#define GH_RED         "#ff6b6b"
#define GH_BLUE        "#3da9fc"
#define GH_BLUE_H      "#74c0fc"

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

ResetWidget::ResetWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(QString(
        "background:%1; color:%2; font-family:'Segoe UI'; font-weight: bold; font-size: 12pt;"
    ).arg(GH_BG, GH_TEXT));

    setupUI();
    connect(&AuthClient::instance(), &AuthClient::responseReceived,
            this, &ResetWidget::onAuthResponse);
}

ResetWidget::~ResetWidget() {}

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

    QHBoxLayout *row = new QHBoxLayout();
    row->addStretch();
    row->addWidget(card);
    row->addStretch();
    outer->addLayout(row);
    outer->addStretch();

    setLayout(outer);
    showStep(1);
}

void ResetWidget::showStep(int step)
{
    m_currentStep = step;
    step1Widget->setVisible(step == 1);
    step2Widget->setVisible(step == 2);
    step3Widget->setVisible(step == 3);
}

void ResetWidget::onEmailTextChanged(const QString &text)
{
    continueBtn->setEnabled(text.contains('@') && text.contains('.'));
}

void ResetWidget::onContinueClicked()
{
    m_pendingEmail = emailEdit->text().trimmed().toStdString();

    QJsonObject obj;
    obj["email"] = emailEdit->text().trimmed();

    AuthClient::instance().sendRequest("reset_request", obj);
}

void ResetWidget::onCodeTextChanged(const QString &text)
{
    verifyCodeBtn->setEnabled(!text.isEmpty());
}

void ResetWidget::onVerifyCodeClicked()
{
    QJsonObject obj;
    obj["email"] = QString::fromStdString(m_pendingEmail);
    obj["code"]  = codeEdit->text();

    AuthClient::instance().sendRequest("reset_verify", obj);
}

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
    QJsonObject obj;
    obj["email"] = QString::fromStdString(m_pendingEmail);
    obj["new_password"] = newPasswordEdit->text();

    AuthClient::instance().sendRequest("reset_finish", obj);
}

void ResetWidget::onAuthResponse(const QString &response)
{
    if (!isVisible()) return;

    QJsonDocument doc = QJsonDocument::fromJson(response.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString status = obj["status"].toString();

    if (status == "ok") {
        if (m_currentStep == 1) {
            codeStatusLabel->setText("Code sent to your email");
            codeStatusLabel->show();
            showStep(2);
        } else if (m_currentStep == 2) {
            codeStatusLabel->setText("Code verified");
            showStep(3);
        } else if (m_currentStep == 3) {
            QMessageBox::information(this, "Success", "Password has been changed successfully!");
            emit resetSuccess();
        }
    } else {
        QString msg = obj["message"].toString();
        if (m_currentStep == 1) {
            emailErrorLabel->setText(msg);
            emailErrorLabel->show();
        } else if (m_currentStep == 2) {
            codeErrorLabel->setText(msg);
            codeErrorLabel->show();
        }
    }
}

void ResetWidget::onBackClicked()
{
    emailEdit->clear();
    codeEdit->clear();
    newPasswordEdit->clear();
    confirmPasswordEdit->clear();
    showStep(1);
    emit backToAuth();
}
