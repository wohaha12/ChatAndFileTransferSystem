#include "UploadRecordDB.h"
#include <sqlite3.h>
#include <iostream>
#include <ctime>

namespace ChatSystem {
namespace StorageServer {
namespace RPC {

UploadRecordDB::UploadRecordDB(QObject* parent)
    : QObject(parent)
    , m_db(nullptr)
    , m_initialized(false)
{
}

UploadRecordDB::~UploadRecordDB()
{
    close();
}

bool UploadRecordDB::initialize(const std::string& dbPath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_initialized) {
        std::cerr << "数据库已初始化" << std::endl;
        return true;
    }
    
    m_dbPath = dbPath;
    
    int result = sqlite3_open(dbPath.c_str(), &m_db);
    if (result != SQLITE_OK) {
        std::cerr << "打开数据库失败: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    if (!createTables()) {
        std::cerr << "创建数据库表失败" << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
        return false;
    }
    
    m_initialized = true;
    std::cout << "数据库初始化成功: " << dbPath << std::endl;
    return true;
}

void UploadRecordDB::close()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
    
    m_initialized = false;
}

bool UploadRecordDB::createTables()
{
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS upload_records (
            upload_token TEXT PRIMARY KEY,
            user_id INTEGER NOT NULL,
            file_hash TEXT NOT NULL,
            file_size INTEGER NOT NULL,
            file_name TEXT NOT NULL,
            server_id INTEGER NOT NULL,
            server_ip TEXT NOT NULL,
            server_port INTEGER NOT NULL,
            storage_path TEXT NOT NULL,
            status INTEGER NOT NULL DEFAULT 0,
            total_chunks INTEGER NOT NULL DEFAULT 0,
            success_chunks INTEGER NOT NULL DEFAULT 0,
            failed_chunks INTEGER NOT NULL DEFAULT 0,
            upload_time INTEGER NOT NULL,
            retry_count INTEGER NOT NULL DEFAULT 0,
            last_retry_time INTEGER DEFAULT 0,
            error_msg TEXT DEFAULT ''
        );
        
        CREATE INDEX IF NOT EXISTS idx_status ON upload_records(status);
        CREATE INDEX IF NOT EXISTS idx_upload_time ON upload_records(upload_time);
        CREATE INDEX IF NOT EXISTS idx_last_retry ON upload_records(last_retry_time);
    )";
    
    char* errorMsg = nullptr;
    int result = sqlite3_exec(m_db, sql, nullptr, nullptr, &errorMsg);
    
    if (result != SQLITE_OK) {
        std::cerr << "创建表失败: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
        return false;
    }
    
    return true;
}

bool UploadRecordDB::saveUploadRecord(const LocalUploadRecord& record)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        std::cerr << "数据库未初始化" << std::endl;
        return false;
    }
    
    const char* sql = R"(
        INSERT OR REPLACE INTO upload_records (
            upload_token, user_id, file_hash, file_size, file_name,
            server_id, server_ip, server_port, storage_path,
            status, total_chunks, success_chunks, failed_chunks,
            upload_time, retry_count, last_retry_time, error_msg
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "准备SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, record.uploadToken.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(record.userId));
    sqlite3_bind_text(stmt, 3, record.fileHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 4, static_cast<sqlite3_int64>(record.fileSize));
    sqlite3_bind_text(stmt, 5, record.fileName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, record.serverId);
    sqlite3_bind_text(stmt, 7, record.serverIp.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, record.serverPort);
    sqlite3_bind_text(stmt, 9, record.storagePath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 10, record.status);
    sqlite3_bind_int(stmt, 11, record.totalChunks);
    sqlite3_bind_int(stmt, 12, record.successChunks);
    sqlite3_bind_int(stmt, 13, record.failedChunks);
    sqlite3_bind_int64(stmt, 14, static_cast<sqlite3_int64>(record.uploadTime));
    sqlite3_bind_int(stmt, 15, record.retryCount);
    sqlite3_bind_int64(stmt, 16, static_cast<sqlite3_int64>(record.lastRetryTime));
    sqlite3_bind_text(stmt, 17, record.errorMsg.c_str(), -1, SQLITE_STATIC);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        std::cerr << "执行SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    std::cout << "保存上传记录成功: " << record.uploadToken << std::endl;
    return true;
}

bool UploadRecordDB::updateUploadRecordStatus(const std::string& token, uint32_t status, 
                                        uint32_t retryCount, const std::string& errorMsg)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        std::cerr << "数据库未初始化" << std::endl;
        return false;
    }
    
    const char* sql = R"(
        UPDATE upload_records 
        SET status = ?, retry_count = ?, last_retry_time = ?, error_msg = ?
        WHERE upload_token = ?;
    )";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "准备SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));
    
    sqlite3_bind_int(stmt, 1, status);
    sqlite3_bind_int(stmt, 2, retryCount);
    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(currentTime));
    sqlite3_bind_text(stmt, 4, errorMsg.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, token.c_str(), -1, SQLITE_STATIC);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        std::cerr << "执行SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    std::cout << "更新上传记录状态成功: " << token << " (状态: " << status << ")" << std::endl;
    return true;
}

std::vector<LocalUploadRecord> UploadRecordDB::getPendingRetryRecords(uint64_t retryInterval)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<LocalUploadRecord> records;
    
    if (!m_initialized) {
        std::cerr << "数据库未初始化" << std::endl;
        return records;
    }
    
    uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));
    uint64_t minRetryTime = currentTime - retryInterval;
    
    const char* sql = R"(
        SELECT upload_token, user_id, file_hash, file_size, file_name,
               server_id, server_ip, server_port, storage_path,
               status, total_chunks, success_chunks, failed_chunks,
               upload_time, retry_count, last_retry_time, error_msg
        FROM upload_records
        WHERE status = 0 
          AND (last_retry_time = 0 OR last_retry_time < ?)
        ORDER BY upload_time ASC
        LIMIT 100;
    )";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "准备SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return records;
    }
    
    sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(minRetryTime));
    
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        LocalUploadRecord record = recordFromStatement(stmt);
        records.push_back(record);
    }
    
    sqlite3_finalize(stmt);
    
    std::cout << "获取待重试记录数量: " << records.size() << std::endl;
    return records;
}

bool UploadRecordDB::deleteConfirmedRecords(int days)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        std::cerr << "数据库未初始化" << std::endl;
        return false;
    }
    
    uint64_t expireTime = static_cast<uint64_t>(std::time(nullptr)) - (days * 86400);
    
    const char* sql = R"(
        DELETE FROM upload_records
        WHERE status = 1 AND upload_time < ?;
    )";
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "准备SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(expireTime));
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        std::cerr << "执行SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    
    int deletedCount = sqlite3_changes(m_db);
    std::cout << "删除已确认记录数量: " << deletedCount << std::endl;
    return true;
}

int UploadRecordDB::getRecordCount(int status)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_initialized) {
        std::cerr << "数据库未初始化" << std::endl;
        return 0;
    }
    
    const char* sql;
    if (status == -1) {
        sql = "SELECT COUNT(*) FROM upload_records;";
    } else {
        sql = "SELECT COUNT(*) FROM upload_records WHERE status = ?;";
    }
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "准备SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        return 0;
    }
    
    if (status != -1) {
        sqlite3_bind_int(stmt, 1, status);
    }
    
    result = sqlite3_step(stmt);
    if (result != SQLITE_ROW) {
        std::cerr << "执行SQL语句失败: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return 0;
    }
    
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    
    return count;
}

LocalUploadRecord UploadRecordDB::recordFromStatement(sqlite3_stmt* stmt)
{
    LocalUploadRecord record;
    
    record.uploadToken = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    record.userId = static_cast<uint64_t>(sqlite3_column_int64(stmt, 1));
    record.fileHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    record.fileSize = static_cast<uint64_t>(sqlite3_column_int64(stmt, 3));
    record.fileName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    record.serverId = static_cast<uint32_t>(sqlite3_column_int(stmt, 5));
    record.serverIp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    record.serverPort = static_cast<uint16_t>(sqlite3_column_int(stmt, 7));
    record.storagePath = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
    record.status = static_cast<uint32_t>(sqlite3_column_int(stmt, 9));
    record.totalChunks = static_cast<uint32_t>(sqlite3_column_int(stmt, 10));
    record.successChunks = static_cast<uint32_t>(sqlite3_column_int(stmt, 11));
    record.failedChunks = static_cast<uint32_t>(sqlite3_column_int(stmt, 12));
    record.uploadTime = static_cast<uint64_t>(sqlite3_column_int64(stmt, 13));
    record.retryCount = static_cast<uint32_t>(sqlite3_column_int(stmt, 14));
    record.lastRetryTime = static_cast<uint64_t>(sqlite3_column_int64(stmt, 15));
    
    const char* errorMsg = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 16));
    record.errorMsg = errorMsg ? errorMsg : "";
    
    return record;
}

} // namespace RPC
} // namespace StorageServer
} // namespace ChatSystem
