#include "TcpServer.h"
#include "Connection.h"
#include <QHostAddress>
#include <QTcpSocket>
#include <iostream>

namespace ChatSystem {
namespace StorageServer {
namespace Network {

TcpServer::TcpServer(QObject *parent)
    : QTcpServer(parent), m_connectionCount(0), m_running(false) {}

TcpServer::~TcpServer() {
  stop();

  for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
    delete it.value();
  }
  m_connections.clear();
}

bool TcpServer::start(quint16 port) {
  if (m_running) {
    std::cerr << "服务器已在运行" << std::endl;
    return false;
  }

  if (!listen(QHostAddress::Any, port)) {
    std::cerr << "监听失败: " << errorString().toStdString() << std::endl;
    return false;
  }

  m_running = true;

  // 注：QTcpServer::newConnection() 信号无参数，使用 incomingConnection()
  // 虚函数处理新连接

  std::cout << "Qt6 TCP服务器启动成功，端口: " << port << std::endl;
  return true;
}

void TcpServer::stop() {
  if (!m_running) {
    return;
  }

  m_running = false;

  close();

  for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
    Connection *connection = it.value();
    if (connection) {
      connection->close();
    }
  }

  for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
    delete it.value();
  }
  m_connections.clear();
  m_connectionCount = 0;

  std::cout << "Qt6 TCP服务器已停止" << std::endl;
}

bool TcpServer::isRunning() const { return m_running; }

int TcpServer::getConnectionCount() const { return m_connectionCount; }

void TcpServer::incomingConnection(qintptr socketDescriptor) {
  QTcpSocket *socket = new QTcpSocket(this);

  if (!socket->setSocketDescriptor(socketDescriptor)) {
    std::cerr << "设置套接字描述符失败" << std::endl;
    delete socket;
    return;
  }

  handleNewConnection(socketDescriptor);
}

void TcpServer::handleNewConnection(qintptr socketDescriptor) {
  Connection *connection = new Connection(socketDescriptor, this);

  {
    QMutexLocker locker(&m_mutex);
    m_connections[socketDescriptor] = connection;
    m_connectionCount.fetchAndAddRelaxed(1);
  }

  connect(connection, &Connection::messageReceived, this,
          [this, socketDescriptor](const Protocol::TransHeader &header,
                                   const QByteArray &data) {
            emit messageReceived(socketDescriptor, header, data);
          });

  connect(
      connection, &Connection::connectionClosed, this,
      [this, socketDescriptor]() { handleConnectionClose(socketDescriptor); });

  emit newConnection(socketDescriptor);
  std::cout << "新连接到达: " << socketDescriptor << std::endl;
}

void TcpServer::handleConnectionClose(qintptr socketDescriptor) {
  QMutexLocker locker(&m_mutex);

  auto it = m_connections.find(socketDescriptor);
  if (it != m_connections.end()) {
    Connection *connection = it.value();
    m_connections.erase(it);
    m_connectionCount.fetchAndSubRelaxed(1);

    emit connectionClosed(socketDescriptor);
    std::cout << "连接关闭: " << socketDescriptor << std::endl;
  }
}

} // namespace Network
} // namespace StorageServer
} // namespace ChatSystem
