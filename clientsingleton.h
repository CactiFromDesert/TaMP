#ifndef CLIENTSINGLETON_H
#define CLIENTSINGLETON_H

#include <QObject>
#include <QString>

class ClientSingleton : public QObject
{
    Q_OBJECT

public:
    static ClientSingleton& instance();

    // ── STUB API (без логики) ──
    bool connectToServer(const QString &host = QString(), int port = 0);
    void disconnectFromServer();
    bool isConnected() const;

    QString sendRequest(const QString &request);
    void sendRequestAsync(const QString &request);

signals:
    void responseReceived(const QString &response);

private:
    explicit ClientSingleton(QObject *parent = nullptr);
    ~ClientSingleton();

    ClientSingleton(const ClientSingleton&) = delete;
    ClientSingleton& operator=(const ClientSingleton&) = delete;

    bool m_connected = false;
};

#endif // CLIENTSINGLETON_H