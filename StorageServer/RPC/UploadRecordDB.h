#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include <QObject>

struct sqlite3;

namespace ChatSystem {
namespace StorageServer {
namespace RPC {

/**
 * @brief 本地上传记录结构
 * @details 用于持久化上传完成通知，防止RPC失败导致数据丢失
 */
struct LocalUploadRecord {
    std::string uploadToken;          // 上传令牌（唯一标识）
    uint64_t userId;               // 用户ID
    std::string fileHash;          // 文件哈希
    uint64_t fileSize;             // 文件大小
    std::string fileName;           // 文件名
    uint32_t serverId;            // 存储服务器ID
    std::string serverIp;           // 服务器IP
    uint16_t serverPort;           // 服务器端口
    std::string storagePath;        // 物理存储路径
    uint32_t status;               // 状态：0-待确认，1-已确认，2-确认失败
    uint32_t totalChunks;          // 总分片数
    uint32_t successChunks;        // 成功分片数
    uint32_t failedChunks;         // 失败分片数
    uint64_t uploadTime;           // 上传时间（Unix时间戳）
    uint32_t retryCount;           // 重试次数
    uint64_t lastRetryTime;        // 最后重试时间（Unix时间戳）
    std::string errorMsg;          // 错误信息
};

/**
 * @brief 本地上传记录数据库管理类
 * @details 使用SQLite数据库持久化上传记录，支持RPC失败后的重试
 */
class UploadRecordDB : public QObject {
    Q_OBJECT
public:
    explicit UploadRecordDB(QObject* parent = nullptr);
    ~UploadRecordDB();
    
    /**
     * @brief 初始化数据库
     * @param dbPath 数据库文件路径
     * @return 成功返回true，失败返回false
     */
    bool initialize(const std::string& dbPath);
    
    /**
     * @brief 关闭数据库
     */
    void close();
    
    /**
     * @brief 保存上传记录
     * @param record 上传记录
     * @return 成功返回true，失败返回false
     */
    bool saveUploadRecord(const LocalUploadRecord& record);
    
    /**
     * @brief 更新上传记录状态
     * @param token 上传令牌
     * @param status 新状态
     * @param retryCount 重试次数
     * @param errorMsg 错误信息
     * @return 成功返回true，失败返回false
     */
    bool updateUploadRecordStatus(const std::string& token, uint32_t status, 
                              uint32_t retryCount, const std::string& errorMsg);
    
    /**
     * @brief 获取待重试的上传记录
     * @param retryInterval 重试间隔（秒）
     * @return 待重试的记录列表
     */
    std::vector<LocalUploadRecord> getPendingRetryRecords(uint64_t retryInterval);
    
    /**
     * @brief 删除已确认的记录
     * @param days 保留天数
     * @return 成功返回true，失败返回false
     */
    bool deleteConfirmedRecords(int days);
    
    /**
     * @brief 获取记录数量
     * @param status 状态（0-待确认，1-已确认，2-确认失败），-1表示全部
     * @return 记录数量
     */
    int getRecordCount(int status = -1);
    
private:
    /**
     * @brief 创建数据库表
     * @return 成功返回true，失败返回false
     */
    bool createTables();
    
    /**
     * @brief 从查询结果构建上传记录
     * @param stmt SQLite语句
     * @return 上传记录
     */
    LocalUploadRecord recordFromStatement(sqlite3_stmt* stmt);
    
private:
    sqlite3* m_db;              // SQLite数据库连接
    std::string m_dbPath;        // 数据库文件路径
    bool m_initialized;          // 是否已初始化
    mutable std::mutex m_mutex;   // 互斥锁
};

} // namespace RPC
} // namespace StorageServer
} // namespace ChatSystem
