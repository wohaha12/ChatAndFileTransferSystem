#include "Connection.h"

namespace ChatSystem {
namespace StorageServer {
namespace Network {

Connection::Connection(qintptr socketDescriptor, QObject *parent)
    : QObject(parent), m_socketDescriptor(socketDescriptor), m_socket(nullptr),
      m_connected(false) {
  // 初始化套接字
}

Connection::~Connection() {
  // 关闭连接
  close();
}

qintptr Connection::socketDescriptor() const { return m_socketDescriptor; }

bool Connection::sendData(const TransHeader &header, const QByteArray &data) {
  // 实现发送数据逻辑
  return false;
}

QByteArray &Connection::receiveBuffer() { return m_receiveBuffer; }

bool Connection::isConnected() const { return m_connected; }

void Connection::close() {
  // 实现关闭连接逻辑
  m_connected = false;
}

void Connection::onSocketReadyRead() {
  // 实现处理套接字就绪读取逻辑
  m_receiveBuffer.append(m_socket->readAll());
  processReceivedData();
}

void Connection::onSocketError(QAbstractSocket::SocketError socketError) {
  // 实现处理套接字错误逻辑
  emit errorOccurred(m_socket->errorString());
}

void Connection::processReceivedData() {
  // 实现处理接收到的数据逻辑
}

} // namespace Network
} // namespace StorageServer
} // namespace ChatSystem
