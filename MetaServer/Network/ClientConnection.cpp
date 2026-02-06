#include "ClientConnection.h"
#include "../../Common/Protocol/TransHeader.h"
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace Network {

ClientConnection::ClientConnection(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket), m_userId(0), m_connected(false),
      m_sequenceNumber(0) {
  if (m_socket) {
    m_socket->setParent(this);
    connect(m_socket, &QTcpSocket::readyRead, this,
            &ClientConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this,
            &ClientConnection::onSocketError);
    connect(m_socket, &QTcpSocket::disconnected, this,
            &ClientConnection::onDisconnected);
    m_connected = (m_socket->state() == QAbstractSocket::ConnectedState);
  }
}

ClientConnection::~ClientConnection() { disconnect(); }

void ClientConnection::disconnect() {
  if (m_socket) {
    m_socket->disconnectFromHost();
  }
  m_connected = false;
}

qintptr ClientConnection::socketDescriptor() const {
  return m_socket ? m_socket->socketDescriptor() : -1;
}

bool ClientConnection::isConnected() const {
  return m_connected && m_socket &&
         m_socket->state() == QAbstractSocket::ConnectedState;
}

bool ClientConnection::sendData(
    const ::ChatSystem::Protocol::TransHeader &header, const QByteArray &data) {
  if (!isConnected()) {
    std::cerr << "连接未建立，无法发送数据" << std::endl;
    return false;
  }

  QByteArray headerData;
  headerData.resize(sizeof(::ChatSystem::Protocol::TransHeader));
  memcpy(headerData.data(), &header,
         sizeof(::ChatSystem::Protocol::TransHeader));

  qint64 totalBytes = m_socket->write(headerData + data);
  if (totalBytes == -1) {
    std::cerr << "发送数据失败: " << m_socket->errorString().toStdString()
              << std::endl;
    return false;
  }

  m_socket->flush();
  return true;
}

void ClientConnection::setUserId(uint64_t userId) { m_userId = userId; }

uint64_t ClientConnection::getUserId() const { return m_userId; }

void ClientConnection::onReadyRead() {
  QByteArray data = m_socket->readAll();
  m_receiveBuffer.append(data);
  processReceivedData();
}

void ClientConnection::onSocketError(QAbstractSocket::SocketError socketError) {
  std::cerr << "套接字错误: " << socketError << " ("
            << m_socket->errorString().toStdString() << ")" << std::endl;
  disconnect();
}

void ClientConnection::onDisconnected() {
  std::cout << "客户端断开连接" << std::endl;
  m_connected = false;
  emit connectionClosed(socketDescriptor());
}

void ClientConnection::processReceivedData() {
  while (m_receiveBuffer.size() >=
         sizeof(::ChatSystem::Protocol::TransHeader)) {
    ::ChatSystem::Protocol::TransHeader header;
    memcpy(&header, m_receiveBuffer.constData(),
           sizeof(::ChatSystem::Protocol::TransHeader));

    header.toHostOrder();

    if (!header.validateMagic()) {
      std::cerr << "无效的魔数: 0x" << std::hex << header.magic << std::endl;
      m_receiveBuffer.clear();
      return;
    }

    uint32_t bodySize = header.len;
    uint32_t totalSize = sizeof(::ChatSystem::Protocol::TransHeader) + bodySize;

    if (m_receiveBuffer.size() < totalSize) {
      break;
    }

    QByteArray body = m_receiveBuffer.mid(
        sizeof(::ChatSystem::Protocol::TransHeader), bodySize);
    m_receiveBuffer.remove(0, totalSize);

    if (!header.validateChecksum(body.constData())) {
      std::cerr << "校验和失败" << std::endl;
      continue;
    }

    emit messageReceived(socketDescriptor(), header, body);
  }
}

} // namespace Network
} // namespace MetaServer
} // namespace ChatSystem