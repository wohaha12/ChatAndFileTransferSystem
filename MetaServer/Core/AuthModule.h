#pragma once

#include <QObject>
#include <memory>
#include <string>

namespace ChatSystem {
namespace MetaServer {

namespace Database {
class DatabaseManager;
}

namespace Core {

using Database::DatabaseManager;

/**
 * @brief 认证模块类
 */
class AuthModule : public QObject {
  Q_OBJECT
public:
  explicit AuthModule(QObject *parent = nullptr);
  ~AuthModule();

  /**
   * @brief 设置数据库管理器
   * @param dbManager 数据库管理器指针
   */
  void setDatabaseManager(DatabaseManager *dbManager);

  /**
   * @brief 用户登录认证
   */
  bool authenticate(const std::string &username,
                    const std::string &passwordHash, uint64_t &userId,
                    std::string &nickname, std::string &sessionToken);

  /**
   * @brief 验证会话令牌
   */
  bool validateSessionToken(uint64_t userId, const std::string &sessionToken);

  /**
   * @brief 用户登出
   */
  bool logout(uint64_t userId, const std::string &sessionToken);

  /**
   * @brief 更新用户状态
   */
  bool updateUserStatus(uint64_t userId, uint8_t status);

private:
  std::string generateSessionToken(uint64_t userId);
  std::string computePasswordHash(const std::string &password,
                                  const std::string &salt);
  std::string generateSalt();

private:
  DatabaseManager *m_dbManager; // 数据库管理器
};

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem
