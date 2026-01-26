#pragma once

#include <string>
#include <map>
#include <mutex>
#include <QObject>
#include <QTimer>

namespace ChatSystem {
namespace StorageServer {
namespace Core {

/**
 * @brief Token验证器类（增强版）
 * @details 支持Token续期、宽限期机制
 */
class TokenValidator : public QObject {
    Q_OBJECT
public:
    explicit TokenValidator(QObject* parent = nullptr);
    ~TokenValidator();
    
    /**
     * @brief 验证上传令牌
     * @param token 上传令牌
     * @param userId 输出用户ID
     * @param fileHash 输出文件哈希
     * @param fileSize 输出文件大小
     * @return 有效返回true，无效返回false
     */
    bool validateUploadToken(const std::string& token, uint64_t& userId, 
                          std::string& fileHash, uint64_t& fileSize);
    
    /**
     * @brief 生成上传令牌
     * @param userId 用户ID
     * @param fileHash 文件哈希
     * @param fileSize 文件大小
     * @return 上传令牌
     */
    std::string generateUploadToken(uint64_t userId, const std::string& fileHash, 
                             uint64_t fileSize);
    
    /**
     * @brief 清理过期令牌
     */
    void cleanupExpiredTokens();
    
    /**
     * @brief 标记Token为活跃
     * @param token 上传令牌
     */
    void markTokenActive(const std::string& token);
    
private slots:
    /**
     * @brief 自动续期槽函数
     */
    void onRenewTimer();
    
    /**
     * @brief 清理过期令牌槽函数
     */
    void onCleanupTimer();
    
private:
    /**
     * @brief 令牌信息结构体（增强版）
     */
    struct TokenInfo {
        uint64_t userId;       // 用户ID
        std::string fileHash;  // 文件哈希
        uint64_t fileSize;    // 文件大小
        uint64_t expireTime;   // 过期时间
        uint64_t gracePeriodEnd;  // 宽限期结束时间（过期后额外30分钟）
        uint32_t renewCount;  // 续期次数
        uint64_t lastRenewTime;  // 最后续期时间
        bool isActive;       // 是否活跃（最近有分片到达）
    };
    
    /**
     * @brief 续期Token
     * @param token 上传令牌
     * @return 成功返回true，失败返回false
     */
    bool renewToken(const std::string& token);
    
private:
    std::map<std::string, TokenInfo> m_tokens;    // 令牌映射
    std::mutex m_mutex;                       // 互斥锁
    QTimer* m_renewTimer;                 // 自动续期定时器
    QTimer* m_cleanupTimer;                // 清理定时器
    
    static const uint32_t DEFAULT_TTL = 3600;           // 默认TTL 1小时
    static const uint32_t GRACE_PERIOD = 1800;         // 宽限期30分钟
    static const uint32_t MAX_RENEW_COUNT = 10;        // 最大续期次数
    static const uint32_t AUTO_RENEW_INTERVAL = 600;   // 自动续期间隔10分钟
};

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem
