#include "FileDAO.h"
#include "../Core/FileMetaMgr.h"
#include "DatabaseManager.h"
#ifdef USE_NATIVE_MYSQL
#include <mysql/mysql.h>
#endif
#include <iostream>
#include <sstream>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

FileDAO::FileDAO(DatabaseManager *dbManager) : m_dbManager(dbManager) {}
FileDAO::~FileDAO() {}

bool FileDAO::checkPhysicalFileExists(const std::string &fileHash,
                                      std::string &storagePath,
                                      uint32_t &serverId) {
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "SELECT storage_path, storage_server_id FROM t_file_store WHERE "
         "file_hash = '"
      << fileHash << "'";
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return false;
  if (!result)
    return false;
  MYSQL_ROW row = mysql_fetch_row(result);
  if (!row || !row[0]) {
    mysql_free_result(result);
    return false;
  }
  storagePath = row[0];
  serverId = row[1] ? std::stoul(row[1]) : 0;
  mysql_free_result(result);
  return true;
#else
  return false;
#endif
}

bool FileDAO::createPhysicalFileRecord(const std::string &fileHash,
                                       uint64_t fileSize,
                                       const std::string &storagePath,
                                       uint32_t serverId) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "INSERT INTO t_file_store (file_hash, file_size, storage_path, "
         "storage_server_id, ref_count) "
         "VALUES ('"
      << fileHash << "', " << fileSize << ", '" << storagePath << "', "
      << serverId << ", 1)";
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FileDAO::createLogicalFileRecord(
    uint64_t userId, const std::string &fileName, const std::string &filePath,
    uint64_t fileSize, const std::string &fileHash, uint8_t fileType,
    uint64_t parentId, uint64_t &fileId) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "INSERT INTO t_file_info (user_id, file_name, file_path, file_size, "
         "file_hash, file_type, parent_id) "
         "VALUES ("
      << userId << ", '" << fileName << "', '" << filePath << "', " << fileSize
      << ", '" << fileHash << "', " << static_cast<int>(fileType) << ", "
      << parentId << ")";
  uint64_t affectedRows = 0;
  if (!m_dbManager->executeUpdate(oss.str(), affectedRows))
    return false;
  fileId = m_dbManager->getLastInsertId();
  return affectedRows > 0;
}

bool FileDAO::increaseFileRef(const std::string &fileHash) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "UPDATE t_file_store SET ref_count = ref_count + 1, last_access_time "
         "= CURRENT_TIMESTAMP WHERE file_hash = '"
      << fileHash << "'";
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FileDAO::decreaseFileRef(const std::string &fileHash) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "UPDATE t_file_store SET ref_count = ref_count - 1, last_access_time "
         "= CURRENT_TIMESTAMP WHERE file_hash = '"
      << fileHash << "'";
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FileDAO::getUserFiles(uint64_t userId, uint64_t parentId,
                           std::vector<Core::FileMetaInfo> &files) {
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "SELECT id, user_id, file_name, file_path, file_size, file_hash, "
         "file_type, parent_id, UNIX_TIMESTAMP(create_time), "
         "UNIX_TIMESTAMP(update_time) "
         "FROM t_file_info WHERE user_id = "
      << userId << " AND parent_id = " << parentId << " AND is_deleted = 0";
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return false;
  if (!result)
    return false;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    Core::FileMetaInfo info;
    info.fileId = row[0] ? std::stoull(row[0]) : 0;
    info.userId = row[1] ? std::stoull(row[1]) : 0;
    info.fileName = row[2] ? row[2] : "";
    info.filePath = row[3] ? row[3] : "";
    info.fileSize = row[4] ? std::stoull(row[4]) : 0;
    info.fileHash = row[5] ? row[5] : "";
    info.fileType = row[6] ? static_cast<uint8_t>(std::stoi(row[6])) : 0;
    info.parentId = row[7] ? std::stoull(row[7]) : 0;
    info.createTime = row[8] ? std::stoull(row[8]) : 0;
    info.updateTime = row[9] ? std::stoull(row[9]) : 0;
    files.push_back(info);
  }
  mysql_free_result(result);
  return true;
#else
  return false;
#endif
}

bool FileDAO::getFileInfo(uint64_t fileId, Core::FileMetaInfo &fileInfo) {
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "SELECT id, user_id, file_name, file_path, file_size, file_hash, "
         "file_type, parent_id, UNIX_TIMESTAMP(create_time), "
         "UNIX_TIMESTAMP(update_time) "
         "FROM t_file_info WHERE id = "
      << fileId;
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return false;
  if (!result)
    return false;
  MYSQL_ROW row = mysql_fetch_row(result);
  if (!row) {
    mysql_free_result(result);
    return false;
  }
  fileInfo.fileId = row[0] ? std::stoull(row[0]) : 0;
  fileInfo.userId = row[1] ? std::stoull(row[1]) : 0;
  fileInfo.fileName = row[2] ? row[2] : "";
  fileInfo.filePath = row[3] ? row[3] : "";
  fileInfo.fileSize = row[4] ? std::stoull(row[4]) : 0;
  fileInfo.fileHash = row[5] ? row[5] : "";
  fileInfo.fileType = row[6] ? static_cast<uint8_t>(std::stoi(row[6])) : 0;
  fileInfo.parentId = row[7] ? std::stoull(row[7]) : 0;
  fileInfo.createTime = row[8] ? std::stoull(row[8]) : 0;
  fileInfo.updateTime = row[9] ? std::stoull(row[9]) : 0;
  mysql_free_result(result);
  return true;
#else
  return false;
#endif
}

bool FileDAO::deleteFileRecord(uint64_t userId, uint64_t fileId) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "UPDATE t_file_info SET is_deleted = 1, update_time = "
         "CURRENT_TIMESTAMP WHERE id = "
      << fileId << " AND user_id = " << userId;
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FileDAO::renameFile(uint64_t userId, uint64_t fileId,
                         const std::string &newName) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "UPDATE t_file_info SET file_name = '" << newName
      << "', update_time = CURRENT_TIMESTAMP WHERE id = " << fileId
      << " AND user_id = " << userId;
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FileDAO::moveFile(uint64_t userId, uint64_t fileId, uint64_t newParentId) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "UPDATE t_file_info SET parent_id = " << newParentId
      << ", update_time = CURRENT_TIMESTAMP WHERE id = " << fileId
      << " AND user_id = " << userId;
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FileDAO::getStorageServers(
    std::vector<std::tuple<uint32_t, std::string, uint16_t, int>> &servers) {
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::string sql =
      "SELECT id, ip, port, weight FROM t_storage_server WHERE status = 1";
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(sql, result))
    return false;
  if (!result)
    return false;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    uint32_t id = row[0] ? std::stoul(row[0]) : 0;
    std::string ip = row[1] ? row[1] : "";
    uint16_t port = row[2] ? static_cast<uint16_t>(std::stoul(row[2])) : 0;
    int weight = row[3] ? std::stoi(row[3]) : 1;
    servers.push_back(std::make_tuple(id, ip, port, weight));
  }
  mysql_free_result(result);
  return true;
#else
  return false;
#endif
}

bool FileDAO::updateStorageServerStatus(uint32_t serverId, uint8_t status) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "UPDATE t_storage_server SET status = " << static_cast<int>(status)
      << ", last_heartbeat = CURRENT_TIMESTAMP WHERE id = " << serverId;
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem