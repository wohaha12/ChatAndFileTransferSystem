#pragma once

#include <string>
#include <atomic>
#include <map>
#include <mutex>
#include <QObject>

namespace ChatSystem {
namespace StorageServer {
namespace Core {

class TcpServer;
class TokenValidator;
class StorageEngine;
class RPCClient;

/**
 * @brief 存储服务器核心类
 */
class StorageServer : public QObject {
    Q_OBJECT
public:
    explicit StorageServer(QObject* parent = nullptr);
    ~StorageServer();
    
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
     * @brief 设置服务器ID
     * @param serverId 服务器ID
     */
    void setServerId(uint32_t serverId);
    
    /**
     * @brief 获取服务器ID
     * @return 服务器ID
     */
    uint32_t getServerId() const;
    
public slots:
    /**
     * @brief 处理客户端消息
     * @param socketDescriptor 套接字描述符
     * @param header 数据包头部
     * @param data 数据包体
     */
    void handleClientMessage(qintptr socketDescriptor, const Protocol::TransHeader& header, const QByteArray& data);
    
    /**
     * @brief 定期上报状态
     */
    void reportStatus();
    
private:
    /**
     * @brief 处理上传分片请求
     * @param socketDescriptor 套接字描述符
     * @param req 上传分片请求
     */
    void handleUploadChunkRequest(qintptr socketDescriptor, const Protocol::UploadChunk& req);
    
    /**
     * @brief 处理下载分片请求
     * @param socketDescriptor 套接字描述符
     * @param req 下载分片请求
     */
    void handleDownloadChunkRequest(qintptr socketDescriptor, const Protocol::DownloadChunk& req);
    
    /**
     * @brief 通知Meta服务器上传完成
     * @param token 上传令牌
     * @param filePath 文件路径
     */
    void notifyUploadComplete(const std::string& token, const std::string& filePath);
    
private:
    TcpServer* m_tcpServer;               // TCP服务器
    TokenValidator* m_tokenValidator;      // Token验证器
    StorageEngine* m_storageEngine;       // 存储引擎
    RPCClient* m_rpcClient;               // RPC客户端(向Meta汇报)
    
    uint32_t m_serverId;                  // 服务器ID
    std::string m_metaServerIp;           // Meta服务器IP
    uint16_t m_metaServerPort;            // Meta服务器端口
    
    std::map<qintptr, std::string> m_uploadTokens; // 连接对应的上传令牌
    std::mutex m_mutex;                       // 互斥锁
    std::atomic<bool> m_running;                // 运行状态
    QTimer* m_statusTimer;                // 状态上报定时器
};

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem
