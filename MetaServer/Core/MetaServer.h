#pragma once

#include <atomic>
#include <map>
#include <mutex>
#include <string>

#include "InternalProtocol.h"
#include "ProtocolBodies.h"
#include "TransHeader.h"
#include <QObject>

namespace ChatSystem {
namespace MetaServer {

namespace Network {
class TcpServer;
class ClientConnection;
} // namespace Network

namespace Database {
class DatabaseManager;
}

namespace Cache {
class RedisManager;
}

namespace Core {
class AuthModule;
class LoadBalancer;
class FileMetaMgr;

/**
 * @brief Meta服务器核心类
 */
class MetaServer : public QObject {
  Q_OBJECT
public:
  explicit MetaServer(QObject *parent = nullptr);
  ~MetaServer();

  /**
   * @brief 启动服务器
   */
  bool start(const std::string &ip, uint16_t port);

  /**
   * @brief 停止服务器
   */
  void stop();

  /**
   * @brief 获取服务器状态
   */
  bool isRunning() const;

public slots:
  /**
   * @brief 处理客户端连接
   */
  void handleClientConnection();

  /**
   * @brief 处理客户端消息
   */
  void handleClientMessage(qintptr socketDescriptor,
                           const ::ChatSystem::Protocol::TransHeader &header,
                           const QByteArray &data);

  /**
   * @brief 处理Storage服务器状态上报
   */
  void
  handleStorageStatusReport(uint32_t serverId,
                            const ::ChatSystem::Protocol::StatusReport &report);

  /**
   * @brief 处理上传完成通知
   */
  void
  handleUploadComplete(const ::ChatSystem::Protocol::UploadComplete &complete);

private:
  /**
   * @brief 处理登录请求
   */
  void handleLoginRequest(qintptr socketDescriptor,
                          const ::ChatSystem::Protocol::LoginReq &req);

  /**
   * @brief 处理上传请求
   */
  void handleUploadRequest(qintptr socketDescriptor,
                           const ::ChatSystem::Protocol::UploadRequest &req);

  /**
   * @brief 处理下载请求
   */
  void
  handleDownloadRequest(qintptr socketDescriptor,
                        const ::ChatSystem::Protocol::DownloadRequest &req);

  /**
   * @brief 处理文件列表请求
   */
  void handleFileListRequest(qintptr socketDescriptor, uint64_t userId,
                             uint64_t parentId);

  /**
   * @brief 处理删除文件请求
   */
  void handleDeleteFileRequest(qintptr socketDescriptor, uint64_t userId,
                               uint64_t fileId);

  /**
   * @brief 处理重命名文件请求
   */
  void handleRenameFileRequest(qintptr socketDescriptor, uint64_t userId,
                               uint64_t fileId, const std::string &newName);

  /**
   * @brief 生成上传令牌
   */
  std::string generateUploadToken(uint64_t userId, const std::string &fileHash);

  /**
   * @brief 发送响应
   */
  void sendResponse(qintptr socketDescriptor, uint32_t cmd,
                    const QByteArray &data);

private:
  ::ChatSystem::MetaServer::Network::TcpServer *m_tcpServer;    // TCP服务器
  ::ChatSystem::MetaServer::Core::AuthModule *m_authModule;     // 认证模块
  ::ChatSystem::MetaServer::Core::LoadBalancer *m_loadBalancer; // 负载均衡器
  ::ChatSystem::MetaServer::Core::FileMetaMgr
      *m_fileMetaMgr; // 文件元数据管理器
  ::ChatSystem::MetaServer::Cache::RedisManager *m_redisManager; // Redis管理器
  ::ChatSystem::MetaServer::Database::DatabaseManager
      *m_dbManager; // 数据库管理器

  std::map<qintptr, ::ChatSystem::MetaServer::Network::ClientConnection *>
      m_connections;           // 客户端连接映射
  std::mutex m_mutex;          // 互斥锁
  std::atomic<bool> m_running; // 运行状态
};

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem
