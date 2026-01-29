#include "UploadRecordDB.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <iostream>
#include <ctime>

namespace ChatSystem {
namespace StorageServer {
namespace RPC {

UploadRecordDB::UploadRecordDB(QObject* parent)
    : QObject(parent)
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

    m_dbPath = QString::fromStdString(dbPath);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        std::cerr << "打开数据库失败: " << m_db.lastError().text().toStdString() << std::endl;
        return false;
    }

    if (!createTables()) {
        std::cerr << "创建数据库表失败" << std::endl;
        m_db.close();
        m_initialized = false;
        return false;
    }

    m_initialized = true;
    std::cout << "数据库初始化成功: " << dbPath << std::endl;
    return true;
}

void UploadRecordDB::close()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_db.isOpen()) {
        m_db.close();
    }

    m_initialized = false;
}

bool UploadRecordDB::createTables()
{
    QSqlQuery query(m_db);

    QString sql = R"(
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

    if (!query.exec(sql)) {
        std::cerr << "创建表失败: " << query.lastError().text().toStdString() << std::endl;
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

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT OR REPLACE INTO upload_records (
            upload_token, user_id, file_hash, file_size, file_name,
            server_id, server_ip, server_port, storage_path,
            status, total_chunks, success_chunks, failed_chunks,
            upload_time, retry_count, last_retry_time, error_msg
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )");

    query.addBindValue(QString::fromStdString(record.uploadToken));
    query.addBindValue(static_cast<qint64>(record.userId));
    query.addBindValue(QString::fromStdString(record.fileHash));
    query.addBindValue(static_cast<qint64>(record.fileSize));
    query.addBindValue(QString::fromStdString(record.fileName));
    query.addBindValue(record.serverId);
    query.addBindValue(QString::fromStdString(record.serverIp));
    query.addBindValue(record.serverPort);
    query.addBindValue(QString::fromStdString(record.storagePath));
    query.addBindValue(record.status);
    query.addBindValue(record.totalChunks);
    query.addBindValue(record.successChunks);
    query.addBindValue(record.failedChunks);
    query.addBindValue(static_cast<qint64>(record.uploadTime));
    query.addBindValue(record.retryCount);
    query.addBindValue(static_cast<qint64>(record.lastRetryTime));
    query.addBindValue(QString::fromStdString(record.errorMsg));

    if (!query.exec()) {
        std::cerr << "执行SQL语句失败: " << query.lastError().text().toStdString() << std::endl;
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

    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE upload_records
        SET status = ?, retry_count = ?, last_retry_time = ?, error_msg = ?
        WHERE upload_token = ?;
    )");

    uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));

    query.addBindValue(status);
    query.addBindValue(retryCount);
    query.addBindValue(static_cast<qint64>(currentTime));
    query.addBindValue(QString::fromStdString(errorMsg));
    query.addBindValue(QString::fromStdString(token));

    if (!query.exec()) {
        std::cerr << "执行SQL语句失败: " << query.lastError().text().toStdString() << std::endl;
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

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT upload_token, user_id, file_hash, file_size, file_name,
               server_id, server_ip, server_port, storage_path,
               status, total_chunks, success_chunks, failed_chunks,
               upload_time, retry_count, last_retry_time, error_msg
        FROM upload_records
        WHERE status = 0
          AND (last_retry_time = 0 OR last_retry_time < ?)
        ORDER BY upload_time ASC
        LIMIT 100;
    )");

    query.addBindValue(static_cast<qint64>(minRetryTime));

    if (!query.exec()) {
        std::cerr << "准备SQL语句失败: " << query.lastError().text().toStdString() << std::endl;
        return records;
    }

    while (query.next()) {
        LocalUploadRecord record;
        record.uploadToken = query.value(0).toString().toStdString();
        record.userId = static_cast<uint64_t>(query.value(1).toLongLong());
        record.fileHash = query.value(2).toString().toStdString();
        record.fileSize = static_cast<uint64_t>(query.value(3).toLongLong());
        record.fileName = query.value(4).toString().toStdString();
        record.serverId = static_cast<uint32_t>(query.value(5).toInt());
        record.serverIp = query.value(6).toString().toStdString();
        record.serverPort = static_cast<uint16_t>(query.value(7).toInt());
        record.storagePath = query.value(8).toString().toStdString();
        record.status = static_cast<uint32_t>(query.value(9).toInt());
        record.totalChunks = static_cast<uint32_t>(query.value(10).toInt());
        record.successChunks = static_cast<uint32_t>(query.value(11).toInt());
        record.failedChunks = static_cast<uint32_t>(query.value(12).toInt());
        record.uploadTime = static_cast<uint64_t>(query.value(13).toLongLong());
        record.retryCount = static_cast<uint32_t>(query.value(14).toInt());
        record.lastRetryTime = static_cast<uint64_t>(query.value(15).toLongLong());
        record.errorMsg = query.value(16).toString().toStdString();
        records.push_back(record);
    }

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

    QSqlQuery query(m_db);
    query.prepare(R"(
        DELETE FROM upload_records
        WHERE status = 1 AND upload_time < ?;
    )");

    query.addBindValue(static_cast<qint64>(expireTime));

    if (!query.exec()) {
        std::cerr << "执行SQL语句失败: " << query.lastError().text().toStdString() << std::endl;
        return false;
    }

    int deletedCount = query.numRowsAffected();
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

    QSqlQuery query(m_db);

    if (status == -1) {
        if (!query.exec("SELECT COUNT(*) FROM upload_records;")) {
            std::cerr << "准备SQL语句失败: " << query.lastError().text().toStdString() << std::endl;
            return 0;
        }
    } else {
        query.prepare("SELECT COUNT(*) FROM upload_records WHERE status = ?;");
        query.addBindValue(status);
        if (!query.exec()) {
            std::cerr << "准备SQL语句失败: " << query.lastError().text().toStdString() << std::endl;
            return 0;
        }
    }

    if (!query.next()) {
        std::cerr << "执行SQL语句失败: " << query.lastError().text().toStdString() << std::endl;
        return 0;
    }

    return query.value(0).toInt();
}

} // namespace RPC
} // namespace StorageServer
} // namespace ChatSystem
