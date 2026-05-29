#ifndef CLIENTSINGLETON_H
#define CLIENTSINGLETON_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

/**
 * @brief Синглтон для управления TCP-соединением с сервером вычислений.
 * 
 * Обеспечивает единое соединение для всего приложения.
 * Автоматически обрабатывает входящие сообщения и разбивает их по строкам.
 */
class ClientSingleton : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Возвращает единственный экземпляр синглтона.
     * @return ClientSingleton& Ссылка на экземпляр
     */
    static ClientSingleton& instance();

    /**
     * @brief Устанавливает соединение с сервером.
     * @param host Адрес сервера (например, "calc_server" или "localhost")
     * @param port Порт сервера (11999)
     * @return true Соединение установлено
     * @return false Ошибка подключения
     */
    bool connectToServer(const QString &host, int port);
    
    void disconnectFromServer();
    bool isConnected() const;

    /**
     * @brief Отправляет сырой запрос на сервер.
     * @param request Строка запроса (автоматически добавляется '\n')
     */
    void sendRequest(const QString &request);

signals:
    /**
     * @brief Сигнал о получении ответа от сервера.
     * @param response Полученная строка (без '\n')
     */
    void responseReceived(const QString &response);
    
    void connected();
    void disconnected();

private slots:
    void onReadyRead();     ///< Обработчик получения данных от сервера
    void onConnected();     ///< Обработчик успешного подключения
    void onDisconnected();  ///< Обработчик отключения

private:
    explicit ClientSingleton(QObject *parent = nullptr);
    ~ClientSingleton();

    ClientSingleton(const ClientSingleton&) = delete;
    ClientSingleton& operator=(const ClientSingleton&) = delete;

    QTcpSocket *m_socket = nullptr;   ///< TCP-сокет для связи с сервером
    QByteArray m_buffer;              ///< Буфер для накопления данных
};

#endif // CLIENTSINGLETON_H