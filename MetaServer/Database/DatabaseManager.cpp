#include "DatabaseManager.h"
#ifdef USE_NATIVE_MYSQL
#include <mysql/mysql.h>
#endif
#include <chrono>
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

DBConnection::DBConnection(MYSQL *mysql) : m_mysql(mysql) {}

DBConnection::~DBConnection() { close(); }

MYSQL *DBConnection::get() const { return m_mysql; }

bool DBConnection::isValid() const { return m_mysql != nullptr; }

void DBConnection::close() {
#ifdef USE_NATIVE_MYSQL
  if (m_mysql) {
    mysql_close(m_mysql);
    m_mysql = nullptr;
  }
#endif
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), m_port(3306), m_poolSize(10), m_initialized(false),
      m_connected(false) {}

DatabaseManager::~DatabaseManager() { close(); }

bool DatabaseManager::initialize(const std::string &host, uint16_t port,
                                 const std::string &user,
                                 const std::string &password,
                                 const std::string &database,
                                 const std::string &charset,
                                 uint32_t poolSize) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (m_initialized) {
    std::cerr << "数据库连接池已经初始化过了" << std::endl;
    return false;
  }

  m_host = host;
  m_port = port;
  m_user = user;
  m_password = password;
  m_database = database;
  m_charset = charset;
  m_poolSize = poolSize;

  m_connections.reserve(m_poolSize);

  for (uint32_t i = 0; i < m_poolSize; ++i) {
    MYSQL *conn = createConnection();
    if (conn) {
      m_connections.push_back(conn);
      m_freeConnections.push(conn);
    } else {
#ifdef USE_NATIVE_MYSQL
      std::cerr << "创建数据库连接失败，索引: " << i << std::endl;
#endif
      close();
      return false;
    }
  }

  m_initialized = true;
  m_connected = true;
  std::cout << "数据库连接池初始化成功，连接数: " << m_poolSize << std::endl;
  return true;
}

MYSQL *DatabaseManager::getConnection() {
  if (!m_initialized) {
    std::cerr << "数据库连接池未初始化" << std::endl;
    return nullptr;
  }

  std::unique_lock<std::mutex> lock(m_mutex);

  if (!m_freeConnections.empty()) {
    MYSQL *conn = m_freeConnections.front();
    m_freeConnections.pop();

    if (isConnectionValid(conn)) {
      return conn;
    } else {
#ifdef USE_NATIVE_MYSQL
      mysql_close(conn);
#endif
      MYSQL *newConn = createConnection();
      if (newConn) {
        return newConn;
      } else {
        std::cerr << "重新创建数据库连接失败" << std::endl;
        return nullptr;
      }
    }
  }

  std::cerr << "等待数据库连接..." << std::endl;
  if (m_cond.wait_for(lock, std::chrono::seconds(5),
                      [this] { return !m_freeConnections.empty(); })) {
    MYSQL *conn = m_freeConnections.front();
    m_freeConnections.pop();

    if (isConnectionValid(conn)) {
      return conn;
    } else {
#ifdef USE_NATIVE_MYSQL
      mysql_close(conn);
#endif
      return createConnection();
    }
  } else {
    std::cerr << "获取数据库连接超时" << std::endl;
    return nullptr;
  }
}

void DatabaseManager::releaseConnection(MYSQL *conn) {
  if (!conn) {
    std::cerr << "释放空指针连接" << std::endl;
    return;
  }

  std::lock_guard<std::mutex> lock(m_mutex);

  if (!isConnectionValid(conn)) {
    std::cerr << "释放无效的数据库连接" << std::endl;
#ifdef USE_NATIVE_MYSQL
    mysql_close(conn);
#endif
    return;
  }

  m_freeConnections.push(conn);
  m_cond.notify_one();
}

bool DatabaseManager::executeQuery(const std::string &sql, MYSQL_RES *&result) {
#ifdef USE_NATIVE_MYSQL
  MYSQL *conn = getConnection();
  if (!conn) {
    std::cerr << "获取数据库连接失败" << std::endl;
    return false;
  }

  if (mysql_query(conn, sql.c_str()) != 0) {
    std::cerr << "执行SQL查询失败: " << mysql_error(conn) << std::endl;
    std::cerr << "SQL: " << sql << std::endl;
    releaseConnection(conn);
    return false;
  }

  result = mysql_store_result(conn);
  if (!result && mysql_field_count(conn) > 0) {
    std::cerr << "获取结果集失败: " << mysql_error(conn) << std::endl;
    releaseConnection(conn);
    return false;
  }

  releaseConnection(conn);
  return true;
#else
  (void)sql;
  (void)result;
  return false;
#endif
}

bool DatabaseManager::executeUpdate(const std::string &sql,
                                    uint64_t &affectedRows) {
#ifdef USE_NATIVE_MYSQL
  MYSQL *conn = getConnection();
  if (!conn) {
    std::cerr << "获取数据库连接失败" << std::endl;
    return false;
  }

  if (mysql_query(conn, sql.c_str()) != 0) {
    std::cerr << "执行SQL更新失败: " << mysql_error(conn) << std::endl;
    std::cerr << "SQL: " << sql << std::endl;
    releaseConnection(conn);
    return false;
  }

  affectedRows = mysql_affected_rows(conn);
  releaseConnection(conn);
  return true;
#else
  (void)sql;
  affectedRows = 0;
  return false;
#endif
}

uint64_t DatabaseManager::getLastInsertId() {
#ifdef USE_NATIVE_MYSQL
  MYSQL *conn = getConnection();
  if (!conn) {
    std::cerr << "获取数据库连接失败" << std::endl;
    return 0;
  }

  uint64_t id = mysql_insert_id(conn);
  releaseConnection(conn);
  return id;
#else
  return 0;
#endif
}

bool DatabaseManager::beginTransaction(MYSQL *conn) {
#ifdef USE_NATIVE_MYSQL
  if (!conn) {
    std::cerr << "数据库连接为空" << std::endl;
    return false;
  }

  if (mysql_autocommit(conn, 0) != 0) {
    std::cerr << "关闭自动提交失败: " << mysql_error(conn) << std::endl;
    return false;
  }

  return true;
#else
  (void)conn;
  return false;
#endif
}

bool DatabaseManager::commitTransaction(MYSQL *conn) {
#ifdef USE_NATIVE_MYSQL
  if (!conn) {
    std::cerr << "数据库连接为空" << std::endl;
    return false;
  }

  if (mysql_commit(conn) != 0) {
    std::cerr << "提交事务失败: " << mysql_error(conn) << std::endl;
    return false;
  }

  if (mysql_autocommit(conn, 1) != 0) {
    std::cerr << "开启自动提交失败: " << mysql_error(conn) << std::endl;
    return false;
  }

  return true;
#else
  (void)conn;
  return false;
#endif
}

bool DatabaseManager::rollbackTransaction(MYSQL *conn) {
#ifdef USE_NATIVE_MYSQL
  if (!conn) {
    std::cerr << "数据库连接为空" << std::endl;
    return false;
  }

  if (mysql_rollback(conn) != 0) {
    std::cerr << "回滚事务失败: " << mysql_error(conn) << std::endl;
    return false;
  }

  if (mysql_autocommit(conn, 1) != 0) {
    std::cerr << "开启自动提交失败: " << mysql_error(conn) << std::endl;
    return false;
  }

  return true;
#else
  (void)conn;
  return false;
#endif
}

void DatabaseManager::close() {
  std::lock_guard<std::mutex> lock(m_mutex);

#ifdef USE_NATIVE_MYSQL
  for (MYSQL *conn : m_connections) {
    if (conn) {
      mysql_close(conn);
    }
  }
#endif

  m_connections.clear();
  while (!m_freeConnections.empty()) {
    m_freeConnections.pop();
  }

  m_initialized = false;
  m_connected = false;
  std::cout << "所有数据库连接已关闭" << std::endl;
}

bool DatabaseManager::isConnected() const {
  return m_connected && m_initialized;
}

void DatabaseManager::getPoolStatus(uint32_t &totalSize, uint32_t &freeSize,
                                    uint32_t &usedSize) const {
  std::lock_guard<std::mutex> lock(m_mutex);

  totalSize = static_cast<uint32_t>(m_connections.size());
  freeSize = static_cast<uint32_t>(m_freeConnections.size());
  usedSize = totalSize - freeSize;
}

MYSQL *DatabaseManager::initConnection() { return createConnection(); }

MYSQL *DatabaseManager::createConnection() {
#ifdef USE_NATIVE_MYSQL
  MYSQL *mysql = mysql_init(nullptr);
  if (!mysql) {
    std::cerr << "初始化MySQL连接失败" << std::endl;
    return nullptr;
  }

  mysql_options(mysql, MYSQL_SET_CHARSET_NAME, m_charset.c_str());
  mysql_options(mysql, MYSQL_OPT_RECONNECT, &m_connected);

  if (!mysql_real_connect(mysql, m_host.c_str(), m_user.c_str(),
                          m_password.c_str(), m_database.c_str(), m_port,
                          nullptr, CLIENT_MULTI_STATEMENTS)) {
    std::cerr << "连接MySQL失败: " << mysql_error(mysql) << std::endl;
    mysql_close(mysql);
    return nullptr;
  }

  std::cout << "创建数据库连接成功" << std::endl;
  return mysql;
#else
  return nullptr;
#endif
}

bool DatabaseManager::isConnectionValid(MYSQL *conn) {
#ifdef USE_NATIVE_MYSQL
  if (!conn) {
    return false;
  }

  return mysql_ping(conn) == 0;
#else
  (void)conn;
  return false;
#endif
}

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem