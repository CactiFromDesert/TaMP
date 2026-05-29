#ifndef REGWIDGET_H
#define REGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>

class RegWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegWidget(QWidget *parent = nullptr);
    ~RegWidget() override = default;

public slots:
    void clearFields();

signals:
    void registrationSuccess(const QString &login);
    void showAuth();

private slots:
    void onRegisterClicked();
    void onVerifyClicked();
    void onBackClicked();
    void onAuthResponse(const QString &response);

private:
    void setupUI();
    void showStep(int step);

    std::string m_pendingName;
    std::string m_pendingLogin;
    std::string m_pendingPassword;
    std::string m_pendingEmail;

    QWidget *step1Widget;
    QLineEdit *nameEdit;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *emailEdit;
    QLabel *errorLabel;

    QWidget *step2Widget;
    QLineEdit *codeEdit;
    QLabel *codeStatusLabel;
    QLabel *codeErrorLabel;

    QPushButton *btnRegister;
    QPushButton *btnVerify;
    QPushButton *btnBack;

    int m_currentStep = 1;
};

#endif
