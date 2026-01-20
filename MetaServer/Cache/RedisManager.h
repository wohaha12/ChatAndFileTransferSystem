#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <QObject>

// 前向声明Redis相关类型
struct redisContext;

namespace ChatSystem {
namespace MetaServer {
namespace Cache {

/**
 * @brief Redis管理器类
 */
class RedisManager : public QObject {
    Q_OBJECT
public:
    explicit RedisManager(QObject* parent = nullptr);
    ~RedisManager();
    
    /**
     * @brief 初始化Redis连接
     * @param host Redis主机地址
     * @param port Redis端口
     * @param password Redis密码
     * @param db 数据库索引，默认0
     * @return 成功返回true，失败返回false
     */
    bool initialize(const std::string& host, uint16_t port, const std::string& password = "", int db = 0);
    
    /**
     * @brief 获取Redis连接
     * @return Redis连接指针，失败返回nullptr
     */
    redisContext* getConnection();
    
    /**
     * @brief 释放Redis连接
     * @param conn Redis连接指针
     */
    void releaseConnection(redisContext* conn);
    
    /**
     * @brief 设置键值对
     * @param key 键
     * @param value 值
     * @param expire 过期时间（秒），默认0表示不过期
     * @return 成功返回true，失败返回false
     */
    bool set(const std::string& key, const std::string& value, int expire = 0);
    
    /**
     * @brief 获取键对应的值
     * @param key 键
     * @param value 输出值
     * @return 成功返回true，失败返回false
     */
    bool get(const std::string& key, std::string& value);
    
    /**
     * @brief 删除键
     * @param key 键
     * @return 成功返回true，失败返回false
     */
    bool del(const std::string& key);
    
    /**
     * @brief 设置键的过期时间
     * @param key 键
     * @param expire 过期时间（秒）
     * @return 成功返回true，失败返回false
     */
    bool expire(const std::string& key, int expire);
    
    /**
     * @brief 检查键是否存在
     * @param key 键
     * @return 存在返回true，不存在返回false
     */
    bool exists(const std::string& key);
    
    /**
     * @brief 关闭Redis连接
     */
    void close();
    
    /**
     * @brief 获取Redis连接状态
     * @return 连接是否正常
     */
    bool isConnected() const;
    
private:
    /**
     * @brief 初始化Redis连接
     * @return 连接成功返回Redis连接指针，失败返回nullptr
     */
    redisContext* initConnection();
    
private:
    std::string m_host;           // Redis主机地址
    uint16_t m_port;              // Redis端口
    std::string m_password;       // Redis密码
    int m_db;                     // 数据库索引
    
    redisContext* m_redis;        // Redis连接
    bool m_connected;             // 连接状态
    mutable std::mutex m_mutex;   // 互斥锁
};

} // namespace Cache
} // namespace MetaServer
} // namespace ChatSystem
