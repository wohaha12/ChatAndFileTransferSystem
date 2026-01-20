#pragma once

#include <string>
#include <map>
#include <mutex>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

namespace ChatSystem {
namespace MetaServer {
namespace RPC {

/**
 * @brief RPC服务器类
 * @details 用于MetaServer接收StorageServer的RPC请求
 */
class RPCServer : public QObject {
    Q_OBJECT
public:
    explicit RPCServer(QObject* parent = nullptr);
    ~RPCServer();
    
    /**
     * @brief 启动RPC服务器
     * @param ip 监听IP地址
     * @param port 监听端口
     * @return 成功返回true，失败返回false
     */
    bool start(const std::string& ip, uint16_t port);
    
    /**
     * @brief 停止RPC服务器
     */
    void stop();
    
    /**
     * @brief 是否正在运行
     * @return 运行中返回true，未运行返回false
     */
    bool isRunning() const;
    
signals:
    /**
     * @brief 心跳信号
     * @param serverId 存储服务器ID
     * @param cpuUsage CPU使用率
     * @param memoryUsage 内存使用率
     * @param diskUsage 磁盘使用率
     * @param connections 连接数
     */
    void heartbeatReceived(uint32_t serverId, float cpuUsage, float memoryUsage, 
                       float diskUsage, uint32_t connections);
    
    /**
     * @brief 上传完成信号
     * @param userId 用户ID
     * @param fileId 文件ID
     * @param fileHash 文件哈希
     * @param fileSize 文件大小
     * @param storagePath 存储路径
     */
    void uploadCompleteReceived(uint64_t userId, uint64_t fileId, const std::string& fileHash,
                           uint64_t fileSize, const std::string& storagePath);
    
    /**
     * @brief 下载完成信号
     * @param userId 用户ID
     * @param fileId 文件ID
     */
    void downloadCompleteReceived(uint64_t userId, uint64_t fileId);
    
private slots:
    /**
     * @brief 新连接槽函数
     */
    void onNewConnection();
    
    /**
     * @brief 套接字就绪读取槽函数
     */
    void onSocketReadyRead();
    
    /**
     * @brief 套接字错误槽函数
     * @param socketError 套接字错误
     */
    void onSocketError(QAbstractSocket::SocketError socketError);
    
    /**
     * @brief 套接字断开槽函数
     */
    void onSocketDisconnected();
    
private:
    /**
     * @brief 处理RPC请求
     * @param socket 套接字
     * @param data 请求数据
     */
    void handleRequest(QTcpSocket* socket, const QByteArray& data);
    
private:
    QTcpServer* m_server;              // TCP服务器
    std::map<QTcpSocket*, QByteArray> m_receiveBuffers; // 接收缓冲区映射
    mutable std::mutex m_mutex;         // 互斥锁
    bool m_running;                    // 运行状态
};

} // namespace RPC
} // namespace MetaServer
} // namespace ChatSystem