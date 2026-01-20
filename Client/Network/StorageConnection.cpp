#include "StorageConnection.h"
#include <QDebug>

StorageConnection::StorageConnection(QObject* parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_port(0)
{
}

StorageConnection::~StorageConnection()
{
    disconnect();
}

bool StorageConnection::connectToServer(const QString& ip, uint16_t port)
{
    m_ip = ip;
    m_port = port;

    if (m_socket) {
        delete m_socket;
    }

    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, &StorageConnection::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &StorageConnection::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &StorageConnection::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &StorageConnection::onError);

    m_socket->connectToHost(ip, port);

    return m_socket->waitForConnected(5000);
}

void StorageConnection::disconnect()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_buffer.clear();
    }
}

bool StorageConnection::sendRequest(const TransHeader& header, const QByteArray& data)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "Not connected to Storage server";
        return false;
    }

    QByteArray packet;
    TransHeader networkHeader = header;
    networkHeader.toNetworkOrder();
    packet.append(reinterpret_cast<const char*>(&networkHeader), sizeof(TransHeader));
    packet.append(data);

    qint64 bytesWritten = m_socket->write(packet);
    return bytesWritten == packet.size();
}

bool StorageConnection::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void StorageConnection::onConnected()
{
    qDebug() << "Connected to Storage server:" << m_ip << ":" << m_port;
    emit connected();
}

void StorageConnection::onDisconnected()
{
    qDebug() << "Disconnected from Storage server";
    emit disconnected();
}

void StorageConnection::onReadyRead()
{
    processReceivedData();
}

void StorageConnection::onError(QAbstractSocket::SocketError error)
{
    QString errorMsg = m_socket ? m_socket->errorString() : "Unknown error";
    qWarning() << "Storage connection error:" << errorMsg;
    emit errorOccurred(errorMsg);
}

void StorageConnection::processReceivedData()
{
    if (!m_socket) {
        return;
    }

    QByteArray newData = m_socket->readAll();
    m_buffer.append(newData);

    while (m_buffer.size() >= static_cast<int>(sizeof(TransHeader))) {
        TransHeader* header = reinterpret_cast<TransHeader*>(m_buffer.data());
        header->toHostOrder();

        if (!header->validateMagic()) {
            qWarning() << "Invalid magic number";
            m_buffer.clear();
            return;
        }

        uint32_t packetSize = sizeof(TransHeader) + header->len;

        if (m_buffer.size() < static_cast<int>(packetSize)) {
            break;
        }

        QByteArray body = m_buffer.mid(sizeof(TransHeader), header->len);

        emit messageReceived(*header, body);

        m_buffer.remove(0, static_cast<int>(packetSize));
    }
}
