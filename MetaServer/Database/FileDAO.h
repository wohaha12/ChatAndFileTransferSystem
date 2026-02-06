#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace ChatSystem {
namespace MetaServer {

namespace Core {
struct FileMetaInfo;
}

namespace Database {

class DatabaseManager;

/**
 * @brief 文件数据访问对象类
 */
class FileDAO {
public:
  explicit FileDAO(DatabaseManager *dbManager);
  ~FileDAO();

  bool checkPhysicalFileExists(const std::string &fileHash,
                               std::string &storagePath, uint32_t &serverId);
  bool createPhysicalFileRecord(const std::string &fileHash, uint64_t fileSize,
                                const std::string &storagePath,
                                uint32_t serverId);
  bool createLogicalFileRecord(uint64_t userId, const std::string &fileName,
                               const std::string &filePath, uint64_t fileSize,
                               const std::string &fileHash, uint8_t fileType,
                               uint64_t parentId, uint64_t &fileId);
  bool increaseFileRef(const std::string &fileHash);
  bool decreaseFileRef(const std::string &fileHash);
  bool
  getUserFiles(uint64_t userId, uint64_t parentId,
               std::vector<ChatSystem::MetaServer::Core::FileMetaInfo> &files);
  bool getFileInfo(uint64_t fileId,
                   ChatSystem::MetaServer::Core::FileMetaInfo &fileInfo);
  bool deleteFileRecord(uint64_t userId, uint64_t fileId);
  bool renameFile(uint64_t userId, uint64_t fileId, const std::string &newName);
  bool moveFile(uint64_t userId, uint64_t fileId, uint64_t newParentId);
  bool getStorageServers(
      std::vector<std::tuple<uint32_t, std::string, uint16_t, int>> &servers);
  bool updateStorageServerStatus(uint32_t serverId, uint8_t status);

private:
  DatabaseManager *m_dbManager;
};

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem
