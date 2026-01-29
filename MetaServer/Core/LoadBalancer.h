#pragma once

#include <string>
#include <map>
#include <mutex>
#include <QObject>

namespace ChatSystem {
namespace MetaServer {
namespace Core {

/**
 * @brief 负载均衡器类
 */
class LoadBalancer : public QObject {
    Q_OBJECT
public:
    explicit LoadBalancer(QObject* parent = nullptr);
    ~LoadBalancer();
    
    /**
     * @brief 添加存储服务器
     * @param serverId 服务器ID
     * @param ip 服务器IP地址
     * @param port 服务器端口
     * @param weight 服务器权重，默认1
     * @return 成功返回true，失败返回false
     */
    bool addStorageServer(uint32_t serverId, const std::string& ip, uint16_t port, int weight = 1);
    
    /**
     * @brief 移除存储服务器
     * @param serverId 服务器ID
     * @return 成功返回true，失败返回false
     */
    bool removeStorageServer(uint32_t serverId);
    
    /**
     * @brief 更新服务器状态
     * @param serverId 服务器ID
     * @param cpuUsage CPU使用率
     * @param memoryUsage 内存使用率
     * @param diskUsage 磁盘使用率
     * @param connections 当前连接数
     * @return 成功返回true，失败返回false
     */
    bool updateServerStatus(uint32_t serverId, float cpuUsage, float memoryUsage, 
                         float diskUsage, uint32_t connections);
    
    /**
     * @brief 选择最佳存储服务器
     * @param fileSize 文件大小，默认0
     * @return 选中的服务器ID，失败返回0
     */
    uint32_t selectBestServer(uint64_t fileSize = 0);
    
    /**
     * @brief 获取服务器信息
     * @param serverId 服务器ID
     * @param ip 输出IP地址
     * @param port 输出端口
     * @return 成功返回true，失败返回false
     */
    bool getServerInfo(uint32_t serverId, std::string& ip, uint16_t& port) const;
    
    /**
     * @brief 获取所有在线服务器
     * @return 在线服务器ID列表
     */
    std::vector<uint32_t> getOnlineServers() const;
    
signals:
    /**
     * @brief 服务器状态变化信号
     * @param serverId 服务器ID
     * @param online 服务器是否在线
     */
    void serverStatusChanged(uint32_t serverId, bool online);
    
private:
    /**
     * @brief 服务器信息结构体
     */
    struct ServerInfo {
        uint32_t serverId;      // 服务器ID
        std::string ip;         // IP地址
        uint16_t port;          // 端口
        int weight;             // 权重
        bool online;            // 是否在线
        float cpuUsage;         // CPU使用率
        float memoryUsage;      // 内存使用率
        float diskUsage;        // 磁盘使用率
        uint32_t connections;   // 当前连接数
        uint64_t lastHeartbeat; // 最后心跳时间
    int currentWeight;     // 当前权重（用于加权轮询）
    };
    
    /**
     * @brief 计算服务器负载分数
     * @param server 服务器信息
     * @param fileSize 文件大小
     * @return 负载分数，分数越低负载越轻
     */
    float calculateLoadScore(const ServerInfo& server, uint64_t fileSize) const;
    
    /**
     * @brief 检查服务器是否在线
     * @param server 服务器信息
     * @return 在线返回true，离线返回false
     */
    bool isServerOnline(const ServerInfo& server) const;
    
    /**
     * @brief 基于连接数的加权轮询算法
     * @return 选中的服务器ID
     */
    uint32_t selectByWeightedRoundRobin();
    
private:
    std::map<uint32_t, ServerInfo> m_servers;  // 服务器信息映射
    mutable std::mutex m_mutex;                // 互斥锁
    int m_currentWeight;                        // 当前权重（用于加权轮询）
};

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem
