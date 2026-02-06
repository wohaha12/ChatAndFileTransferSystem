#include "MetaConnection.h"
#include <QDebug>

MetaConnection::MetaConnection(QObject *parent)
    : QObject(parent), m_socket(nullptr), m_port(0) {}

MetaConnection::~MetaConnection() { disconnect(); }

bool MetaConnection::connectToServer(const QString &ip, uint16_t port) {
  m_ip = ip;
  m_port = port;

  if (m_socket) {
    delete m_socket;
  }

  m_socket = new QTcpSocket(this);

  connect(m_socket, &QTcpSocket::connected, this, &MetaConnection::onConnected);
  connect(m_socket, &QTcpSocket::disconnected, this,
          &MetaConnection::onDisconnected);
  connect(m_socket, &QTcpSocket::readyRead, this, &MetaConnection::onReadyRead);
  connect(m_socket,
          QOverload<QAbstractSocket::SocketError>::of(
              &QAbstractSocket::errorOccurred),
          this, &MetaConnection::onError);

  m_socket->connectToHost(ip, port);

  return m_socket->waitForConnected(5000);
}

void MetaConnection::disconnect() {
  if (m_socket) {
    m_socket->disconnectFromHost();
    m_buffer.clear();
  }
}

bool MetaConnection::sendRequest(
    const ChatSystem::Protocol::TransHeader &header, const QByteArray &data) {
  if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
    qWarning() << "Not connected to Meta server";
    return false;
  }

  QByteArray packet;
  ChatSystem::Protocol::TransHeader networkHeader = header;
  networkHeader.toNetworkOrder();
  packet.append(reinterpret_cast<const char *>(&networkHeader),
                sizeof(ChatSystem::Protocol::TransHeader));
  packet.append(data);

  qint64 bytesWritten = m_socket->write(packet);
  return bytesWritten == packet.size();
}

bool MetaConnection::isConnected() const {
  return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void MetaConnection::onConnected() {
  qDebug() << "Connected to Meta server:" << m_ip << ":" << m_port;
  emit connected();
}

void MetaConnection::onDisconnected() {
  qDebug() << "Disconnected from Meta server";
  emit disconnected();
}

void MetaConnection::onReadyRead() { processReceivedData(); }

void MetaConnection::onError(QAbstractSocket::SocketError error) {
  QString errorMsg = m_socket ? m_socket->errorString() : "Unknown error";
  qWarning() << "Meta connection error:" << errorMsg;
  emit errorOccurred(errorMsg);
}

void MetaConnection::processReceivedData() {
  if (!m_socket) {
    return;
  }

  QByteArray newData = m_socket->readAll();
  m_buffer.append(newData);

  while (m_buffer.size() >=
         static_cast<int>(sizeof(ChatSystem::Protocol::TransHeader))) {
    ChatSystem::Protocol::TransHeader *header =
        reinterpret_cast<ChatSystem::Protocol::TransHeader *>(m_buffer.data());

    // 我们需要创建副本，因为 toHostOrder 会修改原始数据，而 buffer
    // 可能还要继续使用
    ChatSystem::Protocol::TransHeader headerCopy = *header;
    headerCopy.toHostOrder();

    if (!headerCopy.validateMagic()) {
      qWarning() << "Invalid magic number";
      m_buffer.clear();
      return;
    }

    uint32_t packetSize =
        sizeof(ChatSystem::Protocol::TransHeader) + headerCopy.len;

    if (m_buffer.size() < static_cast<int>(packetSize)) {
      break;
    }

    QByteArray body =
        m_buffer.mid(sizeof(ChatSystem::Protocol::TransHeader), headerCopy.len);

    emit messageReceived(headerCopy, body);

    m_buffer.remove(0, static_cast<int>(packetSize));
  }
}
