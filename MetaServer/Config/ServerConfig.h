#pragma once

#include <string>
#include <map>

namespace ChatSystem {
namespace MetaServer {
namespace Config {

/**
 * @brief 服务器配置类
 */
class ServerConfig {
public:
    /**
     * @brief 获取单例实例
     * @return 单例实例指针
     */
    static ServerConfig* instance();
    
    /**
     * @brief 从文件加载配置
     * @param configFile 配置文件路径
     * @return 成功返回true，失败返回false
     */
    bool loadFromFile(const std::string& configFile);
    
    /**
     * @brief 保存配置到文件
     * @param configFile 配置文件路径
     * @return 成功返回true，失败返回false
     */
    bool saveToFile(const std::string& configFile);
    
    /**
     * @brief 获取监听IP地址
     * @return 监听IP地址
     */
    const std::string& getListenIp() const;
    
    /**
     * @brief 设置监听IP地址
     * @param ip 监听IP地址
     */
    void setListenIp(const std::string& ip);
    
    /**
     * @brief 获取监听端口
     * @return 监听端口
     */
    uint16_t getListenPort() const;
    
    /**
     * @brief 设置监听端口
     * @param port 监听端口
     */
    void setListenPort(uint16_t port);
    
    /**
     * @brief 获取数据库主机地址
     * @return 数据库主机地址
     */
    const std::string& getDbHost() const;
    
    /**
     * @brief 设置数据库主机地址
     * @param host 数据库主机地址
     */
    void setDbHost(const std::string& host);
    
    /**
     * @brief 获取数据库端口
     * @return 数据库端口
     */
    uint16_t getDbPort() const;
    
    /**
     * @brief 设置数据库端口
     * @param port 数据库端口
     */
    void setDbPort(uint16_t port);
    
    /**
     * @brief 获取数据库用户名
     * @return 数据库用户名
     */
    const std::string& getDbUser() const;
    
    /**
     * @brief 设置数据库用户名
     * @param user 数据库用户名
     */
    void setDbUser(const std::string& user);
    
    /**
     * @brief 获取数据库密码
     * @return 数据库密码
     */
    const std::string& getDbPassword() const;
    
    /**
     * @brief 设置数据库密码
     * @param password 数据库密码
     */
    void setDbPassword(const std::string& password);
    
    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string& getDbName() const;
    
    /**
     * @brief 设置数据库名称
     * @param dbName 数据库名称
     */
    void setDbName(const std::string& dbName);
    
    /**
     * @brief 获取Redis主机地址
     * @return Redis主机地址
     */
    const std::string& getRedisHost() const;
    
    /**
     * @brief 设置Redis主机地址
     * @param host Redis主机地址
     */
    void setRedisHost(const std::string& host);
    
    /**
     * @brief 获取Redis端口
     * @return Redis端口
     */
    uint16_t getRedisPort() const;
    
    /**
     * @brief 设置Redis端口
     * @param port Redis端口
     */
    void setRedisPort(uint16_t port);
    
    /**
     * @brief 获取Redis密码
     * @return Redis密码
     */
    const std::string& getRedisPassword() const;
    
    /**
     * @brief 设置Redis密码
     * @param password Redis密码
     */
    void setRedisPassword(const std::string& password);
    
    /**
     * @brief 获取Redis数据库索引
     * @return Redis数据库索引
     */
    int getRedisDb() const;
    
    /**
     * @brief 设置Redis数据库索引
     * @param db Redis数据库索引
     */
    void setRedisDb(int db);
    
    /**
     * @brief 获取心跳超时时间（秒）
     * @return 心跳超时时间
     */
    int getHeartbeatTimeout() const;
    
    /**
     * @brief 设置心跳超时时间（秒）
     * @param timeout 心跳超时时间
     */
    void setHeartbeatTimeout(int timeout);
    
    /**
     * @brief 获取上传令牌过期时间（秒）
     * @return 上传令牌过期时间
     */
    int getUploadTokenExpire() const;
    
    /**
     * @brief 设置上传令牌过期时间（秒）
     * @param expire 上传令牌过期时间
     */
    void setUploadTokenExpire(int expire);
    
private:
    /**
     * @brief 构造函数
     */
    ServerConfig();
    
    /**
     * @brief 析构函数
     */
    ~ServerConfig();
    
    /**
     * @brief 初始化默认配置
     */
    void initDefaultConfig();
    
private:
    std::string m_listenIp;           // 监听IP地址
    uint16_t m_listenPort;            // 监听端口
    
    std::string m_dbHost;             // 数据库主机地址
    uint16_t m_dbPort;                // 数据库端口
    std::string m_dbUser;             // 数据库用户名
    std::string m_dbPassword;         // 数据库密码
    std::string m_dbName;             // 数据库名称
    
    std::string m_redisHost;          // Redis主机地址
    uint16_t m_redisPort;            // Redis端口
    std::string m_redisPassword;      // Redis密码
    int m_redisDb;                   // Redis数据库索引
    
    int m_heartbeatTimeout;          // 心跳超时时间（秒）
    int m_uploadTokenExpire;         // 上传令牌过期时间（秒）
    
    static ServerConfig* s_instance;  // 单例实例指针
};

} // namespace Config
} // namespace MetaServer
} // namespace ChatSystem
