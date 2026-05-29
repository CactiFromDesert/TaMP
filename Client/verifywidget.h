#ifndef VERIFYWIDGET_H
#define VERIFYWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>

class VerifyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VerifyWidget(QWidget *parent = nullptr);
    ~VerifyWidget();

    void setLogin(const QString &login);

signals:
    void verificationSuccess(QString login);
    void backToAuth();

private slots:
    void onVerifyClicked();
    void onBackClicked();
    void onAuthResponse(const QString &response);

private:
    QLabel *infoLabel;
    QLineEdit *codeEdit;
    QPushButton *verifyBtn;
    QPushButton *backBtn;
    QLabel *statusLabel;

    QString login;

    void setupUI();
};

#endif
