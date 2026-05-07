#ifndef VERIFYWIDGET_H
#define VERIFYWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QString>

class Auth;  // forward declaration

class VerifyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VerifyWidget(class Auth *auth, QWidget *parent = nullptr);
    ~VerifyWidget();

    void setLogin(const QString &login);

signals:
    void verificationSuccess(QString login);
    void backToAuth();

private slots:
    void onVerifyClicked();
    void onBackClicked();

private:
    class Auth *m_auth;

    QLabel *infoLabel = nullptr;
    QLineEdit *codeEdit = nullptr;
    QPushButton *verifyBtn = nullptr;
    QPushButton *backBtn = nullptr;
    QLabel *statusLabel = nullptr;

    QString login;

    void setupUI();
};

#endif