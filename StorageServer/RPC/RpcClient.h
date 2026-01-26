#pragma once

#include <string>
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>
#include <vector>
#include "UploadRecordDB.h"

namespace ChatSystem {
namespace StorageServer {
namespace RPC {

/**
 * @brief RPC客户端类（增强版）
 * @details 用于StorageServer向MetaServer发送RPC请求，支持重试和本地持久化
 */
class RPCClient : public QObject {
    Q_OBJECT
public:
    explicit RPCClient(QObject* parent = nullptr);
    ~RPCClient();
    
    /**
     * @brief 初始化RPC客户端
     * @param dbPath 本地数据库路径
     * @return 成功返回true，失败返回false
     */
    bool initialize(const std::string& dbPath);
    
    /**
     * @brief 连接到MetaServer
     * @param host MetaServer主机地址
     * @param port MetaServer端口
     * @return 成功返回true，失败返回false
     */
    bool connectToServer(const std::string& host, uint16_t port);
    
    /**
     * @brief 断开连接
     */
    void disconnect();
    
    /**
     * @brief 是否已连接
     * @return 已连接返回true，未连接返回false
     */
    bool isConnected() const;
    
    /**
     * @brief 发送心跳
     * @param serverId 存储服务器ID
     * @param cpuUsage CPU使用率
     * @param memoryUsage 内存使用率
     * @param diskUsage 磁盘使用率
     * @param connections 连接数
     * @return 成功返回true，失败返回false
     */
    bool sendHeartbeat(uint32_t serverId, float cpuUsage, float memoryUsage, 
                     float diskUsage, uint32_t connections);
    
    /**
     * @brief 发送上传完成通知（带重试）
     * @param userId 用户ID
     * @param fileId 文件ID
     * @param fileHash 文件哈希
     * @param fileSize 文件大小
     * @param storagePath 存储路径
     * @param uploadToken 上传令牌
     * @return 成功返回true，失败返回false
     */
    bool sendUploadComplete(uint64_t userId, uint64_t fileId, const std::string& fileHash,
                        uint64_t fileSize, const std::string& storagePath,
                        const std::string& uploadToken);
    
    /**
     * @brief 发送下载完成通知
     * @param userId 用户ID
     * @param fileId 文件ID
     * @return 成功返回true，失败返回false
     */
    bool sendDownloadComplete(uint64_t userId, uint64_t fileId);
    
signals:
    /**
     * @brief 连接状态改变信号
     * @param connected 是否已连接
     */
    void connectionStateChanged(bool connected);
    
    /**
     * @brief 收到响应信号
     * @param response 响应数据
     */
    void responseReceived(const QByteArray& response);
    
private slots:
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
    
    /**
     * @brief 定时重试槽函数
     */
    void onRetryTimer();
    
private:
    /**
     * @brief 发送RPC请求
     * @param request 请求数据
     * @return 成功返回true，失败返回false
     */
    bool sendRequest(const QByteArray& request);
    
    /**
     * @brief 重试待确认的上传记录
     */
    void retryPendingNotifications();
    
private:
    QTcpSocket* m_socket;           // TCP套接字
    bool m_connected;                // 连接状态
    QByteArray m_receiveBuffer;        // 接收缓冲区
    
    UploadRecordDB* m_uploadRecordDB; // 上传记录数据库
    QTimer* m_retryTimer;            // 重试定时器
    
    static const int MAX_RETRY_COUNT = 5;      // 最大重试次数
    static const int RETRY_INTERVAL_MS = 30000;  // 重试间隔30秒
};

} // namespace RPC
} // namespace StorageServer
} // namespace ChatSystem