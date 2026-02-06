#include "TokenValidator.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ChatSystem {
namespace StorageServer {
namespace Core {

TokenValidator::TokenValidator(QObject *parent)
    : QObject(parent), m_renewTimer(nullptr), m_cleanupTimer(nullptr) {
  m_renewTimer = new QTimer(this);
  connect(m_renewTimer, &QTimer::timeout, this, &TokenValidator::onRenewTimer);

  m_cleanupTimer = new QTimer(this);
  connect(m_cleanupTimer, &QTimer::timeout, this,
          &TokenValidator::onCleanupTimer);

  m_renewTimer->start(AUTO_RENEW_INTERVAL * 1000); // 10分钟
  m_cleanupTimer->start(60000);                    // 1分钟

  std::cout << "Token验证器初始化成功，自动续期定时器已启动（间隔: "
            << AUTO_RENEW_INTERVAL << "秒）" << std::endl;
}

TokenValidator::~TokenValidator() {
  if (m_renewTimer) {
    m_renewTimer->stop();
    delete m_renewTimer;
  }

  if (m_cleanupTimer) {
    m_cleanupTimer->stop();
    delete m_cleanupTimer;
  }
}

bool TokenValidator::validateUploadToken(const std::string &token,
                                         uint64_t &userId,
                                         std::string &fileHash,
                                         uint64_t &fileSize) {
  std::lock_guard<std::mutex> lock(m_mutex);

  auto it = m_tokens.find(token);
  if (it == m_tokens.end()) {
    std::cerr << "令牌不存在: " << token << std::endl;
    return false;
  }

  const TokenInfo &info = it->second;

  uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));

  if (currentTime <= info.gracePeriodEnd) {
    std::cout << "令牌在宽限期内: " << token
              << " (剩余: " << (info.gracePeriodEnd - currentTime) << "秒）"
              << std::endl;

    userId = info.userId;
    fileHash = info.fileHash;
    fileSize = info.fileSize;

    return true;
  }

  if (currentTime > info.expireTime) {
    std::cerr << "令牌已过期: " << token << std::endl;
    m_tokens.erase(it);
    return false;
  }

  userId = info.userId;
  fileHash = info.fileHash;
  fileSize = info.fileSize;

  std::cout << "令牌验证成功: " << token << " (用户ID: " << userId << ")"
            << std::endl;
  return true;
}

std::string TokenValidator::generateUploadToken(uint64_t userId,
                                                const std::string &fileHash,
                                                uint64_t fileSize) {
  std::lock_guard<std::mutex> lock(m_mutex);

  unsigned char randomBytes[16];
  std::ostringstream oss;
  // Use QRandomGenerator for random bytes
  for (int i = 0; i < 16; ++i) {
    randomBytes[i] = QRandomGenerator::global()->generate() % 256;
    oss << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(randomBytes[i]);
  }

  std::string randomStr = oss.str();

  std::string input = randomStr + ":" + std::to_string(userId) + ":" +
                      fileHash + ":" + std::to_string(fileSize);

  QByteArray hash = QCryptographicHash::hash(QByteArray::fromStdString(input),
                                             QCryptographicHash::Sha256);

  std::ostringstream hashOss;
  for (const char b : hash) {
    hashOss << std::hex << std::setw(2) << std::setfill('0')
            << (static_cast<int>(b) & 0xff);
  }

  std::string token = hashOss.str();

  uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));

  TokenInfo info;
  info.userId = userId;
  info.fileHash = fileHash;
  info.fileSize = fileSize;
  info.expireTime = currentTime + DEFAULT_TTL;
  info.gracePeriodEnd = info.expireTime + GRACE_PERIOD;
  info.renewCount = 0;
  info.lastRenewTime = currentTime;
  info.isActive = false;

  m_tokens[token] = info;

  std::cout << "生成上传令牌: " << token << " (用户ID: " << userId
            << ", 文件哈希: " << fileHash << ", 过期时间: " << info.expireTime
            << ", 宽限期结束: " << info.gracePeriodEnd << ")" << std::endl;
  return token;
}

void TokenValidator::cleanupExpiredTokens() {
  std::lock_guard<std::mutex> lock(m_mutex);

  uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));

  auto it = m_tokens.begin();
  while (it != m_tokens.end()) {
    if (currentTime > it->second.gracePeriodEnd) {
      std::cout << "清理过期令牌: " << it->first << std::endl;
      it = m_tokens.erase(it);
    } else {
      ++it;
    }
  }
}

void TokenValidator::markTokenActive(const std::string &token) {
  std::lock_guard<std::mutex> lock(m_mutex);

  auto it = m_tokens.find(token);
  if (it != m_tokens.end()) {
    it->second.isActive = true;
  }
}

void TokenValidator::onRenewTimer() {
  std::lock_guard<std::mutex> lock(m_mutex);

  uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));

  auto it = m_tokens.begin();
  while (it != m_tokens.end()) {
    TokenInfo &info = it->second;

    if (info.isActive && info.renewCount < MAX_RENEW_COUNT) {
      if (renewToken(it->first)) {
        std::cout << "自动续期成功: " << it->first
                  << " (续期次数: " << info.renewCount << ")" << std::endl;
      }
    }

    info.isActive = false; // 重置活跃标记
    ++it;
  }
}

bool TokenValidator::renewToken(const std::string &token) {
  auto it = m_tokens.find(token);
  if (it == m_tokens.end()) {
    return false;
  }

  TokenInfo &info = it->second;

  if (info.renewCount >= MAX_RENEW_COUNT) {
    std::cerr << "令牌已达到最大续期次数: " << token << std::endl;
    return false;
  }

  uint64_t currentTime = static_cast<uint64_t>(std::time(nullptr));

  info.expireTime = currentTime + DEFAULT_TTL;
  info.gracePeriodEnd = info.expireTime + GRACE_PERIOD;
  info.renewCount++;
  info.lastRenewTime = currentTime;

  return true;
}

void TokenValidator::onCleanupTimer() { cleanupExpiredTokens(); }

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem