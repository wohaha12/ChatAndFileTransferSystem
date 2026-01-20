#include "FriendDAO.h"
#include "DatabaseManager.h"
#include <mysql/mysql.h>
#include <iostream>
#include <sstream>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

FriendDAO::FriendDAO(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

FriendDAO::~FriendDAO()
{
}

bool FriendDAO::addFriend(uint64_t userId, uint64_t friendId)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    if (isFriend(userId, friendId)) {
        std::cerr << "已经是好友关系: " << userId << " <-> " << friendId << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "INSERT INTO t_friend (user_id, friend_id) VALUES ("
        << userId << ", " << friendId << ")";
    
    uint64_t affectedRows = 0;
    if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
        std::cerr << "添加好友失败: " << userId << " <-> " << friendId << std::endl;
        return false;
    }
    
    return affectedRows > 0;
}

bool FriendDAO::deleteFriend(uint64_t userId, uint64_t friendId)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "DELETE FROM t_friend WHERE user_id = " << userId
        << " AND friend_id = " << friendId;
    
    uint64_t affectedRows = 0;
    if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
        std::cerr << "删除好友失败: " << userId << " <-> " << friendId << std::endl;
        return false;
    }
    
    return affectedRows > 0;
}

bool FriendDAO::isFriend(uint64_t userId, uint64_t friendId)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "SELECT COUNT(*) FROM t_friend WHERE user_id = " << userId
        << " AND friend_id = " << friendId;
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(oss.str(), result)) {
        std::cerr << "查询好友关系失败: " << userId << " <-> " << friendId << std::endl;
        return false;
    }
    
    if (!result) {
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    bool isFriendRelation = false;
    if (row && row[0]) {
        isFriendRelation = std::stoi(row[0]) > 0;
    }
    
    mysql_free_result(result);
    return isFriendRelation;
}

std::vector<uint64_t> FriendDAO::getFriends(uint64_t userId)
{
    std::vector<uint64_t> friends;
    
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return friends;
    }
    
    std::ostringstream oss;
    oss << "SELECT friend_id FROM t_friend WHERE user_id = " << userId;
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(oss.str(), result)) {
        std::cerr << "查询好友列表失败: " << userId << std::endl;
        return friends;
    }
    
    if (!result) {
        return friends;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0]) {
            friends.push_back(std::stoull(row[0]));
        }
    }
    
    mysql_free_result(result);
    return friends;
}

std::vector<uint64_t> FriendDAO::getOnlineFriends(uint64_t userId)
{
    std::vector<uint64_t> onlineFriends;
    
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return onlineFriends;
    }
    
    std::ostringstream oss;
    oss << "SELECT f.friend_id FROM t_friend f "
           "INNER JOIN t_user u ON f.friend_id = u.id "
           "WHERE f.user_id = " << userId << " AND u.status = 1";
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(oss.str(), result)) {
        std::cerr << "查询在线好友列表失败: " << userId << std::endl;
        return onlineFriends;
    }
    
    if (!result) {
        return onlineFriends;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0]) {
            onlineFriends.push_back(std::stoull(row[0]));
        }
    }
    
    mysql_free_result(result);
    return onlineFriends;
}

std::vector<std::string> FriendDAO::getFriendNames(uint64_t userId)
{
    std::vector<std::string> friendNames;
    
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return friendNames;
    }
    
    std::ostringstream oss;
    oss << "SELECT u.username FROM t_friend f "
           "INNER JOIN t_user u ON f.friend_id = u.id "
           "WHERE f.user_id = " << userId;
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(oss.str(), result)) {
        std::cerr << "查询好友名称列表失败: " << userId << std::endl;
        return friendNames;
    }
    
    if (!result) {
        return friendNames;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0]) {
            friendNames.push_back(row[0]);
        }
    }
    
    mysql_free_result(result);
    return friendNames;
}

std::vector<std::string> FriendDAO::getOnlineFriendNames(uint64_t userId)
{
    std::vector<std::string> onlineFriendNames;
    
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return onlineFriendNames;
    }
    
    std::ostringstream oss;
    oss << "SELECT u.username FROM t_friend f "
           "INNER JOIN t_user u ON f.friend_id = u.id "
           "WHERE f.user_id = " << userId << " AND u.status = 1";
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(oss.str(), result)) {
        std::cerr << "查询在线好友名称列表失败: " << userId << std::endl;
        return onlineFriendNames;
    }
    
    if (!result) {
        return onlineFriendNames;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0]) {
            onlineFriendNames.push_back(row[0]);
        }
    }
    
    mysql_free_result(result);
    return onlineFriendNames;
}

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem