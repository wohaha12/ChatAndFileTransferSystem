#include "StorageServerConfig.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace storage_server {
namespace config {

StorageServerConfig::StorageServerConfig() { SetDefaultConfig(); }

StorageServerConfig::~StorageServerConfig() {}

bool StorageServerConfig::LoadConfig(const std::string &config_path) {
  // 先设置默认配置
  SetDefaultConfig();

  // 解析配置文件
  if (!ParseConfigFile(config_path)) {
    std::cerr << "Failed to parse config file: " << config_path << std::endl;
    return false;
  }

  // 从raw_config_中提取配置项
  if (raw_config_.count("StorageServer/server_name"))
    server_name_ = raw_config_["StorageServer/server_name"];
  if (raw_config_.count("Network/port"))
    listen_port_ = (uint16_t)std::stoi(raw_config_["Network/port"]);
  if (raw_config_.count("MetaServer/host"))
    meta_server_host_ = raw_config_["MetaServer/host"];
  if (raw_config_.count("MetaServer/port"))
    meta_server_rpc_port_ = (uint16_t)std::stoi(raw_config_["MetaServer/port"]);
  if (raw_config_.count("Storage/storage_path"))
    data_dir_ = raw_config_["Storage/storage_path"];
  if (raw_config_.count("Storage/chunk_size"))
    chunk_size_ = (size_t)std::stoll(raw_config_["Storage/chunk_size"]);
  if (raw_config_.count("Network/max_connections"))
    max_connections_ = std::stoi(raw_config_["Network/max_connections"]);
  if (raw_config_.count("MetaServer/heartbeat_interval"))
    heartbeat_interval_ =
        std::stoi(raw_config_["MetaServer/heartbeat_interval"]);
  if (raw_config_.count("Storage/thread_pool_size"))
    thread_pool_size_ = std::stoi(raw_config_["Storage/thread_pool_size"]);
  if (raw_config_.count("Storage/cache_size_mb"))
    cache_size_mb_ = (size_t)std::stoll(raw_config_["Storage/cache_size_mb"]);

  return true;
}

const std::string &StorageServerConfig::GetServerName() const {
  return server_name_;
}

uint16_t StorageServerConfig::GetListenPort() const { return listen_port_; }

const std::string &StorageServerConfig::GetMetaServerHost() const {
  return meta_server_host_;
}

uint16_t StorageServerConfig::GetMetaServerRpcPort() const {
  return meta_server_rpc_port_;
}

const std::string &StorageServerConfig::GetDataDir() const { return data_dir_; }

size_t StorageServerConfig::GetChunkSize() const { return chunk_size_; }

int StorageServerConfig::GetMaxConnections() const { return max_connections_; }

int StorageServerConfig::GetHeartbeatInterval() const {
  return heartbeat_interval_;
}

int StorageServerConfig::GetThreadPoolSize() const { return thread_pool_size_; }

size_t StorageServerConfig::GetCacheSizeMB() const { return cache_size_mb_; }

void StorageServerConfig::SetDefaultConfig() {
  server_name_ = "StorageServer";
  listen_port_ = 9000;
  meta_server_host_ = "127.0.0.1";
  meta_server_rpc_port_ = 8001;
  data_dir_ = "./data";
  chunk_size_ = 1024 * 1024; // 1MB
  max_connections_ = 1000;
  heartbeat_interval_ = 30;
  thread_pool_size_ = 4;
  cache_size_mb_ = 1024; // 1GB

  raw_config_.clear();
}

bool StorageServerConfig::ParseConfigFile(const std::string &config_path) {
  // 实现配置文件解析逻辑
  std::ifstream file(config_path);
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  std::string current_section = "";
  while (std::getline(file, line)) {
    // 移除前后空格
    line.erase(0, line.find_first_not_of(" \t\r\n"));
    if (!line.empty()) {
      line.erase(line.find_last_not_of(" \t\r\n") + 1);
    }

    // 跳过空行和注释行
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // 处理块 [Section]
    if (line[0] == '[' && line.back() == ']') {
      current_section = line.substr(1, line.length() - 2);
      continue;
    }

    // 解析键值对
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);

      // 移除键值对前后空格
      key.erase(0, key.find_first_not_of(" \t"));
      if (!key.empty())
        key.erase(key.find_last_not_of(" \t") + 1);
      value.erase(0, value.find_first_not_of(" \t"));
      if (!value.empty())
        value.erase(value.find_last_not_of(" \t") + 1);

      if (!current_section.empty()) {
        raw_config_[current_section + "/" + key] = value;
      } else {
        raw_config_[key] = value;
      }
    }
  }
  file.close();
  return true;
}

} // namespace config
} // namespace storage_server
