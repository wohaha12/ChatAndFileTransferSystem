#pragma once

#include <string>
#include <atomic>
#include <mutex>
#include <map>

#include <QObject>
#include "TransHeader.h"
#include "ProtocolBodies.h"
#include "InternalProtocol.h"

namespace ChatSystem {
namespace MetaServer {
namespace Core {

class AuthModule;
class LoadBalancer;
class FileMetaMgr;
class RedisManager;
class DatabaseManager;
class TcpServer;
class ClientConnection;

/**
 * @brief Meta服务器核心类
 */
class MetaServer : public QObject {
    Q_OBJECT
public:
    explicit MetaServer(QObject* parent = nullptr);
    ~MetaServer();
    
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
    
public slots:
    /**
     * @brief 处理客户端连接
     */
    void handleClientConnection();
    
    /**
     * @brief 处理客户端消息
     * @param socketDescriptor 套接字描述符
     * @param header 数据包头部
     * @param data 数据包体
     */
    void handleClientMessage(qintptr socketDescriptor, const Protocol::TransHeader& header, const QByteArray& data);
    
    /**
     * @brief 处理Storage服务器状态上报
     * @param serverId 服务器ID
     * @param report 状态报告
     */
    void handleStorageStatusReport(uint32_t serverId, const Protocol::StatusReport& report);
    
    /**
     * @brief 处理上传完成通知
     * @param complete 上传完成信息
     */
    void handleUploadComplete(const Protocol::UploadComplete& complete);
    
private:
    /**
     * @brief 处理登录请求
     * @param socketDescriptor 套接字描述符
     * @param req 登录请求
     */
    void handleLoginRequest(qintptr socketDescriptor, const Protocol::LoginReq& req);
    
    /**
     * @brief 处理上传请求
     * @param socketDescriptor 套接字描述符
     * @param req 上传请求
     */
    void handleUploadRequest(qintptr socketDescriptor, const Protocol::UploadRequest& req);
    
    /**
     * @brief 处理下载请求
     * @param socketDescriptor 套接字描述符
     * @param req 下载请求
     */
    void handleDownloadRequest(qintptr socketDescriptor, const Protocol::DownloadRequest& req);
    
    /**
     * @brief 处理文件列表请求
     * @param socketDescriptor 套接字描述符
     * @param userId 用户ID
     * @param parentId 父目录ID
     */
    void handleFileListRequest(qintptr socketDescriptor, uint64_t userId, uint64_t parentId);
    
    /**
     * @brief 处理删除文件请求
     * @param socketDescriptor 套接字描述符
     * @param userId 用户ID
     * @param fileId 文件ID
     */
    void handleDeleteFileRequest(qintptr socketDescriptor, uint64_t userId, uint64_t fileId);
    
    /**
     * @brief 处理重命名文件请求
     * @param socketDescriptor 套接字描述符
     * @param userId 用户ID
     * @param fileId 文件ID
     * @param newName 新文件名
     */
    void handleRenameFileRequest(qintptr socketDescriptor, uint64_t userId, uint64_t fileId, const std::string& newName);
    
    /**
     * @brief 生成上传令牌
     * @param userId 用户ID
     * @param fileHash 文件哈希
     * @return 上传令牌
     */
    std::string generateUploadToken(uint64_t userId, const std::string& fileHash);
    
    /**
     * @brief 发送响应
     * @param socketDescriptor 套接字描述符
     * @param cmd 命令字
     * @param data 响应数据
     */
    void sendResponse(qintptr socketDescriptor, uint32_t cmd, const QByteArray& data);
    
private:
    TcpServer* m_tcpServer;               // TCP服务器
    AuthModule* m_authModule;              // 认证模块
    LoadBalancer* m_loadBalancer;          // 负载均衡器
    FileMetaMgr* m_fileMetaMgr;           // 文件元数据管理器
    RedisManager* m_redisManager;          // Redis管理器
    DatabaseManager* m_dbManager;         // 数据库管理器
    
    std::map<qintptr, ClientConnection*> m_connections; // 客户端连接映射
    std::mutex m_mutex;                       // 互斥锁
    std::atomic<bool> m_running;                // 运行状态
};

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem
