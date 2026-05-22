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

/**
 * @brief Виджет для восстановления забытого пароля.
 * 
 * Процесс состоит из трёх шагов:
 * 1. Ввод email → отправка кода
 * 2. Ввод кода верификации
 * 3. Ввод и подтверждение нового пароля
 */
class ResetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResetWidget(Database &db, class Auth *auth, QWidget *parent = nullptr);
    ~ResetWidget();

signals:
    void backToAuth();      ///< Вернуться на форму входа
    void resetSuccess();    ///< Пароль успешно изменён

private slots:
    void onEmailTextChanged(const QString &text);
    void onContinueClicked();           ///< Шаг 1: продолжить после ввода email
    void onCodeTextChanged(const QString &text);
    void onVerifyCodeClicked();         ///< Шаг 2: проверить код
    void onNewPasswordTextChanged(const QString &text);
    void onConfirmPasswordTextChanged(const QString &text);
    void onSavePasswordClicked();       ///< Шаг 3: сохранить новый пароль
    void onBackClicked();               ///< Вернуться на предыдущий шаг или на вход

private:
    Database &m_db;             ///< База данных
    class Auth *m_auth;         ///< Объект авторизации

    void setupUI();             ///< Настройка интерфейса
    void showStep(int step);    ///< Переключение между шагами 1, 2, 3
    void validatePasswords();   ///< Проверка совпадения и длины пароля

    // Шаг 1
    QWidget *step1Widget;
    QLineEdit *emailEdit;
    QLabel *emailErrorLabel;
    QPushButton *continueBtn;

    // Шаг 2
    QWidget *step2Widget;
    QLabel *codeStatusLabel;
    QLineEdit *codeEdit;
    QLabel *codeErrorLabel;
    QPushButton *verifyCodeBtn;

    // Шаг 3
    QWidget *step3Widget;
    QLineEdit *newPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *saveBtn;

    QPushButton *backBtn;       ///< Кнопка "Назад" (общая для всех шагов)

    std::string m_pendingEmail; ///< Email, введённый на шаге 1
    std::string m_pendingLogin; ///< Логин, найденный по email
    int m_currentStep = 1;      ///< Текущий шаг (1, 2 или 3)
};

#endif // RESETWIDGET_H