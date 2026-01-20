#pragma once
#include <string>
#include <map>
#include <cstdint>

namespace storage_server {
namespace config {

/**
 * @class StorageServerConfig
 * @brief StorageServer配置管理器，用于加载和管理服务器配置
 */
class StorageServerConfig {
public:
    /**
     * @brief 构造函数
     */
    StorageServerConfig();
    
    /**
     * @brief 析构函数
     */
    ~StorageServerConfig();
    
    /**
     * @brief 加载配置文件
     * @param config_path 配置文件路径
     * @return 成功返回true，失败返回false
     */
    bool LoadConfig(const std::string& config_path);
    
    /**
     * @brief 获取服务器名称
     * @return 服务器名称
     */
    const std::string& GetServerName() const;
    
    /**
     * @brief 获取服务器监听端口
     * @return 监听端口
     */
    uint16_t GetListenPort() const;
    
    /**
     * @brief 获取MetaServer主机地址
     * @return MetaServer主机地址
     */
    const std::string& GetMetaServerHost() const;
    
    /**
     * @brief 获取MetaServer RPC端口
     * @return MetaServer RPC端口
     */
    uint16_t GetMetaServerRpcPort() const;
    
    /**
     * @brief 获取数据存储目录
     * @return 数据存储目录
     */
    const std::string& GetDataDir() const;
    
    /**
     * @brief 获取块大小
     * @return 块大小（字节）
     */
    size_t GetChunkSize() const;
    
    /**
     * @brief 获取最大连接数
     * @return 最大连接数
     */
    int GetMaxConnections() const;
    
    /**
     * @brief 获取心跳间隔（秒）
     * @return 心跳间隔
     */
    int GetHeartbeatInterval() const;
    
    /**
     * @brief 获取线程池大小
     * @return 线程池大小
     */
    int GetThreadPoolSize() const;
    
    /**
     * @brief 获取缓存大小（MB）
     * @return 缓存大小
     */
    size_t GetCacheSizeMB() const;
    
private:
    /**
     * @brief 设置默认配置
     */
    void SetDefaultConfig();
    
    /**
     * @brief 解析配置文件
     * @param config_path 配置文件路径
     * @return 成功返回true，失败返回false
     */
    bool ParseConfigFile(const std::string& config_path);
    
private:
    std::string server_name_;           ///< 服务器名称
    uint16_t listen_port_;             ///< 监听端口
    std::string meta_server_host_;     ///< MetaServer主机地址
    uint16_t meta_server_rpc_port_;    ///< MetaServer RPC端口
    std::string data_dir_;             ///< 数据存储目录
    size_t chunk_size_;                ///< 块大小（字节）
    int max_connections_;              ///< 最大连接数
    int heartbeat_interval_;           ///< 心跳间隔（秒）
    int thread_pool_size_;             ///< 线程池大小
    size_t cache_size_mb_;             ///< 缓存大小（MB）
    
    std::map<std::string, std::string> raw_config_;  ///< 原始配置键值对
};

} // namespace config
} // namespace storage_server
