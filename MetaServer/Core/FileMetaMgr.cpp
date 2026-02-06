#include "FileMetaMgr.h"
#include "../Database/DatabaseManager.h"
#include "../Database/FileDAO.h"
#ifdef USE_NATIVE_MYSQL
#include <mysql/mysql.h>
#endif
#include <cstdint>
#include <iostream>
#include <sstream>

namespace ChatSystem {
namespace MetaServer {
namespace Core {

FileMetaMgr::FileMetaMgr(QObject *parent)
    : QObject(parent), m_dbManager(nullptr), m_redisManager(nullptr) {}

FileMetaMgr::~FileMetaMgr() {}

void FileMetaMgr::setDatabaseManager(DatabaseManager *dbManager) {
  m_dbManager = dbManager;
}

void FileMetaMgr::setRedisManager(RedisManager *redisManager) {
  m_redisManager = redisManager;
}

bool FileMetaMgr::checkFileExists(const std::string &fileHash,
                                  uint64_t fileSize, std::string &storagePath) {
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return false;
  }

  std::ostringstream oss;
  oss << "SELECT storage_path, storage_server_id FROM t_file_store WHERE "
         "file_hash = '"
      << fileHash << "' AND file_size = " << fileSize;

  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result)) {
    std::cerr << "查询物理文件失败: " << fileHash << std::endl;
    return false;
  }

  if (!result) {
    return false;
  }

  MYSQL_ROW row = mysql_fetch_row(result);
  if (!row || !row[0]) {
    mysql_free_result(result);
    return false;
  }

  storagePath = row[0];
  mysql_free_result(result);

  std::cout << "文件已存在，支持秒传: " << fileHash << std::endl;
  return true;
#else
  return false;
#endif
}

uint64_t
FileMetaMgr::createFileRecord(uint64_t userId, const std::string &fileName,
                              const std::string &filePath, uint64_t fileSize,
                              const std::string &fileHash, uint64_t parentId) {
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return 0;
  }

  MYSQL *conn = m_dbManager->getConnection();
  if (!conn) {
    std::cerr << "获取数据库连接失败" << std::endl;
    return 0;
  }

  if (!m_dbManager->beginTransaction(conn)) {
    std::cerr << "开始事务失败" << std::endl;
    m_dbManager->releaseConnection(conn);
    return 0;
  }

  std::ostringstream oss;
  oss << "INSERT INTO t_file_info (user_id, file_name, file_path, file_size, "
         "file_hash, parent_id) "
         "VALUES ("
      << userId << ", '" << fileName << "', '" << filePath << "', " << fileSize
      << ", '" << fileHash << "', " << parentId << ")";

  uint64_t affectedRows = 0;
  if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
    std::cerr << "创建逻辑文件记录失败" << std::endl;
    m_dbManager->rollbackTransaction(conn);
    m_dbManager->releaseConnection(conn);
    return 0;
  }

  uint64_t fileId = m_dbManager->getLastInsertId();

  m_dbManager->commitTransaction(conn);
  m_dbManager->releaseConnection(conn);

  emit fileRecordChanged(userId, fileId);
  std::cout << "创建文件记录成功: " << fileId << std::endl;
  return fileId;
#else
  return 0;
#endif
}

std::vector<FileMetaInfo> FileMetaMgr::getUserFiles(uint64_t userId,
                                                    uint64_t parentId) {
  std::vector<FileMetaInfo> files;
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return files;
  }

  std::ostringstream oss;
  oss << "SELECT id, user_id, file_name, file_path, file_size, file_hash, "
         "file_type, parent_id, UNIX_TIMESTAMP(create_time), "
         "UNIX_TIMESTAMP(update_time) "
         "FROM t_file_info WHERE user_id = "
      << userId << " AND parent_id = " << parentId << " AND is_deleted = 0";

  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result)) {
    std::cerr << "查询用户文件列表失败: " << userId << std::endl;
    return files;
  }

  if (!result) {
    return files;
  }

  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    FileMetaInfo info;
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
#endif
  return files;
}

bool FileMetaMgr::deleteFileRecord(uint64_t userId, uint64_t fileId) {
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return false;
  }

  std::ostringstream oss;
  oss << "UPDATE t_file_info SET is_deleted = 1, update_time = "
         "CURRENT_TIMESTAMP "
         "WHERE id = "
      << fileId << " AND user_id = " << userId;

  uint64_t affectedRows = 0;
  if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
    std::cerr << "删除文件记录失败: " << fileId << std::endl;
    return false;
  }

  if (affectedRows > 0) {
    emit fileRecordChanged(userId, fileId);
    std::cout << "删除文件记录成功: " << fileId << std::endl;
  }

  return affectedRows > 0;
}

bool FileMetaMgr::renameFile(uint64_t userId, uint64_t fileId,
                             const std::string &newName) {
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return false;
  }

  std::ostringstream oss;
  oss << "UPDATE t_file_info SET file_name = '" << newName
      << "', update_time = CURRENT_TIMESTAMP WHERE id = " << fileId
      << " AND user_id = " << userId;

  uint64_t affectedRows = 0;
  if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
    std::cerr << "重命名文件失败: " << fileId << std::endl;
    return false;
  }

  if (affectedRows > 0) {
    emit fileRecordChanged(userId, fileId);
    std::cout << "重命名文件成功: " << fileId << " -> " << newName << std::endl;
  }

  return affectedRows > 0;
}

bool FileMetaMgr::moveFile(uint64_t userId, uint64_t fileId,
                           uint64_t newParentId) {
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return false;
  }

  std::ostringstream oss;
  oss << "UPDATE t_file_info SET parent_id = " << newParentId
      << ", update_time = CURRENT_TIMESTAMP WHERE id = " << fileId
      << " AND user_id = " << userId;

  uint64_t affectedRows = 0;
  if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
    std::cerr << "移动文件失败: " << fileId << std::endl;
    return false;
  }

  if (affectedRows > 0) {
    emit fileRecordChanged(userId, fileId);
    std::cout << "移动文件成功: " << fileId << " -> parent: " << newParentId
              << std::endl;
  }

  return affectedRows > 0;
}

FileMetaInfo FileMetaMgr::getFileInfo(uint64_t fileId) {
  FileMetaInfo info;
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return info;
  }

  std::ostringstream oss;
  oss << "SELECT id, user_id, file_name, file_path, file_size, file_hash, "
         "file_type, parent_id, UNIX_TIMESTAMP(create_time), "
         "UNIX_TIMESTAMP(update_time) "
         "FROM t_file_info WHERE id = "
      << fileId;

  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result)) {
    std::cerr << "查询文件信息失败: " << fileId << std::endl;
    return info;
  }

  if (!result) {
    return info;
  }

  MYSQL_ROW row = mysql_fetch_row(result);
  if (row) {
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
  }

  mysql_free_result(result);
#endif
  return info;
}

bool FileMetaMgr::increaseFileRef(const std::string &fileHash) {
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return false;
  }

  std::ostringstream oss;
  oss << "UPDATE t_file_store SET ref_count = ref_count + 1, "
         "last_access_time = CURRENT_TIMESTAMP WHERE file_hash = '"
      << fileHash << "'";

  uint64_t affectedRows = 0;
  if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
    std::cerr << "增加文件引用计数失败: " << fileHash << std::endl;
    return false;
  }

  std::cout << "增加文件引用计数成功: " << fileHash << std::endl;
  return affectedRows > 0;
}

bool FileMetaMgr::decreaseFileRef(const std::string &fileHash) {
  if (!m_dbManager || !m_dbManager->isConnected()) {
    std::cerr << "数据库管理器未初始化" << std::endl;
    return false;
  }

  std::ostringstream oss;
  oss << "UPDATE t_file_store SET ref_count = ref_count - 1, "
         "last_access_time = CURRENT_TIMESTAMP WHERE file_hash = '"
      << fileHash << "'";

  uint64_t affectedRows = 0;
  if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
    std::cerr << "减少文件引用计数失败: " << fileHash << std::endl;
    return false;
  }

  std::cout << "减少文件引用计数成功: " << fileHash << std::endl;
  return affectedRows > 0;
}

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem