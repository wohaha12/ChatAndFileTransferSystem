#pragma once

#include "CommonProtocol.h"
#include "InternalProtocol.h"
#include "TransHeader.h"
#include <memory>

namespace storage_server {
namespace rpc {

class StorageServer;

/**
 * @class RPCServer
 * @brief RPC服务器，用于接收MetaServer的RPC指令
 */
class RPCServer {
public:
  explicit RPCServer(StorageServer *server);
  ~RPCServer();

  bool Start(uint16_t port);
  void Stop();

  bool HandleRpcRequest(const ::ChatSystem::Protocol::TransHeader &header,
                        const char *data, size_t length);

private:
  bool
  HandleHeartbeatRequest(const ::ChatSystem::Protocol::Heartbeat &heartbeat);
  bool HandleStatusQuery(const ::ChatSystem::Protocol::StatusReport &request);
  bool HandleDeleteChunkRequest(
      const ::ChatSystem::Protocol::DeletePhysicalFile &request);
  bool HandleFileMigrationRequest(
      const ::ChatSystem::Protocol::UploadComplete &request);

private:
  StorageServer *server_; ///< StorageServer实例指针
};

} // namespace rpc
} // namespace storage_server
