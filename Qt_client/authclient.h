#ifndef AUTHCLIENT_H
#define AUTHCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QString>

class AuthClient : public QObject
{
    Q_OBJECT

public:
    static AuthClient& instance();

    bool connectToServer(const QString &host, int port);
    void disconnectFromServer();
    bool isConnected() const;

    void sendRequest(const QString &type, const QJsonObject &data = QJsonObject());

signals:
    void responseReceived(const QString &response);
    void connected();
    void disconnected();

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    explicit AuthClient(QObject *parent = nullptr);
    ~AuthClient();

    AuthClient(const AuthClient&) = delete;
    AuthClient& operator=(const AuthClient&) = delete;

    QTcpSocket *m_socket = nullptr;
    QByteArray m_buffer;
};

#endif // AUTHCLIENT_H
