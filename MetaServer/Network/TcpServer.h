#pragma once

#include "TransHeader.h"
#include <QObject>
#include <QTcpServer>
#include <atomic>
#include <map>
#include <mutex>
#include <string>

using ChatSystem::Protocol::TransHeader;

namespace ChatSystem {
namespace MetaServer {
namespace Network {

class ClientConnection;

/**
 * @brief TCP服务器类
 */
class TcpServer : public QObject {
  Q_OBJECT
public:
  explicit TcpServer(QObject *parent = nullptr);
  ~TcpServer();

  /**
   * @brief 启动服务器
   * @param ip 监听IP地址
   * @param port 监听端口
   * @return 成功返回true，失败返回false
   */
  bool start(const std::string &ip, uint16_t port);

  /**
   * @brief 停止服务器
   */
  void stop();

  /**
   * @brief 获取服务器状态
   * @return 服务器是否正在运行
   */
  bool isRunning() const;

  /**
   * @brief 获取连接数
   * @return 当前连接数
   */
  int getConnectionCount() const;

  /**
   * @brief 发送数据给客户端
   * @param socketDescriptor 套接字描述符
   * @param header 数据包头部
   * @param data 数据包体
   * @return 成功返回true，失败返回false
   */
  bool sendData(qintptr socketDescriptor, const TransHeader &header,
                const QByteArray &data);

signals:
  /**
   * @brief 新连接到达信号
   * @param socketDescriptor 套接字描述符
   */
  void newConnection(qintptr socketDescriptor);

  /**
   * @brief 连接关闭信号
   * @param socketDescriptor 套接字描述符
   */
  void connectionClosed(qintptr socketDescriptor);

  /**
   * @brief 收到消息信号
   * @param socketDescriptor 套接字描述符
   * @param header 数据包头部
   * @param data 数据包体
   */
  void messageReceived(qintptr socketDescriptor, const TransHeader &header,
                       const QByteArray &data);

private slots:
  /**
   * @brief 处理新连接
   */
  void onNewConnection();

  /**
   * @brief 处理连接关闭
   * @param socketDescriptor 套接字描述符
   */
  void onConnectionClosed(qintptr socketDescriptor);

  /**
   * @brief 处理消息接收
   * @param socketDescriptor 套接字描述符
   * @param header 数据包头部
   * @param data 数据包体
   */
  void onMessageReceived(qintptr socketDescriptor, const TransHeader &header,
                         const QByteArray &data);

private:
  QTcpServer *m_server;                                // TCP服务器实例
  std::map<qintptr, ClientConnection *> m_connections; // 客户端连接映射
  std::mutex m_mutex;                                  // 互斥锁
  std::atomic<bool> m_running;                         // 运行状态
  std::atomic<int> m_connectionCount;                  // 连接数
};

} // namespace Network
} // namespace MetaServer
} // namespace ChatSystem
