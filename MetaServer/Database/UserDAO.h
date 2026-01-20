#pragma once

#include <string>
#include <vector>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

class DatabaseManager;

/**
 * @brief 用户数据访问对象类
 */
class UserDAO {
public:
    /**
     * @brief 构造函数
     * @param dbManager 数据库管理器指针
     */
    explicit UserDAO(DatabaseManager* dbManager);
    
    /**
     * @brief 析构函数
     */
    ~UserDAO();
    
    /**
     * @brief 根据用户名获取用户信息
     * @param username 用户名
     * @param userId 输出用户ID
     * @param passwordHash 输出密码哈希
     * @param salt 输出盐值
     * @param nickname 输出昵称
     * @return 成功返回true，失败返回false
     */
    bool getUserByUsername(const std::string& username, uint64_t& userId, 
                         std::string& passwordHash, std::string& salt, std::string& nickname);
    
    /**
     * @brief 根据用户ID获取用户信息
     * @param userId 用户ID
     * @param username 输出用户名
     * @param nickname 输出昵称
     * @param avatarUrl 输出头像URL
     * @param status 输出状态
     * @return 成功返回true，失败返回false
     */
    bool getUserById(uint64_t userId, std::string& username, std::string& nickname, 
                   std::string& avatarUrl, uint8_t& status);
    
    /**
     * @brief 更新用户状态
     * @param userId 用户ID
     * @param status 状态值：0-离线，1-在线，2-隐身
     * @return 成功返回true，失败返回false
     */
    bool updateUserStatus(uint64_t userId, uint8_t status);
    
    /**
     * @brief 更新用户昵称
     * @param userId 用户ID
     * @param nickname 新昵称
     * @return 成功返回true，失败返回false
     */
    bool updateUserNickname(uint64_t userId, const std::string& nickname);
    
    /**
     * @brief 更新用户头像
     * @param userId 用户ID
     * @param avatarUrl 新头像URL
     * @return 成功返回true，失败返回false
     */
    bool updateUserAvatar(uint64_t userId, const std::string& avatarUrl);
    
    /**
     * @brief 获取在线用户列表
     * @return 在线用户ID列表
     */
    std::vector<uint64_t> getOnlineUsers();
    
private:
    DatabaseManager* m_dbManager;  // 数据库管理器
};

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem
