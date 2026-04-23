#ifndef RESETWIDGET_H
#define RESETWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QString>

class ResetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResetWidget(QWidget *parent = nullptr);
    ~ResetWidget();

signals:
    void backToAuth();
    void resetSuccess();

public slots:
    // ───────── UI callbacks (ДОЛЖНЫ БЫТЬ PUBLIC) ─────────
    void onEmailTextChanged(const QString &text);
    void onContinueClicked();

    void onCodeTextChanged(const QString &text);
    void onVerifyCodeClicked();

    void onNewPasswordTextChanged(const QString &text);
    void onConfirmPasswordTextChanged(const QString &text);

    void onTogglePassword1();
    void onTogglePassword2();

    void onSavePasswordClicked();
    void onBackClicked();

    void onLockTimerFired();

    // 🔴 MOCK response handler (важно: public slot)
    void onResetResponseReceived(const QString &response);

private:
    // ── Step 1: email ─────────────────────────────
    QWidget     *step1Widget = nullptr;
    QLineEdit   *emailEdit = nullptr;
    QLabel      *emailErrorLabel = nullptr;
    QPushButton *continueBtn = nullptr;

    // ── Step 2: code ──────────────────────────────
    QWidget     *step2Widget = nullptr;
    QLabel      *codeStatusLabel = nullptr;
    QLineEdit   *codeEdit = nullptr;
    QLabel      *codeErrorLabel = nullptr;
    QPushButton *verifyCodeBtn = nullptr;

    // ── Step 3: new password ──────────────────────
    QWidget     *step3Widget = nullptr;
    QLineEdit   *newPasswordEdit = nullptr;
    QLabel      *newPasswordErrorLabel = nullptr;
    QPushButton *togglePassBtn1 = nullptr;

    QLineEdit   *confirmPasswordEdit = nullptr;
    QLabel      *confirmErrorLabel = nullptr;
    QPushButton *togglePassBtn2 = nullptr;

    QPushButton *saveBtn = nullptr;
    QPushButton *backBtn = nullptr;

    // ── State ─────────────────────────────────────
    QString m_email;
    QString m_code;

    int  failedAttempts = 0;
    bool isLocked = false;
    bool m_waitingForResponse = false;

    QTimer *lockTimer = nullptr;

    enum Step
    {
        StepEmail,
        StepCode,
        StepPassword
    };

    Step m_currentStep = StepEmail;

    // ── Helpers ───────────────────────────────────
    void setupUI();
    void showStep(Step step);
    bool isEmailValid(const QString &email) const;
    void validatePasswords();
    void applyLock(int minutes, const QString &message);
};

#endif // RESETWIDGET_H