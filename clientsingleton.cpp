#include "clientsingleton.h"

ClientSingleton::ClientSingleton(QObject *parent)
    : QObject(parent)
{
    m_connected = false;
}

ClientSingleton::~ClientSingleton() {}

ClientSingleton& ClientSingleton::instance()
{
    static ClientSingleton inst;
    return inst;
}



bool ClientSingleton::connectToServer(const QString &, int)
{
    // UI stub — всегда "успех", но ничего не делает
    m_connected = true;
    return true;
}

void ClientSingleton::disconnectFromServer()
{
    m_connected = false;
}

bool ClientSingleton::isConnected() const
{
    return m_connected;
}

// ─────────────────────────────
// STUB: синхронный запрос
// ─────────────────────────────
QString ClientSingleton::sendRequest(const QString &)
{
    // ничего не обрабатываем
    return QString();
}

// ─────────────────────────────
// STUB: асинхронный запрос
// ─────────────────────────────
void ClientSingleton::sendRequestAsync(const QString &)
{
    // ничего не отправляем
    // просто оставляем точку расширения

    emit responseReceived(QString());
}