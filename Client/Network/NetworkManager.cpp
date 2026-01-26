#include "NetworkManager.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent)
    , m_metaSocket(nullptr)
    , m_storageSocket(nullptr)
    , m_sequenceNumber(0)
{
}

NetworkManager::~NetworkManager()
{
    disconnectFromServer();
}

bool NetworkManager::connectToMetaServer(const QString& ip, uint16_t port)
{
    if (m_metaSocket) {
        delete m_metaSocket;
    }

    m_metaSocket = new QTcpSocket(this);

    connect(m_metaSocket, &QTcpSocket::connected, this, [this, ip, port]() {
        emit metaServerConnectionChanged(true);
        qDebug() << "Connected to Meta server:" << ip << ":" << port;
    });

    connect(m_metaSocket, &QTcpSocket::disconnected, this, [this]() {
        emit metaServerConnectionChanged(false);
        qDebug() << "Disconnected from Meta server";
    });

    connect(m_metaSocket, &QTcpSocket::readyRead, this, &NetworkManager::onMetaSocketReadyRead);
    connect(m_metaSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onMetaSocketError);

    m_metaSocket->connectToHost(ip, port);

    return m_metaSocket->waitForConnected(5000);
}

bool NetworkManager::connectToStorageServer(const QString& ip, uint16_t port)
{
    if (m_storageSocket) {
        delete m_storageSocket;
    }

    m_storageSocket = new QTcpSocket(this);

    connect(m_storageSocket, &QTcpSocket::connected, this, [this, ip, port]() {
        emit storageServerConnectionChanged(true);
        qDebug() << "Connected to Storage server:" << ip << ":" << port;
    });

    connect(m_storageSocket, &QTcpSocket::disconnected, this, [this]() {
        emit storageServerConnectionChanged(false);
        qDebug() << "Disconnected from Storage server";
    });

    connect(m_storageSocket, &QTcpSocket::readyRead, this, &NetworkManager::onStorageSocketReadyRead);
    connect(m_storageSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onStorageSocketError);

    m_storageSocket->connectToHost(ip, port);

    return m_storageSocket->waitForConnected(5000);
}

void NetworkManager::disconnectFromServer()
{
    if (m_metaSocket) {
        m_metaSocket->disconnectFromHost();
        m_metaBuffer.clear();
    }

    if (m_storageSocket) {
        m_storageSocket->disconnectFromHost();
        m_storageBuffer.clear();
    }
}

bool NetworkManager::sendToMetaServer(const TransHeader& header, const QByteArray& data)
{
    if (!m_metaSocket || m_metaSocket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Meta server not connected";
        return false;
    }

    QByteArray packet;
    TransHeader networkHeader = header;
    networkHeader.toNetworkOrder();
    packet.append(reinterpret_cast<const char*>(&networkHeader), sizeof(TransHeader));
    packet.append(data);

    qint64 bytesWritten = m_metaSocket->write(packet);
    return bytesWritten == packet.size();
}

bool NetworkManager::sendToStorageServer(const TransHeader& header, const QByteArray& data)
{
    if (!m_storageSocket || m_storageSocket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Storage server not connected";
        return false;
    }

    QByteArray packet;
    TransHeader networkHeader = header;
    networkHeader.toNetworkOrder();
    packet.append(reinterpret_cast<const char*>(&networkHeader), sizeof(TransHeader));
    packet.append(data);

    qint64 bytesWritten = m_storageSocket->write(packet);
    return bytesWritten == packet.size();
}

bool NetworkManager::isMetaServerConnected() const
{
    return m_metaSocket && m_metaSocket->state() == QAbstractSocket::ConnectedState;
}

bool NetworkManager::isStorageServerConnected() const
{
    return m_storageSocket && m_storageSocket->state() == QAbstractSocket::ConnectedState;
}

uint32_t NetworkManager::generateSequenceNumber()
{
    return static_cast<uint32_t>(m_sequenceNumber.fetchAndAddRelaxed(1));
}

void NetworkManager::onMetaSocketReadyRead()
{
    processReceivedData(m_metaSocket, m_metaBuffer, true);
}

void NetworkManager::onStorageSocketReadyRead()
{
    processReceivedData(m_storageSocket, m_storageBuffer, false);
}

void NetworkManager::onMetaSocketError(QAbstractSocket::SocketError error)
{
    qWarning() << "Meta socket error:" << error;
}

void NetworkManager::onStorageSocketError(QAbstractSocket::SocketError error)
{
    qWarning() << "Storage socket error:" << error;
}

void NetworkManager::processReceivedData(QTcpSocket* socket, QByteArray& buffer, bool isMeta)
{
    if (!socket) {
        return;
    }

    QByteArray newData = socket->readAll();
    buffer.append(newData);

    while (buffer.size() >= static_cast<int>(sizeof(TransHeader))) {
        TransHeader* header = reinterpret_cast<TransHeader*>(buffer.data());
        header->toHostOrder();

        if (!header->validateMagic()) {
            qWarning() << "Invalid magic number";
            buffer.clear();
            return;
        }

        uint32_t packetSize = sizeof(TransHeader) + header->len;

        if (buffer.size() < static_cast<int>(packetSize)) {
            break;
        }

        QByteArray body = buffer.mid(sizeof(TransHeader), header->len);

        if (isMeta) {
            emit metaServerMessageReceived(*header, body);
        } else {
            emit storageServerMessageReceived(*header, body);
        }

        buffer.remove(0, static_cast<int>(packetSize));
    }
}
