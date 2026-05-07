#ifndef RESETWIDGET_H
#define RESETWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QString>

class Auth;
class Database;

class ResetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResetWidget(Database &db, class Auth *auth, QWidget *parent = nullptr);
    ~ResetWidget();

signals:
    void backToAuth();
    void resetSuccess();

private slots:
    void onEmailTextChanged(const QString &text);
    void onContinueClicked();
    void onCodeTextChanged(const QString &text);
    void onVerifyCodeClicked();
    void onNewPasswordTextChanged(const QString &text);
    void onConfirmPasswordTextChanged(const QString &text);
    void onSavePasswordClicked();
    void onBackClicked();

private:
    Database &m_db;
    class Auth *m_auth;

    void setupUI();
    void showStep(int step);
    void validatePasswords();

    // Step 1
    QWidget *step1Widget;
    QLineEdit *emailEdit;
    QLabel *emailErrorLabel;
    QPushButton *continueBtn;

    // Step 2
    QWidget *step2Widget;
    QLabel *codeStatusLabel;
    QLineEdit *codeEdit;
    QLabel *codeErrorLabel;
    QPushButton *verifyCodeBtn;

    // Step 3
    QWidget *step3Widget;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *saveBtn;

    QPushButton *backBtn;

    std::string m_pendingEmail;
    std::string m_pendingLogin;
    int m_currentStep = 1;
};

#endif // RESETWIDGET_H