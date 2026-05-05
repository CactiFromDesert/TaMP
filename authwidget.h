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

private slots:
    void onLoginClicked();
    void onTogglePassword();

private:
    QLineEdit   *loginEdit = nullptr;
    QLineEdit   *passwordEdit = nullptr;
    QPushButton *loginBtn = nullptr;
    QLabel      *statusLabel = nullptr;

    void setupUI();
};

#endif // AUTHWIDGET_H