#include "FriendDAO.h"
#include "DatabaseManager.h"
#ifdef USE_NATIVE_MYSQL
#include <mysql/mysql.h>
#endif
#include <iostream>
#include <sstream>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

FriendDAO::FriendDAO(DatabaseManager *dbManager) : m_dbManager(dbManager) {}
FriendDAO::~FriendDAO() {}

bool FriendDAO::addFriend(uint64_t userId, uint64_t friendId) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  if (isFriend(userId, friendId))
    return false;
  std::ostringstream oss;
  oss << "INSERT INTO t_friend (user_id, friend_id) VALUES (" << userId << ", "
      << friendId << ")";
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FriendDAO::deleteFriend(uint64_t userId, uint64_t friendId) {
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "DELETE FROM t_friend WHERE user_id = " << userId
      << " AND friend_id = " << friendId;
  uint64_t affectedRows = 0;
  return m_dbManager->executeUpdate(oss.str(), affectedRows) &&
         affectedRows > 0;
}

bool FriendDAO::isFriend(uint64_t userId, uint64_t friendId) {
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return false;
  std::ostringstream oss;
  oss << "SELECT COUNT(*) FROM t_friend WHERE user_id = " << userId
      << " AND friend_id = " << friendId;
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return false;
  if (!result)
    return false;
  MYSQL_ROW row = mysql_fetch_row(result);
  bool isFriendRelation = false;
  if (row && row[0])
    isFriendRelation = std::stoi(row[0]) > 0;
  mysql_free_result(result);
  return isFriendRelation;
#else
  return false;
#endif
}

std::vector<uint64_t> FriendDAO::getFriends(uint64_t userId) {
  std::vector<uint64_t> friends;
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return friends;
  std::ostringstream oss;
  oss << "SELECT friend_id FROM t_friend WHERE user_id = " << userId;
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return friends;
  if (!result)
    return friends;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    if (row[0])
      friends.push_back(std::stoull(row[0]));
  }
  mysql_free_result(result);
#endif
  return friends;
}

std::vector<uint64_t> FriendDAO::getOnlineFriends(uint64_t userId) {
  std::vector<uint64_t> onlineFriends;
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return onlineFriends;
  std::ostringstream oss;
  oss << "SELECT f.friend_id FROM t_friend f INNER JOIN t_user u ON "
         "f.friend_id = u.id "
         "WHERE f.user_id = "
      << userId << " AND u.status = 1";
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return onlineFriends;
  if (!result)
    return onlineFriends;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    if (row[0])
      onlineFriends.push_back(std::stoull(row[0]));
  }
  mysql_free_result(result);
#endif
  return onlineFriends;
}

std::vector<std::string> FriendDAO::getFriendNames(uint64_t userId) {
  std::vector<std::string> friendNames;
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return friendNames;
  std::ostringstream oss;
  oss << "SELECT u.username FROM t_friend f INNER JOIN t_user u ON f.friend_id "
         "= u.id WHERE f.user_id = "
      << userId;
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return friendNames;
  if (!result)
    return friendNames;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    if (row[0])
      friendNames.push_back(row[0]);
  }
  mysql_free_result(result);
#endif
  return friendNames;
}

std::vector<std::string> FriendDAO::getOnlineFriendNames(uint64_t userId) {
  std::vector<std::string> onlineFriendNames;
#ifdef USE_NATIVE_MYSQL
  if (!m_dbManager || !m_dbManager->isConnected())
    return onlineFriendNames;
  std::ostringstream oss;
  oss << "SELECT u.username FROM t_friend f INNER JOIN t_user u ON f.friend_id "
         "= u.id "
         "WHERE f.user_id = "
      << userId << " AND u.status = 1";
  MYSQL_RES *result = nullptr;
  if (!m_dbManager->executeQuery(oss.str(), result))
    return onlineFriendNames;
  if (!result)
    return onlineFriendNames;
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(result))) {
    if (row[0])
      onlineFriendNames.push_back(row[0]);
  }
  mysql_free_result(result);
#endif
  return onlineFriendNames;
}

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem