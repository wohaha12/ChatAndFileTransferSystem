#pragma once

#include <string>
#include <memory>
#include <QObject>

namespace ChatSystem {
namespace MetaServer {
namespace Core {

class DatabaseManager;

/**
 * @brief 认证模块类
 */
class AuthModule : public QObject {
    Q_OBJECT
public:
    explicit AuthModule(QObject* parent = nullptr);
    ~AuthModule();
    
    /**
     * @brief 设置数据库管理器
     * @param dbManager 数据库管理器指针
     */
    void setDatabaseManager(DatabaseManager* dbManager);
    
    /**
     * @brief 用户登录认证
     * @param username 用户名
     * @param passwordHash 密码哈希
     * @param userId 输出用户ID
     * @param nickname 输出昵称
     * @param sessionToken 输出会话令牌
     * @return 成功返回true，失败返回false
     */
    bool authenticate(const std::string& username, const std::string& passwordHash, 
                     uint64_t& userId, std::string& nickname, std::string& sessionToken);
    
    /**
     * @brief 验证会话令牌
     * @param userId 用户ID
     * @param sessionToken 会话令牌
     * @return 有效返回true，无效返回false
     */
    bool validateSessionToken(uint64_t userId, const std::string& sessionToken);
    
    /**
     * @brief 用户登出
     * @param userId 用户ID
     * @param sessionToken 会话令牌
     * @return 成功返回true，失败返回false
     */
    bool logout(uint64_t userId, const std::string& sessionToken);
    
    /**
     * @brief 更新用户状态
     * @param userId 用户ID
     * @param status 状态值：0-离线，1-在线，2-隐身
     * @return 成功返回true，失败返回false
     */
    bool updateUserStatus(uint64_t userId, uint8_t status);
    
private:
    /**
     * @brief 生成会话令牌
     * @param userId 用户ID
     * @return 会话令牌
     */
    std::string generateSessionToken(uint64_t userId);
    
    /**
     * @brief 计算密码哈希
     * @param password 密码
     * @param salt 盐值
     * @return 密码哈希
     */
    std::string computePasswordHash(const std::string& password, const std::string& salt);
    
    /**
     * @brief 生成盐值
     * @return 盐值
     */
    std::string generateSalt();
    
private:
    DatabaseManager* m_dbManager;  // 数据库管理器
};

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem
