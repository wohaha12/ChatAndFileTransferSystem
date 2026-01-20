#pragma once

#include <string>
#include <atomic>
#include <map>
#include <mutex>
#include <QObject>

namespace ChatSystem {
namespace StorageServer {
namespace Network {

class Connection;

/**
 * @brief TCP服务器类（Epoll封装）
 */
class TcpServer : public QObject {
    Q_OBJECT
public:
    explicit TcpServer(QObject* parent = nullptr);
    ~TcpServer();
    
    /**
     * @brief 启动服务器
     * @param ip 监听IP地址
     * @param port 监听端口
     * @return 成功返回true，失败返回false
     */
    bool start(const std::string& ip, uint16_t port);
    
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
    
private slots:
    /**
     * @brief Epoll事件循环
     */
    void eventLoop();
    
private:
    /**
     * @brief 处理新连接
     */
    void handleNewConnection();
    
    /**
     * @brief 处理连接关闭
     * @param socketDescriptor 套接字描述符
     */
    void handleConnectionClose(qintptr socketDescriptor);
    
    /**
     * @brief 处理可读事件
     * @param socketDescriptor 套接字描述符
     */
    void handleReadableEvent(qintptr socketDescriptor);
    
private:
    int m_epollFd;                        // Epoll文件描述符
    int m_listenFd;                       // 监听套接字
    std::map<qintptr, Connection*> m_connections; // 连接映射
    QThread* m_eventThread;               // 事件循环线程
    std::atomic<bool> m_running;                // 运行状态
    std::mutex m_mutex;                       // 互斥锁
    std::atomic<int> m_connectionCount;       // 当前连接数
};

} // namespace Network
} // namespace StorageServer
} // namespace ChatSystem
