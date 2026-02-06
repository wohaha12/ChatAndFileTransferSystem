#pragma once

#include <QObject>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace ChatSystem {
namespace MetaServer {

namespace Database {
class DatabaseManager;
}

namespace Cache {
class RedisManager;
}

namespace Core {

struct FileMetaInfo; // Forward declaration for use in other namespaces

using Cache::RedisManager;
using Database::DatabaseManager;

/**
 * @brief 文件元数据信息结构体
 */
struct FileMetaInfo {
  uint64_t fileId;      // 文件ID
  uint64_t userId;      // 用户ID
  std::string fileName; // 文件名
  std::string filePath; // 文件路径
  uint64_t fileSize;    // 文件大小
  std::string fileHash; // 文件哈希
  uint8_t fileType;     // 文件类型：0-文件，1-目录
  uint64_t parentId;    // 父目录ID，0表示根目录
  uint64_t createTime;  // 创建时间
  uint64_t updateTime;  // 更新时间
};

/**
 * @brief 文件元数据管理器类
 */
class FileMetaMgr : public QObject {
  Q_OBJECT
public:
  explicit FileMetaMgr(QObject *parent = nullptr);
  ~FileMetaMgr();

  /**
   * @brief 设置数据库管理器
   * @param dbManager 数据库管理器指针
   */
  void setDatabaseManager(DatabaseManager *dbManager);

  /**
   * @brief 设置Redis管理器
   * @param redisManager Redis管理器指针
   */
  void setRedisManager(RedisManager *redisManager);

  /**
   * @brief 检查文件是否已存在(秒传)
   * @param fileHash 文件哈希
   * @param fileSize 文件大小
   * @param storagePath 输出存储路径
   * @return 存在返回true，不存在返回false
   */
  bool checkFileExists(const std::string &fileHash, uint64_t fileSize,
                       std::string &storagePath);

  /**
   * @brief 创建文件记录
   * @param userId 用户ID
   * @param fileName 文件名
   * @param filePath 文件路径
   * @param fileSize 文件大小
   * @param fileHash 文件哈希
   * @param parentId 父目录ID，默认0
   * @return 文件ID，失败返回0
   */
  uint64_t createFileRecord(uint64_t userId, const std::string &fileName,
                            const std::string &filePath, uint64_t fileSize,
                            const std::string &fileHash, uint64_t parentId = 0);

  /**
   * @brief 获取用户文件列表
   * @param userId 用户ID
   * @param parentId 父目录ID，默认0
   * @return 文件元数据信息列表
   */
  std::vector<FileMetaInfo> getUserFiles(uint64_t userId,
                                         uint64_t parentId = 0);

  /**
   * @brief 删除文件记录
   * @param userId 用户ID
   * @param fileId 文件ID
   * @return 成功返回true，失败返回false
   */
  bool deleteFileRecord(uint64_t userId, uint64_t fileId);

  /**
   * @brief 重命名文件
   * @param userId 用户ID
   * @param fileId 文件ID
   * @param newName 新文件名
   * @return 成功返回true，失败返回false
   */
  bool renameFile(uint64_t userId, uint64_t fileId, const std::string &newName);

  /**
   * @brief 移动文件
   * @param userId 用户ID
   * @param fileId 文件ID
   * @param newParentId 新父目录ID
   * @return 成功返回true，失败返回false
   */
  bool moveFile(uint64_t userId, uint64_t fileId, uint64_t newParentId);

  /**
   * @brief 获取文件信息
   * @param fileId 文件ID
   * @return 文件元数据信息
   */
  FileMetaInfo getFileInfo(uint64_t fileId);

  /**
   * @brief 增加文件引用计数
   * @param fileHash 文件哈希
   * @return 成功返回true，失败返回false
   */
  bool increaseFileRef(const std::string &fileHash);

  /**
   * @brief 减少文件引用计数
   * @param fileHash 文件哈希
   * @return 成功返回true，失败返回false
   */
  bool decreaseFileRef(const std::string &fileHash);

signals:
  /**
   * @brief 文件记录变化信号
   * @param userId 用户ID
   * @param fileId 文件ID
   */
  void fileRecordChanged(uint64_t userId, uint64_t fileId);

private:
  DatabaseManager *m_dbManager; // 数据库管理器
  RedisManager *m_redisManager; // Redis管理器
  mutable std::mutex m_mutex;   // 互斥锁
};

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem
