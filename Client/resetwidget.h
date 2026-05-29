#ifndef RESETWIDGET_H
#define RESETWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
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

private slots:
    void onEmailTextChanged(const QString &text);
    void onContinueClicked();
    void onCodeTextChanged(const QString &text);
    void onVerifyCodeClicked();
    void onNewPasswordTextChanged(const QString &text);
    void onConfirmPasswordTextChanged(const QString &text);
    void onSavePasswordClicked();
    void onBackClicked();
    void onAuthResponse(const QString &response);

private:
    void setupUI();
    void showStep(int step);
    void validatePasswords();

    QWidget *step1Widget;
    QLineEdit *emailEdit;
    QLabel *emailErrorLabel;
    QPushButton *continueBtn;

    QWidget *step2Widget;
    QLabel *codeStatusLabel;
    QLineEdit *codeEdit;
    QLabel *codeErrorLabel;
    QPushButton *verifyCodeBtn;

    QWidget *step3Widget;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *saveBtn;

    QPushButton *backBtn;

    std::string m_pendingEmail;
    int m_currentStep = 1;
};

#endif
