#include "AuthModule.h"
#include "../Database/UserDAO.h"
#include "../Database/DatabaseManager.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace Core {

AuthModule::AuthModule(QObject* parent)
    : QObject(parent)
    , m_dbManager(nullptr)
{
}

AuthModule::~AuthModule()
{
}

void AuthModule::setDatabaseManager(DatabaseManager* dbManager)
{
    m_dbManager = dbManager;
}

bool AuthModule::authenticate(const std::string& username, const std::string& passwordHash, 
                              uint64_t& userId, std::string& nickname, std::string& sessionToken)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    Database::UserDAO userDAO(m_dbManager);
    
    std::string storedPasswordHash, salt;
    if (!userDAO.getUserByUsername(username, userId, storedPasswordHash, salt, nickname)) {
        std::cerr << "用户不存在: " << username << std::endl;
        return false;
    }
    
    std::string computedHash = computePasswordHash(passwordHash, salt);
    if (computedHash != storedPasswordHash) {
        std::cerr << "密码错误: " << username << std::endl;
        return false;
    }
    
    sessionToken = generateSessionToken(userId);
    
    if (!updateUserStatus(userId, 1)) {
        std::cerr << "更新用户状态失败: " << userId << std::endl;
        return false;
    }
    
    std::cout << "用户登录成功: " << username << " (ID: " << userId << ")" << std::endl;
    return true;
}

bool AuthModule::validateSessionToken(uint64_t userId, const std::string& sessionToken)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    std::string username, nickname, avatarUrl;
    uint8_t status;
    
    Database::UserDAO userDAO(m_dbManager);
    if (!userDAO.getUserById(userId, username, nickname, avatarUrl, status)) {
        std::cerr << "用户不存在: " << userId << std::endl;
        return false;
    }
    
    std::string expectedToken = generateSessionToken(userId);
    return sessionToken == expectedToken;
}

bool AuthModule::logout(uint64_t userId, const std::string& sessionToken)
{
    if (!validateSessionToken(userId, sessionToken)) {
        std::cerr << "会话令牌无效: " << userId << std::endl;
        return false;
    }
    
    if (!updateUserStatus(userId, 0)) {
        std::cerr << "更新用户状态失败: " << userId << std::endl;
        return false;
    }
    
    std::cout << "用户登出成功: " << userId << std::endl;
    return true;
}

bool AuthModule::updateUserStatus(uint64_t userId, uint8_t status)
{
    if (!m_dbManager || !m_dbManager->isConnected()) {
        std::cerr << "数据库管理器未初始化" << std::endl;
        return false;
    }
    
    Database::UserDAO userDAO(m_dbManager);
    return userDAO.updateUserStatus(userId, status);
}

std::string AuthModule::generateSessionToken(uint64_t userId)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    std::string input = std::to_string(userId) + ":" + std::to_string(std::time(nullptr));
    
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return oss.str();
}

std::string AuthModule::computePasswordHash(const std::string& password, const std::string& salt)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    std::string input = password + salt;
    
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return oss.str();
}

std::string AuthModule::generateSalt()
{
    unsigned char salt[16];
    if (RAND_bytes(salt, sizeof(salt)) != 1) {
        std::cerr << "生成盐值失败" << std::endl;
        return "";
    }
    
    std::ostringstream oss;
    for (int i = 0; i < 16; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(salt[i]);
    }
    
    return oss.str();
}

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem