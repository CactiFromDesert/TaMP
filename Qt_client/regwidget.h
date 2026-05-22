#ifndef REGWIDGET_H
#define REGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include "database.h"
#include "auth.h"

/**
 * @brief Виджет для регистрации нового пользователя.
 * 
 * Процесс регистрации состоит из двух шагов:
 * 1. Ввод данных (имя, логин, пароль, email)
 * 2. Подтверждение email через код верификации
 */
class RegWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegWidget(Database &db, QWidget *parent = nullptr);
    ~RegWidget() override = default;

public slots:
    void clearFields();  ///< Очистка всех полей ввода

signals:
    void registrationSuccess(const QString &login);  ///< Регистрация успешно завершена
    void showAuth();                                 ///< Переключиться на форму входа

private slots:
    void onRegisterClicked();   ///< Обработчик нажатия "Register" (шаг 1)
    void onVerifyClicked();     ///< Обработчик нажатия "Verify" (шаг 2)
    void onBackClicked();       ///< Возврат на шаг 1 или на форму входа

private:
    void setupUI();             ///< Настройка интерфейса
    void showStep(int step);    ///< Переключение между шагами (1 или 2)

    Database &m_db;             ///< База данных
    Auth m_auth;                ///< Объект авторизации

    std::string m_pendingName;      ///< Имя, сохранённое после шага 1
    std::string m_pendingLogin;     ///< Логин, сохранённый после шага 1
    std::string m_pendingPassword;  ///< Пароль, сохранённый после шага 1
    std::string m_pendingEmail;     ///< Email, сохранённый после шага 1

    /// Виджеты шага 1
    QWidget *step1Widget;
    QLineEdit *nameEdit;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *emailEdit;
    QLabel *errorLabel;

    /// Виджеты шага 2
    QWidget *step2Widget;
    QLineEdit *codeEdit;
    QLabel *codeStatusLabel;
    QLabel *codeErrorLabel;

    QPushButton *btnRegister;
    QPushButton *btnVerify;
    QPushButton *btnBack;

    int m_currentStep = 1;  ///< Текущий шаг (1 или 2)
};

#endif