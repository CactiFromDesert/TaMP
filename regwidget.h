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

signals:
    void registrationSuccess(const QString &login);
    void showAuth();

private slots:
    void onShowAuthClicked();

private:
    void setupUI();

    // UI
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *emailEdit;

    QLabel *errorLabel;
    QLabel *okLabel;

    QPushButton *btnRegister;
    QPushButton *btnCode;
    QPushButton *btnVerify;
};

#endif