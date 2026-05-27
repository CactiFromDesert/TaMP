#include "clientsingleton.h"

ClientSingleton::ClientSingleton(QObject *parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientSingleton::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &ClientSingleton::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientSingleton::onDisconnected);
}

ClientSingleton::~ClientSingleton()
{
    disconnectFromServer();
}

ClientSingleton& ClientSingleton::instance()
{
    static ClientSingleton inst;
    return inst;
}

void ClientSingleton::connectToServer()
{
    QString host = qgetenv("CALC_HOST");
    if (host.isEmpty()) host = "calc_server";  // fallback для docker compose
    
    quint16 port = 11999;
    m_socket->connectToHost(host, port);
}

void ClientSingleton::disconnectFromServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

bool ClientSingleton::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ClientSingleton::sendRequest(const QString &request)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = request.toUtf8() + "\n";
        m_socket->write(data);
        m_socket->flush();
    }
}

void ClientSingleton::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);

        if (!line.isEmpty())
            emit responseReceived(QString::fromUtf8(line));
    }
}

void ClientSingleton::onConnected()
{
    emit connected();
}

void ClientSingleton::onDisconnected()
{
    emit disconnected();
}