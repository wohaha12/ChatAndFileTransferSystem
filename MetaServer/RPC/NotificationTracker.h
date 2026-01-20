#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <cstdint>
#include <QObject>
#include <QTimer>

namespace ChatSystem {
namespace MetaServer {
namespace RPC {

/**
 * @brief 通知追踪器类
 * @details 用于记录已处理的RPC通知，实现幂等性
 */
class NotificationTracker : public QObject {
    Q_OBJECT
public:
    explicit NotificationTracker(QObject* parent = nullptr);
    ~NotificationTracker();
    
    /**
     * @brief 检查通知是否已处理
     * @param token 上传令牌（唯一标识）
     * @return 已处理返回true，未处理返回false
     */
    bool isProcessed(const std::string& token);
    
    /**
     * @brief 记录已处理的通知
     * @param token 上传令牌（唯一标识）
     */
    void markProcessed(const std::string& token);
    
    /**
     * @brief 清理过期的通知记录
     * @param expireSeconds 过期时间（秒），默认3600秒（1小时）
     */
    void cleanupExpiredNotifications(uint64_t expireSeconds = 3600);
    
    /**
     * @brief 获取已处理通知数量
     * @return 已处理通知数量
     */
    size_t getProcessedCount() const;
    
private slots:
    /**
     * @brief 定时清理槽函数
     */
    void onCleanupTimer();
    
private:
    struct NotificationRecord {
        uint64_t timestamp;  // 处理时间戳
    };
    
private:
    std::unordered_map<std::string, NotificationRecord> m_processedNotifications;  // 已处理的通知
    mutable std::mutex m_mutex;   // 互斥锁
    QTimer* m_cleanupTimer;       // 清理定时器
};

} // namespace RPC
} // namespace MetaServer
} // namespace ChatSystem
