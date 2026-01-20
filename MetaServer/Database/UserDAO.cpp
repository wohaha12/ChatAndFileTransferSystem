#include "UserDAO.h"
#include "DatabaseManager.h"
#include <mysql/mysql.h>
#include <iostream>
#include <sstream>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

UserDAO::UserDAO(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

UserDAO::~UserDAO()
{
}

bool UserDAO::getUserByUsername(const std::string& username, uint64_t& userId, 
                               std::string& passwordHash, std::string& salt, std::string& nickname)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "SELECT id, password_hash, salt, nickname FROM t_user WHERE username = '"
        << username << "'";
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(oss.str(), result)) {
        std::cerr << "查询用户失败: " << username << std::endl;
        return false;
    }
    
    if (!result) {
        std::cerr << "结果集为空" << std::endl;
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        std::cerr << "用户不存在: " << username << std::endl;
        mysql_free_result(result);
        return false;
    }
    
    userId = std::stoull(row[0]);
    passwordHash = row[1] ? row[1] : "";
    salt = row[2] ? row[2] : "";
    nickname = row[3] ? row[3] : "";
    
    mysql_free_result(result);
    return true;
}

bool UserDAO::getUserById(uint64_t userId, std::string& username, std::string& nickname, 
                         std::string& avatarUrl, uint8_t& status)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "SELECT username, nickname, avatar_url, status FROM t_user WHERE id = " << userId;
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(oss.str(), result)) {
        std::cerr << "查询用户失败: " << userId << std::endl;
        return false;
    }
    
    if (!result) {
        std::cerr << "结果集为空" << std::endl;
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        std::cerr << "用户不存在: " << userId << std::endl;
        mysql_free_result(result);
        return false;
    }
    
    username = row[0] ? row[0] : "";
    nickname = row[1] ? row[1] : "";
    avatarUrl = row[2] ? row[2] : "";
    status = row[3] ? static_cast<uint8_t>(std::stoi(row[3])) : 0;
    
    mysql_free_result(result);
    return true;
}

bool UserDAO::updateUserStatus(uint64_t userId, uint8_t status)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "UPDATE t_user SET status = " << static_cast<int>(status)
        << ", update_time = CURRENT_TIMESTAMP WHERE id = " << userId;
    
    uint64_t affectedRows = 0;
    if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
        std::cerr << "更新用户状态失败: " << userId << std::endl;
        return false;
    }
    
    return affectedRows > 0;
}

bool UserDAO::updateUserNickname(uint64_t userId, const std::string& nickname)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "UPDATE t_user SET nickname = '" << nickname
        << "', update_time = CURRENT_TIMESTAMP WHERE id = " << userId;
    
    uint64_t affectedRows = 0;
    if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
        std::cerr << "更新用户昵称失败: " << userId << std::endl;
        return false;
    }
    
    return affectedRows > 0;
}

bool UserDAO::updateUserAvatar(uint64_t userId, const std::string& avatarUrl)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::ostringstream oss;
    oss << "UPDATE t_user SET avatar_url = '" << avatarUrl
        << "', update_time = CURRENT_TIMESTAMP WHERE id = " << userId;
    
    uint64_t affectedRows = 0;
    if (!m_dbManager->executeUpdate(oss.str(), affectedRows)) {
        std::cerr << "更新用户头像失败: " << userId << std::endl;
        return false;
    }
    
    return affectedRows > 0;
}

std::vector<uint64_t> UserDAO::getOnlineUsers()
{
    std::vector<uint64_t> onlineUsers;
    
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return onlineUsers;
    }
    
    std::string sql = "SELECT id FROM t_user WHERE status = 1";
    
    MYSQL_RES* result = nullptr;
    if (!m_dbManager->executeQuery(sql, result)) {
        std::cerr << "查询在线用户失败" << std::endl;
        return onlineUsers;
    }
    
    if (!result) {
        return onlineUsers;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0]) {
            onlineUsers.push_back(std::stoull(row[0]));
        }
    }
    
    mysql_free_result(result);
    return onlineUsers;
}

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem