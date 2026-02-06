#pragma once

#include "CommonProtocol.h"
#include "TransHeader.h"
#include <QAbstractSocket>
#include <QObject>
#include <QTcpSocket>
#include <atomic>
#include <mutex>
#include <string>

using ChatSystem::Protocol::TransHeader;

namespace ChatSystem {
namespace StorageServer {
namespace Network {

/**
 * @brief 连接管理类
 */
class Connection : public QObject {
  Q_OBJECT
public:
  explicit Connection(qintptr socketDescriptor, QObject *parent = nullptr);
  ~Connection();

  /**
   * @brief 获取套接字描述符
   * @return 套接字描述符
   */
  qintptr socketDescriptor() const;

  /**
   * @brief 发送数据
   * @param header 数据包头部
   * @param data 数据包体
   * @return 成功返回true，失败返回false
   */
  bool sendData(const TransHeader &header, const QByteArray &data);

  /**
   * @brief 获取接收缓冲区
   * @return 接收缓冲区
   */
  QByteArray &receiveBuffer();

  /**
   * @brief 获取连接状态
   * @return 连接是否正常
   */
  bool isConnected() const;

  /**
   * @brief 关闭连接
   */
  void close();

signals:
  /**
   * @brief 收到完整消息信号
   * @param header 数据包头部
   * @param data 数据包体
   */
  void messageReceived(const TransHeader &header, const QByteArray &data);

  /**
   * @brief 连接关闭信号
   */
  void connectionClosed();

  /**
   * @brief 连接错误信号
   * @param error 错误信息
   */
  void errorOccurred(const QString &error);

private slots:
  /**
   * @brief 处理套接字就绪读取
   */
  void onSocketReadyRead();

  /**
   * @brief 处理套接字错误
   * @param socketError 套接字错误
   */
  void onSocketError(QAbstractSocket::SocketError socketError);

private:
  /**
   * @brief 处理接收到的数据
   */
  void processReceivedData();

private:
  qintptr m_socketDescriptor;    // 套接字描述符
  QTcpSocket *m_socket;          // TCP套接字
  QByteArray m_receiveBuffer;    // 接收缓冲区
  std::atomic<bool> m_connected; // 连接状态
  std::mutex m_mutex;            // 互斥锁
};

} // namespace Network
} // namespace StorageServer
} // namespace ChatSystem
