#include "StorageServerConfig.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace storage_server {
namespace config {

StorageServerConfig::StorageServerConfig() {
    SetDefaultConfig();
}

StorageServerConfig::~StorageServerConfig() {
}

bool StorageServerConfig::LoadConfig(const std::string& config_path) {
    // 先设置默认配置
    SetDefaultConfig();
    
    // 解析配置文件
    if (!ParseConfigFile(config_path)) {
        std::cerr << "Failed to parse config file: " << config_path << std::endl;
        return false;
    }
    
    // 从raw_config_中提取配置项
    // 这里可以根据实际配置文件格式进行解析
    
    return true;
}

const std::string& StorageServerConfig::GetServerName() const {
    return server_name_;
}

uint16_t StorageServerConfig::GetListenPort() const {
    return listen_port_;
}

const std::string& StorageServerConfig::GetMetaServerHost() const {
    return meta_server_host_;
}

uint16_t StorageServerConfig::GetMetaServerRpcPort() const {
    return meta_server_rpc_port_;
}

const std::string& StorageServerConfig::GetDataDir() const {
    return data_dir_;
}

size_t StorageServerConfig::GetChunkSize() const {
    return chunk_size_;
}

int StorageServerConfig::GetMaxConnections() const {
    return max_connections_;
}

int StorageServerConfig::GetHeartbeatInterval() const {
    return heartbeat_interval_;
}

int StorageServerConfig::GetThreadPoolSize() const {
    return thread_pool_size_;
}

size_t StorageServerConfig::GetCacheSizeMB() const {
    return cache_size_mb_;
}

void StorageServerConfig::SetDefaultConfig() {
    server_name_ = "StorageServer";
    listen_port_ = 9000;
    meta_server_host_ = "127.0.0.1";
    meta_server_rpc_port_ = 8001;
    data_dir_ = "./data";
    chunk_size_ = 1024 * 1024;  // 1MB
    max_connections_ = 1000;
    heartbeat_interval_ = 30;
    thread_pool_size_ = 4;
    cache_size_mb_ = 1024;  // 1GB
    
    raw_config_.clear();
}

bool StorageServerConfig::ParseConfigFile(const std::string& config_path) {
    // 实现配置文件解析逻辑
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // 跳过空行和注释行
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // 解析键值对
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // 移除前后空格
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            raw_config_[key] = value;
        }
    }
    
    file.close();
    return true;
}

} // namespace config
} // namespace storage_server
