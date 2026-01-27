#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QMap>
#include <QMutex>
#include <QAtomicInt>
#include <atomic>
#include "../../Common/Protocol/TransHeader.h"

namespace ChatSystem {
namespace StorageServer {
namespace Network {

class Connection;

/**
 * @brief TCP服务器类（Qt6原生网络实现）
 * @details 使用QTcpServer实现跨平台网络通信
 *          Qt6在Windows上自动使用IOCP，在Linux上自动使用Epoll
 */
class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(QObject* parent = nullptr);
    ~TcpServer();
    
    /**
     * @brief 启动服务器
     * @param port 监听端口
     * @return 成功返回true，失败返回false
     */
    bool start(quint16 port);
    
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
    void messageReceived(qintptr socketDescriptor, const Protocol::TransHeader& header, const QByteArray& data);
    
protected:
    /**
     * @brief 重写QTcpServer的incomingConnection方法
     * @param socketDescriptor 套接字描述符
     */
    void incomingConnection(qintptr socketDescriptor) override;
    
private:
    /**
     * @brief 处理新连接
     * @param socketDescriptor 套接字描述符
     */
    void handleNewConnection(qintptr socketDescriptor);
    
    /**
     * @brief 处理连接关闭
     * @param socketDescriptor 套接字描述符
     */
    void handleConnectionClose(qintptr socketDescriptor);
    
private:
    QMap<qintptr, Connection*> m_connections; // 连接映射
    QMutex m_mutex;                              // 互斥锁
    QAtomicInt m_connectionCount;                   // 当前连接数
    QAtomicInt m_running;                          // 运行状态
};

} // namespace Network
} // namespace StorageServer
} // namespace ChatSystem
