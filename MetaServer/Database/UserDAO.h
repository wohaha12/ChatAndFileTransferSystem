#pragma once

#include <cstdint>
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
  explicit UserDAO(DatabaseManager *dbManager);
  ~UserDAO();

  bool getUserByUsername(const std::string &username, uint64_t &userId,
                         std::string &passwordHash, std::string &salt,
                         std::string &nickname);
  bool getUserById(uint64_t userId, std::string &username,
                   std::string &nickname, std::string &avatarUrl,
                   uint8_t &status);
  bool updateUserStatus(uint64_t userId, uint8_t status);
  bool updateUserNickname(uint64_t userId, const std::string &nickname);
  bool updateUserAvatar(uint64_t userId, const std::string &avatarUrl);
  std::vector<uint64_t> getOnlineUsers();

private:
  DatabaseManager *m_dbManager;
};

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem
