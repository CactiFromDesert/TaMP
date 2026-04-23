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
    void showRegister();
    void showVerifyAuth(const QString &login);
    void showReset();

private slots:
    void onLoginClicked();
    void onTogglePassword();
    void onLockTimerFired();
    void onRegisterClicked();
    void onForgotClicked();
    void onAuthResponseReceived(const QString &response);

private:
    // UI элементы — сразу nullptr чтобы не было мусора
    QLineEdit   *loginEdit         = nullptr;
    QLineEdit   *passwordEdit      = nullptr;
    QPushButton *loginBtn          = nullptr;
    QPushButton *registerBtn       = nullptr;
    QPushButton *forgotBtn         = nullptr;
    QPushButton *togglePasswordBtn = nullptr;
    QLabel      *statusLabel       = nullptr;
    QLabel      *attemptsLabel     = nullptr;

    // состояния тоже инициализируем
    int    failedAttempts = 0;
    int    lockLevel      = 0;
    QTimer *lockTimer     = nullptr;
    bool   isLocked       = false;
    bool   m_waitingForAuth = false;

    void applyLock(int minutes, const QString &message);
    void setupUI();
};

#endif // AUTHWIDGET_H