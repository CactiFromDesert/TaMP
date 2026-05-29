#include "clientsingleton.h"
#include <QDebug>

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

bool ClientSingleton::connectToServer(const QString &host, int port)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        return true;

    qInfo() << "ClientSingleton: connecting to" << host << ":" << port;
    m_socket->connectToHost(host, port);
    bool ok = m_socket->waitForConnected(3000);
    if (ok) {
        qInfo() << "ClientSingleton: connected successfully";
    } else {
        qWarning() << "ClientSingleton: connection failed:" << m_socket->errorString();
    }
    return ok;
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
    QByteArray packet = request.toUtf8() + "\n";

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(packet);
    } else {
        qWarning() << "ClientSingleton: NOT CONNECTED, cannot send" << request;
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
    qInfo() << "ClientSingleton: connected";
    emit connected();
}

void ClientSingleton::onDisconnected()
{
    qWarning() << "ClientSingleton: disconnected";
    emit disconnected();
}
