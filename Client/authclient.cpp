#include "authclient.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QDebug>

AuthClient::AuthClient(QObject *parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &AuthClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &AuthClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &AuthClient::onDisconnected);
}

AuthClient::~AuthClient()
{
    disconnectFromServer();
}

AuthClient& AuthClient::instance()
{
    static AuthClient inst;
    return inst;
}

bool AuthClient::connectToServer(const QString &host, int port)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        return true;

    qInfo() << "AuthClient: connecting to" << host << ":" << port;
    m_socket->connectToHost(host, port);
    bool ok = m_socket->waitForConnected(3000);
    if (ok) {
        qInfo() << "AuthClient: connected successfully";
    } else {
        qWarning() << "AuthClient: connection failed:" << m_socket->errorString();
    }
    return ok;
}

void AuthClient::disconnectFromServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

bool AuthClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void AuthClient::sendRequest(const QString &type, const QJsonObject &data)
{
    QJsonObject obj;
    obj["type"] = type;
    obj["data"] = data;

    QJsonDocument doc(obj);
    QByteArray packet = doc.toJson(QJsonDocument::Compact) + "\n";

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        qInfo() << "AuthClient: sending" << type;
        m_socket->write(packet);
    } else {
        qWarning() << "AuthClient: NOT CONNECTED, cannot send" << type
                   << "(state:" << m_socket->state() << ")";
    }
}

void AuthClient::onReadyRead()
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

void AuthClient::onConnected()
{
    emit connected();
}

void AuthClient::onDisconnected()
{
    emit disconnected();
}
