#ifndef CLIENTSINGLETON_H
#define CLIENTSINGLETON_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class ClientSingleton : public QObject
{
    Q_OBJECT

public:
    static ClientSingleton& instance();

    bool connectToServer(const QString &host, int port);
    void disconnectFromServer();
    bool isConnected() const;

    void sendRequest(const QString &request);

signals:
    void responseReceived(const QString &response);
    void connected();
    void disconnected();

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    explicit ClientSingleton(QObject *parent = nullptr);
    ~ClientSingleton();

    ClientSingleton(const ClientSingleton&) = delete;
    ClientSingleton& operator=(const ClientSingleton&) = delete;

    QTcpSocket *m_socket = nullptr;
    QByteArray m_buffer;
};

#endif // CLIENTSINGLETON_H