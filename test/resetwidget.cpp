#include "resetwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QRegularExpression>

// ── UI STUB MODE ─────────────────────────────
#define UI_STUB 1

// ── Fake response helper ─────────────────────
static void fakeResponse(ResetWidget *self, const QString &resp)
{
#if UI_STUB
    QTimer::singleShot(150, self, [self, resp]() {
        self->onResetResponseReceived(resp);
    });
#else
    Q_UNUSED(self)
    Q_UNUSED(resp)
#endif
}

// ── GitHub palette (changed colors only) ─────
#define GH_BG "#0b1e2d"
#define GH_CARD "#102a43"
#define GH_BORDER "#1f4e79"
#define GH_TEXT "#e6f2ff"
#define GH_MUTED "#9ecae6"
#define GH_GREEN "#4cd4b0"
#define GH_BLUE "#3da9fc"

// ── Styles ───────────────────────────────────
static QString inputStyle()
{
    return
        "QLineEdit{"
        "background:#0b1e2d;"
        "color:#e6f2ff;"
        "border:1px solid #1f4e79;"
        "border-radius:6px;"
        "padding:6px;"
        "font-family:'Segoe UI';"
        "font-weight: bold;"
        "font-size: 12pt;"
        "}"
        "QLineEdit:focus{border-color:#3da9fc;}";
}

static QString btnStylePrimary()
{
    return
        "QPushButton{"
        "background:#3da9fc;"
        "color:white;"
        "border:none;"
        "border-radius:6px;"
        "padding:7px;"
        "font-weight: bold;"
        "font-size: 12pt;"
        "}"
        "QPushButton:hover{background:#74c0fc;}";
}

static QString btnStyleSecondary()
{
    return
        "QPushButton{"
        "background:transparent;"
        "color:#e6f2ff;"
        "border:1px solid #1f4e79;"
        "border-radius:6px;"
        "padding:6px;"
        "font-weight: bold;"
        "font-size: 12pt;"
        "}"
        "QPushButton:hover{background:#1f4e79;}";
}

// ── Constructor ──────────────────────────────
ResetWidget::ResetWidget(QWidget *parent)
    : QWidget(parent),
    failedAttempts(0),
    isLocked(false),
    m_currentStep(StepEmail),
    m_waitingForResponse(false)
{
    lockTimer = new QTimer(this);
    lockTimer->setSingleShot(true);

    connect(lockTimer,
            &QTimer::timeout,
            this,
            &ResetWidget::onLockTimerFired);

    setStyleSheet(
        "background:#0b1e2d;"
        "color:#e6f2ff;"
        "font-family:'Segoe UI';"
        "font-weight: bold;"
        "font-size: 12pt;"
        );

    setupUI();
}

ResetWidget::~ResetWidget() {}

// ── UI SETUP ─────────────────────────────────
void ResetWidget::setupUI()
{
    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->addStretch();

    QWidget *card = new QWidget(this);
    card->setFixedWidth(340);
    card->setStyleSheet(
        "background:#102a43;"
        "border:1px solid #1f4e79;"
        "border-radius:12px;"
        "font-weight: bold;"
        );

    QVBoxLayout *l = new QVBoxLayout(card);
    l->setSpacing(10);
    l->setContentsMargins(20,20,20,20);

    QLabel *title = new QLabel("Password recovery", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "border:none;"
        "font-size:16pt;"
        "font-weight: bold;"
        );

    l->addWidget(title);

    // STEP 1 EMAIL

    step1Widget = new QWidget(card);
    QVBoxLayout *s1 = new QVBoxLayout(step1Widget);
    s1->setSpacing(10);

    emailEdit = new QLineEdit(step1Widget);
    emailEdit->setFrame(false);
    emailEdit->setPlaceholderText("Email");
    emailEdit->setStyleSheet(inputStyle());

    connect(emailEdit,
            &QLineEdit::textChanged,
            this,
            &ResetWidget::onEmailTextChanged);

    s1->addWidget(emailEdit);

    emailErrorLabel = new QLabel(step1Widget);
    emailErrorLabel->setStyleSheet("border:none; color:#ff6b6b; font-weight: bold; font-size: 12pt;");
    emailErrorLabel->hide();
    s1->addWidget(emailErrorLabel);

    continueBtn = new QPushButton("Continue", step1Widget);
    continueBtn->setEnabled(false);
    continueBtn->setStyleSheet(btnStylePrimary());

    connect(continueBtn,
            &QPushButton::clicked,
            this,
            &ResetWidget::onContinueClicked);

    s1->addWidget(continueBtn);

    l->addWidget(step1Widget);

    // STEP 2 CODE

    step2Widget = new QWidget(card);
    QVBoxLayout *s2 = new QVBoxLayout(step2Widget);
    s2->setSpacing(10);

    codeStatusLabel = new QLabel(step2Widget);
    codeStatusLabel->setStyleSheet("border:none; color:#4cd4b0; font-weight: bold; font-size: 12pt;");
    codeStatusLabel->hide();
    s2->addWidget(codeStatusLabel);

    codeEdit = new QLineEdit(step2Widget);
    codeEdit->setPlaceholderText("Code");
    codeEdit->setStyleSheet(inputStyle());

    connect(codeEdit,
            &QLineEdit::textChanged,
            this,
            &ResetWidget::onCodeTextChanged);

    s2->addWidget(codeEdit);

    codeErrorLabel = new QLabel(step2Widget);
    codeErrorLabel->setStyleSheet("border:none; color:#ff6b6b; font-weight: bold; font-size: 12pt;");
    codeErrorLabel->hide();
    s2->addWidget(codeErrorLabel);

    verifyCodeBtn = new QPushButton("Verify", step2Widget);
    verifyCodeBtn->setEnabled(false);
    verifyCodeBtn->setStyleSheet(btnStylePrimary());

    connect(verifyCodeBtn,
            &QPushButton::clicked,
            this,
            &ResetWidget::onVerifyCodeClicked);

    s2->addWidget(verifyCodeBtn);

    l->addWidget(step2Widget);

    // STEP 3 PASSWORD

    step3Widget = new QWidget(card);
    QVBoxLayout *s3 = new QVBoxLayout(step3Widget);
    s3->setSpacing(10);

    newPasswordEdit = new QLineEdit(step3Widget);
    newPasswordEdit->setPlaceholderText("New password");
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    newPasswordEdit->setStyleSheet(inputStyle());

    connect(newPasswordEdit,
            &QLineEdit::textChanged,
            this,
            &ResetWidget::onNewPasswordTextChanged);

    s3->addWidget(newPasswordEdit);

    confirmPasswordEdit = new QLineEdit(step3Widget);
    confirmPasswordEdit->setPlaceholderText("Confirm password");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setStyleSheet(inputStyle());

    connect(confirmPasswordEdit,
            &QLineEdit::textChanged,
            this,
            &ResetWidget::onConfirmPasswordTextChanged);

    s3->addWidget(confirmPasswordEdit);

    saveBtn = new QPushButton("Save", step3Widget);
    saveBtn->setEnabled(false);
    saveBtn->setStyleSheet(btnStylePrimary());

    connect(saveBtn,
            &QPushButton::clicked,
            this,
            &ResetWidget::onSavePasswordClicked);

    s3->addWidget(saveBtn);

    l->addWidget(step3Widget);

    backBtn = new QPushButton("Back", card);
    backBtn->setStyleSheet(btnStyleSecondary());

    connect(backBtn,
            &QPushButton::clicked,
            this,
            &ResetWidget::onBackClicked);

    l->addWidget(backBtn);

    // центрирование карточки

    QHBoxLayout *row = new QHBoxLayout();
    row->addStretch();
    row->addWidget(card);
    row->addStretch();

    outer->addLayout(row);
    outer->addStretch();

    setLayout(outer);

    showStep(StepEmail);
}

// ── STEP SWITCH ─────────────────────────────
void ResetWidget::showStep(Step step)
{
    m_currentStep = step;

    step1Widget->setVisible(step == StepEmail);
    step2Widget->setVisible(step == StepCode);
    step3Widget->setVisible(step == StepPassword);
}

// ── EMAIL STEP ─────────────────────────────
void ResetWidget::onEmailTextChanged(const QString &text)
{
    continueBtn->setEnabled(text.contains("@"));
}

void ResetWidget::onContinueClicked()
{
    fakeResponse(this, "reset_code_sent");
}

// ── CODE STEP ─────────────────────────────
void ResetWidget::onCodeTextChanged(const QString &text)
{
    verifyCodeBtn->setEnabled(!text.isEmpty());
}

void ResetWidget::onVerifyCodeClicked()
{
    fakeResponse(this, "reset_code_ok");
}

// ── PASSWORD STEP ─────────────────────────
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
    bool ok =
        newPasswordEdit->text().length() >= 8 &&
        newPasswordEdit->text() ==
            confirmPasswordEdit->text();

    saveBtn->setEnabled(ok);
}

void ResetWidget::onSavePasswordClicked()
{
    fakeResponse(this, "password_changed");
}

// ── RESPONSE ─────────────────────────────
void ResetWidget::onResetResponseReceived(const QString &response)
{
    if (response == "reset_code_sent")
        showStep(StepCode);

    if (response == "reset_code_ok")
        showStep(StepPassword);

    if (response == "password_changed")
        emit resetSuccess();

    m_waitingForResponse = false;
}

// ── BACK ─────────────────────────────────
void ResetWidget::onBackClicked()
{
    emailEdit->clear();
    codeEdit->clear();
    newPasswordEdit->clear();
    confirmPasswordEdit->clear();

    showStep(StepEmail);

    emit backToAuth();
}

// ── UNUSED STUBS ─────────────────────────
void ResetWidget::onLockTimerFired() {}

void ResetWidget::onTogglePassword1() {}

void ResetWidget::onTogglePassword2() {}