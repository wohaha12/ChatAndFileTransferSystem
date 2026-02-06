#pragma once

#include <QObject>
#include <QTimer>
#include <atomic>
#include <map>
#include <mutex>
#include <string>

#include "CommonProtocol.h"
#include "ProtocolBodies.h"
#include "TransHeader.h"

namespace ChatSystem {
namespace StorageServer {
namespace Network {
class TcpServer;
}
namespace RPC {
class RPCClient;
}
} // namespace StorageServer
} // namespace ChatSystem

namespace ChatSystem {
namespace StorageServer {
namespace Core {

class TokenValidator;
class StorageEngine;

/**
 * @brief 存储服务器核心类
 */
class StorageServer : public QObject {
  Q_OBJECT
public:
  explicit StorageServer(QObject *parent = nullptr);
  ~StorageServer();

  bool start(const std::string &ip, uint16_t port);
  void stop();
  bool isRunning() const;
  void setServerId(uint32_t serverId);
  uint32_t getServerId() const;
  void setMetaServerInfo(const std::string &ip, uint16_t port);
  void setDataDir(const std::string &dataDir);

public slots:
  void handleClientMessage(qintptr socketDescriptor,
                           const ::ChatSystem::Protocol::TransHeader &header,
                           const QByteArray &data);
  void reportStatus();

private:
  void
  handleUploadChunkRequest(qintptr socketDescriptor,
                           const ::ChatSystem::Protocol::UploadChunkReq &req);
  void handleDownloadChunkRequest(
      qintptr socketDescriptor,
      const ::ChatSystem::Protocol::DownloadChunkReq &req);
  void notifyUploadComplete(const std::string &token,
                            const std::string &filePath);

private:
  ChatSystem::StorageServer::Network::TcpServer *m_tcpServer; // TCP服务器
  TokenValidator *m_tokenValidator;                           // Token验证器
  StorageEngine *m_storageEngine;                             // 存储引擎
  ChatSystem::StorageServer::RPC::RPCClient
      *m_rpcClient; // RPC客户端(向Meta汇报)

  uint32_t m_serverId;        // 服务器ID
  std::string m_metaServerIp; // Meta服务器IP
  uint16_t m_metaServerPort;  // Meta服务器端口
  std::string m_dataDir;      // 数据存储目录

  std::map<qintptr, std::string> m_uploadTokens; // 连接对应的上传令牌
  std::mutex m_mutex;                            // 互斥锁
  std::atomic<bool> m_running;                   // 运行状态
  QTimer *m_statusTimer;                         // 状态上报定时器
};

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem
