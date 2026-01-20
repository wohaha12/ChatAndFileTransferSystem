#include "TokenValidator.h"
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>

namespace ChatSystem {
namespace StorageServer {
namespace Core {

TokenValidator::TokenValidator(QObject* parent)
    : QObject(parent)
    , m_cleanupTimer(nullptr)
{
    m_cleanupTimer = new QTimer(this);
    
    connect(m_cleanupTimer, &QTimer::timeout, this, &TokenValidator::cleanupExpiredTokens);
    
    m_cleanupTimer->start(60000);
}

TokenValidator::~TokenValidator()
{
    delete m_cleanupTimer;
}

bool TokenValidator::validateUploadToken(const std::string& token, uint64_t& userId, 
                                   std::string& fileHash, uint64_t& fileSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_tokens.find(token);
    if (it == m_tokens.end()) {
        std::cerr << "令牌不存在: " << token << std::endl;
        return false;
    }
    
    const TokenInfo& info = it->second;
    
    uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));
    if (currentTime > info.expireTime) {
        std::cerr << "令牌已过期: " << token << std::endl;
        m_tokens.erase(it);
        return false;
    }
    
    userId = info.userId;
    fileHash = info.fileHash;
    fileSize = info.fileSize;
    
    std::cout << "令牌验证成功: " << token << " (用户ID: " << userId << ")" << std::endl;
    return true;
}

std::string TokenValidator::generateUploadToken(uint64_t userId, const std::string& fileHash, 
                                          uint64_t fileSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    unsigned char randomBytes[16];
    if (RAND_bytes(randomBytes, sizeof(randomBytes)) != 1) {
        std::cerr << "生成随机字节失败" << std::endl;
        return "";
    }
    
    std::ostringstream oss;
    for (int i = 0; i < 16; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(randomBytes[i]);
    }
    
    std::string randomStr = oss.str();
    
    std::string input = randomStr + ":" + std::to_string(userId) + ":" + 
                      fileHash + ":" + std::to_string(fileSize);
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    
    std::ostringstream hashOss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hashOss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    std::string token = hashOss.str();
    
    TokenInfo info;
    info.userId = userId;
    info.fileHash = fileHash;
    info.fileSize = fileSize;
    info.expireTime = static_cast<uint64_t>(std::time(nullptr)) + 3600;
    
    m_tokens[token] = info;
    
    std::cout << "生成上传令牌: " << token << " (用户ID: " << userId << ", 文件哈希: " << fileHash << ")" << std::endl;
    return token;
}

void TokenValidator::cleanupExpiredTokens()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));
    
    auto it = m_tokens.begin();
    while (it != m_tokens.end()) {
        if (currentTime > it->second.expireTime) {
            std::cout << "清理过期令牌: " << it->first << std::endl;
            it = m_tokens.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem