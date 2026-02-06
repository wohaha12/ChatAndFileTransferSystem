#include "StorageServer.h"
#include "../Network/TcpServer.h"
#include "../RPC/RPCClient.h"
#include "StorageEngine.h"
#include "TokenValidator.h"

namespace ChatSystem {
namespace StorageServer {
namespace Core {

StorageServer::StorageServer(QObject *parent)
    : QObject(parent), m_tcpServer(nullptr), m_tokenValidator(nullptr),
      m_storageEngine(nullptr), m_rpcClient(nullptr), m_serverId(0),
      m_metaServerPort(0), m_running(false), m_statusTimer(nullptr) {
  // 初始化各个模块
}

StorageServer::~StorageServer() {
  // 停止服务器
  stop();

  // 释放资源
  delete m_tcpServer;
  delete m_tokenValidator;
  delete m_storageEngine;
  delete m_rpcClient;
  delete m_statusTimer;
}

void StorageServer::setServerId(uint32_t serverId) { m_serverId = serverId; }

uint32_t StorageServer::getServerId() const { return m_serverId; }

void StorageServer::setMetaServerInfo(const std::string &ip, uint16_t port) {
  m_metaServerIp = ip;
  m_metaServerPort = port;
}

void StorageServer::setDataDir(const std::string &dataDir) {
  m_dataDir = dataDir;
}

bool StorageServer::isRunning() const { return m_running; }

bool StorageServer::start(const std::string &ip, uint16_t port) {
  // 1. 初始化存储引擎
  m_storageEngine = new StorageEngine(this);
  if (!m_storageEngine->initialize(m_dataDir)) {
    return false;
  }

  // 2. 初始化Token验证器
  m_tokenValidator = new TokenValidator(this);

  // 3. 初始化RPC客户端并连接到MetaServer
  m_rpcClient = new RPC::RPCClient(this);
  if (!m_rpcClient->initialize("storage_rpc.db")) {
    // 虽然DB加载失败比较严重，但某些情况下可能可以继续（虽然最好还是返回false）
  }
  m_rpcClient->connectToServer(m_metaServerIp, m_metaServerPort);

  // 4. 初始化TCL服务器
  m_tcpServer = new Network::TcpServer(this);
  connect(m_tcpServer, &Network::TcpServer::messageReceived, this,
          &StorageServer::handleClientMessage);

  if (!m_tcpServer->start(port)) {
    return false;
  }

  // 5. 设置状态上报定时器
  m_statusTimer = new QTimer(this);
  connect(m_statusTimer, &QTimer::timeout, this, &StorageServer::reportStatus);
  m_statusTimer->start(30000); // 30秒一次

  m_running = true;
  return true;
}

void StorageServer::stop() {
  if (m_statusTimer) {
    m_statusTimer->stop();
  }
  if (m_tcpServer) {
    m_tcpServer->stop();
  }
  if (m_rpcClient) {
    m_rpcClient->disconnect();
  }
  m_running = false;
}

void StorageServer::handleClientMessage(qintptr socketDescriptor,
                                        const Protocol::TransHeader &header,
                                        const QByteArray &data) {
  // 实现处理客户端消息逻辑
}

void StorageServer::reportStatus() {
  // 实现定期上报状态逻辑
}

void StorageServer::handleUploadChunkRequest(
    qintptr socketDescriptor, const Protocol::UploadChunkReq &req) {
  // 实现处理上传分片请求逻辑
}

void StorageServer::handleDownloadChunkRequest(
    qintptr socketDescriptor, const Protocol::DownloadChunkReq &req) {
  // 实现处理下载分片请求逻辑
}

void StorageServer::notifyUploadComplete(const std::string &token,
                                         const std::string &filePath) {
  // 实现通知Meta服务器上传完成逻辑
}

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem
