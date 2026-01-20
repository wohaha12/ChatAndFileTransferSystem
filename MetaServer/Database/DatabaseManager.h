#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <QObject>
#include <vector>

// 前向声明MySQL相关类型
struct MYSQL;
struct MYSQL_RES;

namespace ChatSystem {
namespace MetaServer {
namespace Database {

/**
 * @brief 数据库连接包装类
 * @details 管理单个数据库连接的生命周期
 */
class DBConnection {
public:
    explicit DBConnection(MYSQL* mysql);
    ~DBConnection();
    
    MYSQL* get() const;
    bool isValid() const;
    void close();
    
private:
    MYSQL* m_mysql;
};

/**
 * @brief 数据库连接管理器类
 * @details 支持连接池的数据库管理器，提供高效的数据库访问
 */
class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();
    
    /**
     * @brief 初始化数据库连接池
     * @param host 数据库主机地址
     * @param port 数据库端口
     * @param user 数据库用户名
     * @param password 数据库密码
     * @param database 数据库名称
     * @param charset 字符集，默认utf8mb4
     * @param poolSize 连接池大小，默认10
     * @return 成功返回true，失败返回false
     */
    bool initialize(const std::string& host, uint16_t port, const std::string& user, 
                   const std::string& password, const std::string& database, 
                   const std::string& charset = "utf8mb4", uint32_t poolSize = 10);
    
    /**
     * @brief 获取数据库连接
     * @return 数据库连接指针，失败返回nullptr
     */
    MYSQL* getConnection();
    
    /**
     * @brief 释放数据库连接
     * @param conn 数据库连接指针
     */
    void releaseConnection(MYSQL* conn);
    
    /**
     * @brief 执行SQL查询
     * @param sql SQL语句
     * @param result 输出结果集
     * @return 成功返回true，失败返回false
     */
    bool executeQuery(const std::string& sql, MYSQL_RES*& result);
    
    /**
     * @brief 执行SQL更新（INSERT/UPDATE/DELETE）
     * @param sql SQL语句
     * @param affectedRows 输出影响的行数
     * @return 成功返回true，失败返回false
     */
    bool executeUpdate(const std::string& sql, uint64_t& affectedRows);
    
    /**
     * @brief 获取最后插入的ID
     * @return 最后插入的ID
     */
    uint64_t getLastInsertId();
    
    /**
     * @brief 开始事务
     * @param conn 数据库连接
     * @return 成功返回true，失败返回false
     */
    bool beginTransaction(MYSQL* conn);
    
    /**
     * @brief 提交事务
     * @param conn 数据库连接
     * @return 成功返回true，失败返回false
     */
    bool commitTransaction(MYSQL* conn);
    
    /**
     * @brief 回滚事务
     * @param conn 数据库连接
     * @return 成功返回true，失败返回false
     */
    bool rollbackTransaction(MYSQL* conn);
    
    /**
     * @brief 关闭数据库连接池
     */
    void close();
    
    /**
     * @brief 获取数据库连接状态
     * @return 连接是否正常
     */
    bool isConnected() const;
    
    /**
     * @brief 获取连接池状态
     * @param totalSize 总连接数
     * @param freeSize 空闲连接数
     * @param usedSize 使用中连接数
     */
    void getPoolStatus(uint32_t& totalSize, uint32_t& freeSize, uint32_t& usedSize) const;
    
private:
    /**
     * @brief 初始化数据库连接
     * @return 连接成功返回MySQL连接指针，失败返回nullptr
     */
    MYSQL* initConnection();
    
    /**
     * @brief 创建新的数据库连接
     * @return 数据库连接指针
     */
    MYSQL* createConnection();
    
    /**
     * @brief 检查连接是否有效
     * @param conn 数据库连接
     * @return 有效返回true，无效返回false
     */
    bool isConnectionValid(MYSQL* conn);
    
private:
    std::string m_host;           // 数据库主机地址
    uint16_t m_port;              // 数据库端口
    std::string m_user;           // 数据库用户名
    std::string m_password;       // 数据库密码
    std::string m_database;       // 数据库名称
    std::string m_charset;        // 字符集
    uint32_t m_poolSize;        // 连接池大小
    
    std::vector<MYSQL*> m_connections;  // 所有连接
    std::queue<MYSQL*> m_freeConnections; // 空闲连接队列
    mutable std::mutex m_mutex;   // 互斥锁
    std::condition_variable m_cond; // 条件变量
    bool m_initialized;          // 是否已初始化
    bool m_connected;            // 连接状态
};

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem
