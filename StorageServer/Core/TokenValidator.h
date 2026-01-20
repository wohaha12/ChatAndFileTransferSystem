#pragma once

#include <string>
#include <map>
#include <mutex>
#include <QObject>

namespace ChatSystem {
namespace StorageServer {
namespace Core {

/**
 * @brief Token验证器类
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
    
private:
    /**
     * @brief 令牌信息结构体
     */
    struct TokenInfo {
        uint64_t userId;       // 用户ID
        std::string fileHash;  // 文件哈希
        uint64_t fileSize;    // 文件大小
        uint64_t expireTime;   // 过期时间
    };
    
private:
    std::map<std::string, TokenInfo> m_tokens;    // 令牌映射
    std::mutex m_mutex;                       // 互斥锁
    QTimer* m_cleanupTimer;                // 清理定时器
};

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem
