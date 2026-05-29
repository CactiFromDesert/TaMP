#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QString>

class AuthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWidget(QWidget *parent = nullptr);
    ~AuthWidget() override;

signals:
    void loginSuccess(const QString &login);
    void showVerifyAuth(const QString &login);
    void showRegister();
    void showReset();
    void suggestReset();

public slots:
    void clearFields();
    
private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onForgotClicked();
    void onTogglePassword();
    void onAuthResponse(const QString &response);

private:
    QLineEdit   *loginEdit = nullptr;
    QLineEdit   *passwordEdit = nullptr;
    QPushButton *loginBtn = nullptr;
    QPushButton *registerBtn = nullptr;
    QPushButton *forgotBtn = nullptr;
    QLabel      *statusLabel = nullptr;

    void setupUI();

    int m_loginAttempts = 0;
    QString m_pendingLogin;
};

#endif // AUTHWIDGET_H