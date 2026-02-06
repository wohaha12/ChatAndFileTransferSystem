#include "MetaServer.h"
#include "../Cache/RedisManager.h"
#include "../Database/DatabaseManager.h"
#include "../Network/ClientConnection.h"
#include "../Network/TcpServer.h"
#include "AuthModule.h"
#include "FileMetaMgr.h"
#include "LoadBalancer.h"
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace Core {

MetaServer::MetaServer(QObject *parent)
    : QObject(parent), m_tcpServer(nullptr), m_authModule(nullptr),
      m_loadBalancer(nullptr), m_fileMetaMgr(nullptr), m_redisManager(nullptr),
      m_dbManager(nullptr), m_running(false) {}

MetaServer::~MetaServer() {
  stop();
  delete m_tcpServer;
  delete m_authModule;
  delete m_loadBalancer;
  delete m_fileMetaMgr;
  delete m_redisManager;
  delete m_dbManager;
}

bool MetaServer::start(const std::string &ip, uint16_t port) {
  m_running = true;
  std::cout << "Meta Server started on " << ip << ":" << port << std::endl;
  return true;
}

void MetaServer::stop() {
  m_running = false;
  std::cout << "Meta Server stopped" << std::endl;
}

bool MetaServer::isRunning() const { return m_running; }

void MetaServer::handleClientConnection() {}

void MetaServer::handleClientMessage(
    qintptr socketDescriptor, const ::ChatSystem::Protocol::TransHeader &header,
    const QByteArray &data) {}

void MetaServer::handleStorageStatusReport(
    uint32_t serverId, const ::ChatSystem::Protocol::StatusReport &report) {}

void MetaServer::handleUploadComplete(
    const ::ChatSystem::Protocol::UploadComplete &complete) {}

void MetaServer::handleLoginRequest(
    qintptr socketDescriptor, const ::ChatSystem::Protocol::LoginReq &req) {}

void MetaServer::handleUploadRequest(
    qintptr socketDescriptor,
    const ::ChatSystem::Protocol::UploadRequest &req) {}

void MetaServer::handleDownloadRequest(
    qintptr socketDescriptor,
    const ::ChatSystem::Protocol::DownloadRequest &req) {}

void MetaServer::handleFileListRequest(qintptr socketDescriptor,
                                       uint64_t userId, uint64_t parentId) {}

void MetaServer::handleDeleteFileRequest(qintptr socketDescriptor,
                                         uint64_t userId, uint64_t fileId) {}

void MetaServer::handleRenameFileRequest(qintptr socketDescriptor,
                                         uint64_t userId, uint64_t fileId,
                                         const std::string &newName) {}

std::string MetaServer::generateUploadToken(uint64_t userId,
                                            const std::string &fileHash) {
  return "test-token";
}

void MetaServer::sendResponse(qintptr socketDescriptor, uint32_t cmd,
                              const QByteArray &data) {
  auto it = m_connections.find(socketDescriptor);
  if (it == m_connections.end()) {
    return;
  }

  ::ChatSystem::MetaServer::Network::ClientConnection *conn = it->second;
  if (!conn) {
    return;
  }

  ::ChatSystem::Protocol::TransHeader header;
  header.magic = 0x12345678;
  header.cmd = cmd;
  header.seq = 0;
  header.len = data.size();
  header.checksum = 0;
  header.reserved = 0;

  conn->sendData(header, data);
}

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem
