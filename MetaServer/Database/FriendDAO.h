#pragma once

#include <string>
#include <vector>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

class DatabaseManager;

/**
 * @brief 好友数据访问对象类
 * @details 管理用户好友关系的数据库操作
 */
class FriendDAO {
public:
    /**
     * @brief 构造函数
     * @param dbManager 数据库管理器指针
     */
    explicit FriendDAO(DatabaseManager* dbManager);
    
    /**
     * @brief 析构函数
     */
    ~FriendDAO();
    
    /**
     * @brief 添加好友关系
     * @param userId 用户ID
     * @param friendId 好友ID
     * @return 成功返回true，失败返回false
     */
    bool addFriend(uint64_t userId, uint64_t friendId);
    
    /**
     * @brief 删除好友关系
     * @param userId 用户ID
     * @param friendId 好友ID
     * @return 成功返回true，失败返回false
     */
    bool deleteFriend(uint64_t userId, uint64_t friendId);
    
    /**
     * @brief 检查是否为好友
     * @param userId 用户ID
     * @param friendId 好友ID
     * @return 是好友返回true，不是好友返回false
     */
    bool isFriend(uint64_t userId, uint64_t friendId);
    
    /**
     * @brief 获取用户的好友列表
     * @param userId 用户ID
     * @return 好友ID列表
     */
    std::vector<uint64_t> getFriends(uint64_t userId);
    
    /**
     * @brief 获取用户的好友列表（仅在线）
     * @param userId 用户ID
     * @return 在线好友ID列表
     */
    std::vector<uint64_t> getOnlineFriends(uint64_t userId);
    
    /**
     * @brief 获取好友的用户名列表
     * @param userId 用户ID
     * @return 好友用户名列表
     */
    std::vector<std::string> getFriendNames(uint64_t userId);
    
    /**
     * @brief 获取在线好友的用户名列表
     * @param userId 用户ID
     * @return 在线好友用户名列表
     */
    std::vector<std::string> getOnlineFriendNames(uint64_t userId);
    
private:
    DatabaseManager* m_dbManager;  // 数据库管理器
};

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem