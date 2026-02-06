#include "TcpServer.h"
#include "../../Common/Protocol/TransHeader.h"
#include "ClientConnection.h"
#include <QHostAddress>
#include <QTcpServer>
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace Network {

TcpServer::TcpServer(QObject *parent)
    : QObject(parent), m_server(nullptr), m_running(false),
      m_connectionCount(0) {
  m_server = new QTcpServer(this);

  connect(m_server, &QTcpServer::newConnection, this,
          &TcpServer::onNewConnection);
}

TcpServer::~TcpServer() {
  stop();

  for (auto &pair : m_connections) {
    delete pair.second;
  }
  m_connections.clear();

  delete m_server;
}

bool TcpServer::start(const std::string &ip, uint16_t port) {
  if (m_running) {
    std::cerr << "服务器已在运行" << std::endl;
    return false;
  }

  QHostAddress address;
  if (ip == "0.0.0.0" || ip == "*") {
    address = QHostAddress::Any;
  } else {
    address = QHostAddress(QString::fromStdString(ip));
    if (address.isNull()) {
      std::cerr << "无效的IP地址: " << ip << std::endl;
      return false;
    }
  }

  if (!m_server->listen(address, port)) {
    std::cerr << "服务器监听失败: " << m_server->errorString().toStdString()
              << std::endl;
    return false;
  }

  m_running = true;
  std::cout << "TCP服务器启动成功: " << ip << ":" << port << std::endl;
  return true;
}

void TcpServer::stop() {
  if (!m_running) {
    return;
  }

  m_running = false;

  for (auto &pair : m_connections) {
    delete pair.second;
  }
  m_connections.clear();

  m_server->close();

  std::cout << "TCP服务器已停止" << std::endl;
}

bool TcpServer::isRunning() const { return m_running; }

int TcpServer::getConnectionCount() const { return m_connectionCount; }

bool TcpServer::sendData(qintptr socketDescriptor,
                         const ::ChatSystem::Protocol::TransHeader &header,
                         const QByteArray &data) {
  std::lock_guard<std::mutex> lock(m_mutex);

  auto it = m_connections.find(socketDescriptor);
  if (it == m_connections.end()) {
    std::cerr << "连接不存在: " << socketDescriptor << std::endl;
    return false;
  }

  return it->second->sendData(header, data);
}

void TcpServer::onNewConnection() {
  QTcpSocket *socket = m_server->nextPendingConnection();
  if (!socket) {
    return;
  }

  qintptr socketDescriptor = socket->socketDescriptor();

  ClientConnection *connection = new ClientConnection(socket, this);

  connect(connection, &ClientConnection::connectionClosed, this,
          &TcpServer::onConnectionClosed);
  connect(connection, &ClientConnection::messageReceived, this,
          &TcpServer::onMessageReceived);

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections[socketDescriptor] = connection;
    m_connectionCount++;
  }

  emit newConnection(socketDescriptor);
  std::cout << "新连接到达: " << socketDescriptor << std::endl;
}

void TcpServer::onConnectionClosed(qintptr socketDescriptor) {
  std::lock_guard<std::mutex> lock(m_mutex);

  auto it = m_connections.find(socketDescriptor);
  if (it != m_connections.end()) {
    delete it->second;
    m_connections.erase(it);
    m_connectionCount--;

    emit connectionClosed(socketDescriptor);
    std::cout << "连接关闭: " << socketDescriptor << std::endl;
  }
}

void TcpServer::onMessageReceived(
    qintptr socketDescriptor, const ::ChatSystem::Protocol::TransHeader &header,
    const QByteArray &data) {
  emit messageReceived(socketDescriptor, header, data);
}

} // namespace Network
} // namespace MetaServer
} // namespace ChatSystem