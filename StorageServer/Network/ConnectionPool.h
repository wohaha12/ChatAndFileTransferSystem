#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <QObject>

namespace ChatSystem {
namespace StorageServer {
namespace Network {

class Connection;

/**
 * @brief 连接池类
 */
class ConnectionPool : public QObject {
    Q_OBJECT
public:
    explicit ConnectionPool(QObject* parent = nullptr);
    ~ConnectionPool();
    
    /**
     * @brief 初始化连接池
     * @param maxConnections 最大连接数
     * @param ip 服务器IP地址
     * @param port 服务器端口
     * @return 成功返回true，失败返回false
     */
    bool initialize(size_t maxConnections, const std::string& ip, uint16_t port);
    
    /**
     * @brief 获取连接
     * @param timeout 超时时间（毫秒），默认-1表示无限等待
     * @return 连接指针，失败返回nullptr
     */
    Connection* getConnection(int timeout = -1);
    
    /**
     * @brief 释放连接
     * @param conn 连接指针
     */
    void releaseConnection(Connection* conn);
    
    /**
     * @brief 获取连接池状态
     * @return 连接池是否正在运行
     */
    bool isRunning() const;
    
    /**
     * @brief 获取当前活跃连接数
     * @return 当前活跃连接数
     */
    size_t getActiveConnectionCount() const;
    
    /**
     * @brief 获取当前空闲连接数
     * @return 当前空闲连接数
     */
    size_t getIdleConnectionCount() const;
    
    /**
     * @brief 关闭连接池
     */
    void close();
    
private:
    /**
     * @brief 创建新连接
     * @return 新连接指针，失败返回nullptr
     */
    Connection* createConnection();
    
private:
    size_t m_maxConnections;               // 最大连接数
    std::string m_ip;                     // 服务器IP地址
    uint16_t m_port;                      // 服务器端口
    
    std::queue<Connection*> m_idleConnections; // 空闲连接队列
    std::mutex m_mutex;                       // 互斥锁
    std::condition_variable m_condition;       // 条件变量
    std::atomic<size_t> m_activeCount;        // 活跃连接数
    std::atomic<bool> m_running;              // 运行状态
};

} // namespace Network
} // namespace StorageServer
} // namespace ChatSystem
