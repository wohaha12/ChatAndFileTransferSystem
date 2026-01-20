#include "LoadBalancer.h"
#include <algorithm>
#include <iostream>
#include <ctime>
#include <limits>

namespace ChatSystem {
namespace MetaServer {
namespace Core {

LoadBalancer::LoadBalancer(QObject* parent)
    : QObject(parent)
    , m_currentWeight(0)
{
}

LoadBalancer::~LoadBalancer()
{
}

bool LoadBalancer::addStorageServer(uint32_t serverId, const std::string& ip, uint16_t port, int weight)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_servers.find(serverId) != m_servers.end()) {
        std::cerr << "服务器已存在: " << serverId << std::endl;
        return false;
    }
    
    ServerInfo info;
    info.serverId = serverId;
    info.ip = ip;
    info.port = port;
    info.weight = weight;
    info.online = false;
    info.cpuUsage = 0.0f;
    info.memoryUsage = 0.0f;
    info.diskUsage = 0.0f;
    info.connections = 0;
    info.lastHeartbeat = 0;
    
    m_servers[serverId] = info;
    
    std::cout << "添加存储服务器成功: " << serverId << " (" << ip << ":" << port << ")" << std::endl;
    return true;
}

bool LoadBalancer::removeStorageServer(uint32_t serverId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_servers.find(serverId);
    if (it == m_servers.end()) {
        std::cerr << "服务器不存在: " << serverId << std::endl;
        return false;
    }
    
    m_servers.erase(it);
    
    std::cout << "移除存储服务器成功: " << serverId << std::endl;
    return true;
}

bool LoadBalancer::updateServerStatus(uint32_t serverId, float cpuUsage, float memoryUsage, 
                                     float diskUsage, uint32_t connections)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_servers.find(serverId);
    if (it == m_servers.end()) {
        std::cerr << "服务器不存在: " << serverId << std::endl;
        return false;
    }
    
    bool wasOnline = it->second.online;
    it->second.cpuUsage = cpuUsage;
    it->second.memoryUsage = memoryUsage;
    it->second.diskUsage = diskUsage;
    it->second.connections = connections;
    it->second.lastHeartbeat = std::time(nullptr);
    it->second.online = true;
    
    if (!wasOnline) {
        emit serverStatusChanged(serverId, true);
        std::cout << "服务器上线: " << serverId << std::endl;
    }
    
    return true;
}

uint32_t LoadBalancer::selectBestServer(uint64_t fileSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<uint32_t> onlineServers;
    int totalWeight = 0;
    
    for (auto& pair : m_servers) {
        ServerInfo& server = pair.second;
        if (server.online && server.connections < (server.weight * 100)) {
            onlineServers.push_back(server.serverId);
            totalWeight += server.weight;
        }
    }
    
    if (onlineServers.empty() || totalWeight == 0) {
        std::cerr << "没有可用的在线服务器" << std::endl;
        return 0;
    }
    
    uint32_t bestServer = selectByWeightedRoundRobin();
    
    if (bestServer != 0) {
        std::cout << "选择存储服务器: " << bestServer << std::endl;
    }
    
    return bestServer;
}

bool LoadBalancer::getServerInfo(uint32_t serverId, std::string& ip, uint16_t& port) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_servers.find(serverId);
    if (it == m_servers.end()) {
        std::cerr << "服务器不存在: " << serverId << std::endl;
        return false;
    }
    
    ip = it->second.ip;
    port = it->second.port;
    return true;
}

std::vector<uint32_t> LoadBalancer::getOnlineServers() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<uint32_t> onlineServers;
    for (const auto& pair : m_servers) {
        if (pair.second.online) {
            onlineServers.push_back(pair.first);
        }
    }
    
    return onlineServers;
}

float LoadBalancer::calculateLoadScore(const ServerInfo& server, uint64_t fileSize) const
{
    if (!server.online) {
        return std::numeric_limits<float>::max();
    }
    
    if (server.connections >= static_cast<uint32_t>(server.weight * 100)) {
        return std::numeric_limits<float>::max();
    }
    
    float maxConnections = server.weight * 100.0f;
    float score = static_cast<float>(server.connections) / maxConnections;
    
    return score;
}

bool LoadBalancer::isServerOnline(const ServerInfo& server) const
{
    uint64_t currentTime = std::time(nullptr);
    uint64_t heartbeatTimeout = 60;
    
    return server.online && (currentTime - server.lastHeartbeat < heartbeatTimeout);
}

uint32_t LoadBalancer::selectByWeightedRoundRobin()
{
    std::vector<uint32_t> onlineServers;
    int totalWeight = 0;
    
    for (auto& pair : m_servers) {
        ServerInfo& server = pair.second;
        if (server.online && server.connections < static_cast<uint32_t>(server.weight * 100)) {
            onlineServers.push_back(server.serverId);
            totalWeight += server.weight;
        }
    }
    
    if (onlineServers.empty() || totalWeight == 0) {
        return 0;
    }
    
    m_currentWeight += totalWeight;
    
    uint32_t bestServer = 0;
    int maxCurrentWeight = -1;
    
    for (uint32_t serverId : onlineServers) {
        ServerInfo& server = m_servers[serverId];
        server.currentWeight += server.weight;
        
        if (server.currentWeight > maxCurrentWeight) {
            maxCurrentWeight = server.currentWeight;
            bestServer = serverId;
        }
    }
    
    if (bestServer != 0) {
        m_servers[bestServer].currentWeight -= m_currentWeight;
    }
    
    m_currentWeight = 0;
    
    return bestServer;
}

} // namespace Core
} // namespace MetaServer
} // namespace ChatSystem