#include "RedisManager.h"
#ifdef USE_NATIVE_REDIS
#include <hiredis/hiredis.h>
#endif
#include <QFutureWatcher>
#include <QtConcurrent>

namespace ChatSystem {
namespace MetaServer {
namespace Cache {

RedisManager::RedisManager(QObject *parent)
    : QObject(parent), m_redis(nullptr), m_connected(false), m_db(0) {}

RedisManager::~RedisManager() { close(); }

bool RedisManager::initialize(const std::string &host, uint16_t port,
                              const std::string &password, int db) {
#ifdef USE_NATIVE_REDIS
  m_host = host;
  m_port = port;
  m_password = password;
  m_db = db;

  m_redis = initConnection();
  if (!m_redis) {
    std::cerr << "初始化Redis连接失败" << std::endl;
    return false;
  }

  if (!m_password.empty()) {
    redisReply *reply = static_cast<redisReply *>(
        redisCommand(m_redis, "AUTH %s", m_password.c_str()));

    if (!reply || reply->type == REDIS_REPLY_ERROR) {
      std::cerr << "Redis认证失败" << std::endl;
      if (reply) {
        freeReplyObject(reply);
      }
      return false;
    }

    if (reply) {
      freeReplyObject(reply);
    }
  }

  if (m_db != 0) {
    redisReply *reply =
        static_cast<redisReply *>(redisCommand(m_redis, "SELECT %d", m_db));

    if (!reply || reply->type == REDIS_REPLY_ERROR) {
      std::cerr << "选择Redis数据库失败" << std::endl;
      if (reply) {
        freeReplyObject(reply);
      }
      return false;
    }

    if (reply) {
      freeReplyObject(reply);
    }
  }

  m_connected = true;
  std::cout << "Redis连接初始化成功: " << host << ":" << port << std::endl;
  return true;
#else
  return false;
#endif
}

#ifdef USE_NATIVE_REDIS
redisContext *RedisManager::getConnection() {
  if (!m_connected || !m_redis) {
    std::cerr << "Redis未连接" << std::endl;
    return nullptr;
  }

  if (m_redis->err) {
    std::cerr << "Redis连接错误: " << m_redis->errstr << std::endl;
    return nullptr;
  }

  return m_redis;
}
#else
redisContext *RedisManager::getConnection() { return nullptr; }
#endif

#ifdef USE_NATIVE_REDIS
void RedisManager::releaseConnection(redisContext *conn) {
  // 单连接模式，不需要特殊处理
}
#else
void RedisManager::releaseConnection(redisContext *conn) {}
#endif

bool RedisManager::set(const std::string &key, const std::string &value,
                       int expireTime) {
#ifdef USE_NATIVE_REDIS
  std::lock_guard<std::mutex> lock(m_mutex);

  redisContext *conn = getConnection();
  if (!conn) {
    std::cerr << "获取Redis连接失败" << std::endl;
    return false;
  }

  redisReply *reply = nullptr;

  if (expireTime > 0) {
    reply = static_cast<redisReply *>(redisCommand(
        conn, "SETEX %s %d %s", key.c_str(), expireTime, value.c_str()));
  } else {
    reply = static_cast<redisReply *>(
        redisCommand(conn, "SET %s %s", key.c_str(), value.c_str()));
  }

  if (!reply || reply->type == REDIS_REPLY_ERROR) {
    std::cerr << "设置Redis键值失败: " << key << std::endl;
    if (reply) {
      freeReplyObject(reply);
    }
    return false;
  }

  if (reply) {
    freeReplyObject(reply);
  }

  return true;
#else
  return false;
#endif
}

bool RedisManager::get(const std::string &key, std::string &value) {
#ifdef USE_NATIVE_REDIS
  std::lock_guard<std::mutex> lock(m_mutex);

  redisContext *conn = getConnection();
  if (!conn) {
    std::cerr << "获取Redis连接失败" << std::endl;
    return false;
  }

  redisReply *reply =
      static_cast<redisReply *>(redisCommand(conn, "GET %s", key.c_str()));

  if (!reply || reply->type == REDIS_REPLY_ERROR) {
    std::cerr << "获取Redis键值失败: " << key << std::endl;
    if (reply) {
      freeReplyObject(reply);
    }
    return false;
  }

  if (reply->type == REDIS_REPLY_NIL) {
    if (reply) {
      freeReplyObject(reply);
    }
    return false;
  }

  if (reply->type == REDIS_REPLY_STRING) {
    value = std::string(reply->str, reply->len);
  }

  if (reply) {
    freeReplyObject(reply);
  }

  return true;
#else
  return false;
#endif
}

bool RedisManager::del(const std::string &key) {
#ifdef USE_NATIVE_REDIS
  std::lock_guard<std::mutex> lock(m_mutex);

  redisContext *conn = getConnection();
  if (!conn) {
    std::cerr << "获取Redis连接失败" << std::endl;
    return false;
  }

  redisReply *reply =
      static_cast<redisReply *>(redisCommand(conn, "DEL %s", key.c_str()));

  if (!reply || reply->type == REDIS_REPLY_ERROR) {
    std::cerr << "删除Redis键失败: " << key << std::endl;
    if (reply) {
      freeReplyObject(reply);
    }
    return false;
  }

  if (reply) {
    freeReplyObject(reply);
  }

  return true;
#else
  return false;
#endif
}

bool RedisManager::expire(const std::string &key, int expireTime) {
#ifdef USE_NATIVE_REDIS
  std::lock_guard<std::mutex> lock(m_mutex);

  redisContext *conn = getConnection();
  if (!conn) {
    std::cerr << "获取Redis连接失败" << std::endl;
    return false;
  }

  redisReply *reply = static_cast<redisReply *>(
      redisCommand(conn, "EXPIRE %s %d", key.c_str(), expireTime));

  if (!reply || reply->type == REDIS_REPLY_ERROR) {
    std::cerr << "设置Redis键过期时间失败: " << key << std::endl;
    if (reply) {
      freeReplyObject(reply);
    }
    return false;
  }

  if (reply) {
    freeReplyObject(reply);
  }

  return true;
#else
  return false;
#endif
}

bool RedisManager::exists(const std::string &key) {
#ifdef USE_NATIVE_REDIS
  std::lock_guard<std::mutex> lock(m_mutex);

  redisContext *conn = getConnection();
  if (!conn) {
    std::cerr << "获取Redis连接失败" << std::endl;
    return false;
  }

  redisReply *reply =
      static_cast<redisReply *>(redisCommand(conn, "EXISTS %s", key.c_str()));

  if (!reply || reply->type == REDIS_REPLY_ERROR) {
    std::cerr << "检查Redis键存在失败: " << key << std::endl;
    if (reply) {
      freeReplyObject(reply);
    }
    return false;
  }

  bool existsVal = false;
  if (reply->type == REDIS_REPLY_INTEGER) {
    existsVal = (reply->integer == 1);
  }

  if (reply) {
    freeReplyObject(reply);
  }

  return existsVal;
#else
  return false;
#endif
}

void RedisManager::close() {
#ifdef USE_NATIVE_REDIS
  std::lock_guard<std::mutex> lock(m_mutex);

  if (m_redis) {
    redisFree(m_redis);
    m_redis = nullptr;
    m_connected = false;
    std::cout << "Redis连接已关闭" << std::endl;
  }
#endif
}

bool RedisManager::isConnected() const {
#ifdef USE_NATIVE_REDIS
  return m_connected && m_redis && m_redis->err == 0;
#else
  return false;
#endif
}

#ifdef USE_NATIVE_REDIS
redisContext *RedisManager::initConnection() {
  struct timeval timeout = {1, 500000};

  redisContext *redis =
      redisConnectWithTimeout(m_host.c_str(), m_port, timeout);

  if (redis == nullptr || redis->err) {
    std::cerr << "连接Redis失败: " << (redis ? redis->errstr : "无法分配上下文")
              << std::endl;
    return nullptr;
  }

  return redis;
}
#endif

QFuture<bool> RedisManager::setAsync(const std::string &key,
                                     const std::string &value, int expireTime) {
  return QtConcurrent::run(&RedisManager::set, this, key, value, expireTime);
}

QFuture<std::pair<bool, std::string>>
RedisManager::getAsync(const std::string &key) {
  return QtConcurrent::run([this, key] {
    std::string val;
    bool ok = get(key, val);
    return std::make_pair(ok, val);
  });
}

QFuture<bool> RedisManager::delAsync(const std::string &key) {
  return QtConcurrent::run(&RedisManager::del, this, key);
}

QFuture<bool> RedisManager::existsAsync(const std::string &key) {
  return QtConcurrent::run(&RedisManager::exists, this, key);
}

} // namespace Cache
} // namespace MetaServer
} // namespace ChatSystem