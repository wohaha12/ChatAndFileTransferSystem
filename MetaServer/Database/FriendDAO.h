#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

class DatabaseManager;

/**
 * @brief 好友数据访问对象类
 */
class FriendDAO {
public:
  explicit FriendDAO(DatabaseManager *dbManager);
  ~FriendDAO();

  bool addFriend(uint64_t userId, uint64_t friendId);
  bool deleteFriend(uint64_t userId, uint64_t friendId);
  bool isFriend(uint64_t userId, uint64_t friendId);
  std::vector<uint64_t> getFriends(uint64_t userId);
  std::vector<uint64_t> getOnlineFriends(uint64_t userId);
  std::vector<std::string> getFriendNames(uint64_t userId);
  std::vector<std::string> getOnlineFriendNames(uint64_t userId);

private:
  DatabaseManager *m_dbManager;
};

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem