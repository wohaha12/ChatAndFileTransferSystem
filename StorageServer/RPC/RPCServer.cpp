#include "RPCServer.h"
#include "../Core/StorageServer.h"
#include "CommandTypes.h"
#include "CommonProtocol.h"
#include "InternalProtocol.h"

namespace storage_server {
namespace rpc {

RPCServer::RPCServer(StorageServer *server) : server_(server) {}

RPCServer::~RPCServer() {}

bool RPCServer::Start(uint16_t port) { return true; }

void RPCServer::Stop() {}

bool RPCServer::HandleRpcRequest(
    const ::ChatSystem::Protocol::TransHeader &header, const char *data,
    size_t length) {
  switch (header.cmd) {
  case ::ChatSystem::Protocol::CMD_INTERNAL_HEARTBEAT: {
    ::ChatSystem::Protocol::Heartbeat heartbeat;
    return HandleHeartbeatRequest(heartbeat);
  }
  case ::ChatSystem::Protocol::CMD_INTERNAL_STATUS_REPORT: {
    ::ChatSystem::Protocol::StatusReport request;
    return HandleStatusQuery(request);
  }
  case ::ChatSystem::Protocol::CMD_INTERNAL_DELETE_FILE: {
    ::ChatSystem::Protocol::DeletePhysicalFile request;
    return HandleDeleteChunkRequest(request);
  }
  case ::ChatSystem::Protocol::CMD_INTERNAL_UPLOAD_COMPLETE: {
    ::ChatSystem::Protocol::UploadComplete request;
    return HandleFileMigrationRequest(request);
  }
  default:
    return false;
  }
}

bool RPCServer::HandleHeartbeatRequest(
    const ::ChatSystem::Protocol::Heartbeat &heartbeat) {
  return true;
}

bool RPCServer::HandleStatusQuery(
    const ::ChatSystem::Protocol::StatusReport &request) {
  return true;
}

bool RPCServer::HandleDeleteChunkRequest(
    const ::ChatSystem::Protocol::DeletePhysicalFile &request) {
  return true;
}

bool RPCServer::HandleFileMigrationRequest(
    const ::ChatSystem::Protocol::UploadComplete &request) {
  return true;
}

} // namespace rpc
} // namespace storage_server
