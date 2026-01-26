#include "NotificationTracker.h"
#include <ctime>
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace RPC {

NotificationTracker::NotificationTracker(QObject* parent)
    : QObject(parent)
    , m_cleanupTimer(nullptr)
{
    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &NotificationTracker::onCleanupTimer);
    
    m_cleanupTimer->start(3600000);  // 每小时清理一次
    std::cout << "通知追踪器初始化成功，清理定时器已启动（间隔: 1小时）" << std::endl;
}

NotificationTracker::~NotificationTracker()
{
    if (m_cleanupTimer) {
        m_cleanupTimer->stop();
        delete m_cleanupTimer;
    }
}

bool NotificationTracker::isProcessed(const std::string& token)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_processedNotifications.find(token);
    if (it != m_processedNotifications.end()) {
        std::cout << "通知已处理: " << token << std::endl;
        return true;
    }
    
    return false;
}

void NotificationTracker::markProcessed(const std::string& token)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    NotificationRecord record;
    record.timestamp = static_cast<uint64_t>(std::time(nullptr));
    
    m_processedNotifications[token] = record;
    
    std::cout << "记录已处理通知: " << token << " (总数: " 
              << m_processedNotifications.size() << ")" << std::endl;
}

void NotificationTracker::cleanupExpiredNotifications(uint64_t expireSeconds)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_processedNotifications.empty()) {
        return;
    }
    
    uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));
    uint64_t expireTime = currentTime - expireSeconds;
    
    size_t beforeCount = m_processedNotifications.size();
    
    auto it = m_processedNotifications.begin();
    while (it != m_processedNotifications.end()) {
        if (it->second.timestamp < expireTime) {
            std::cout << "清理过期通知: " << it->first << std::endl;
            it = m_processedNotifications.erase(it);
        } else {
            ++it;
        }
    }
    
    size_t afterCount = m_processedNotifications.size();
    std::cout << "清理过期通知完成: 清理 " << (beforeCount - afterCount) 
              << " 条，剩余 " << afterCount << " 条" << std::endl;
}

void NotificationTracker::onCleanupTimer()
{
    cleanupExpiredNotifications(3600);  // 清理1小时前的记录
}

size_t NotificationTracker::getProcessedCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_processedNotifications.size();
}

} // namespace RPC
} // namespace MetaServer
} // namespace ChatSystem
