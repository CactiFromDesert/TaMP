#ifndef VERIFYWIDGET_H
#define VERIFYWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QString>

class Auth;

/**
 * @brief Виджет для двухфакторной верификации при входе.
 * 
 * Отображается после успешной проверки пароля.
 * Пользователь должен ввести код, отправленный на email.
 */
class VerifyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VerifyWidget(class Auth *auth, QWidget *parent = nullptr);
    ~VerifyWidget();

    /**
     * @brief Устанавливает логин пользователя для верификации.
     * @param login Логин, переданный из AuthWidget
     */
    void setLogin(const QString &login);

signals:
    void verificationSuccess(QString login);  ///< Код верен, вход выполнен
    void backToAuth();                        ///< Вернуться на форму входа

private slots:
    void onVerifyClicked();  ///< Проверка введённого кода
    void onBackClicked();    ///< Возврат на форму входа

private:
    class Auth *m_auth;       ///< Объект авторизации

    QLabel *infoLabel;        ///< Информация об отправленном коде
    QLineEdit *codeEdit;      ///< Поле для ввода 6-значного кода
    QPushButton *verifyBtn;   ///< Кнопка проверки
    QPushButton *backBtn;     ///< Кнопка возврата
    QLabel *statusLabel;      ///< Метка для сообщений об ошибках

    QString login;            ///< Логин верифицируемого пользователя

    void setupUI();           ///< Настройка интерфейса
};

#endif