#include "ServerConfig.h"
#include <fstream>
#include <sstream>

namespace ChatSystem {
namespace MetaServer {
namespace Config {

// 初始化单例实例
ServerConfig* ServerConfig::s_instance = nullptr;

ServerConfig* ServerConfig::instance() {
    if (!s_instance) {
        s_instance = new ServerConfig();
    }
    return s_instance;
}

ServerConfig::ServerConfig() {
    // 初始化默认配置
    initDefaultConfig();
}

ServerConfig::~ServerConfig() {
    // 清理资源
}

bool ServerConfig::loadFromFile(const std::string& configFile) {
    // 实现从文件加载配置逻辑
    return false;
}

bool ServerConfig::saveToFile(const std::string& configFile) {
    // 实现保存配置到文件逻辑
    return false;
}

const std::string& ServerConfig::getListenIp() const {
    return m_listenIp;
}

void ServerConfig::setListenIp(const std::string& ip) {
    m_listenIp = ip;
}

uint16_t ServerConfig::getListenPort() const {
    return m_listenPort;
}

void ServerConfig::setListenPort(uint16_t port) {
    m_listenPort = port;
}

const std::string& ServerConfig::getDbHost() const {
    return m_dbHost;
}

void ServerConfig::setDbHost(const std::string& host) {
    m_dbHost = host;
}

uint16_t ServerConfig::getDbPort() const {
    return m_dbPort;
}

void ServerConfig::setDbPort(uint16_t port) {
    m_dbPort = port;
}

const std::string& ServerConfig::getDbUser() const {
    return m_dbUser;
}

void ServerConfig::setDbUser(const std::string& user) {
    m_dbUser = user;
}

const std::string& ServerConfig::getDbPassword() const {
    return m_dbPassword;
}

void ServerConfig::setDbPassword(const std::string& password) {
    m_dbPassword = password;
}

const std::string& ServerConfig::getDbName() const {
    return m_dbName;
}

void ServerConfig::setDbName(const std::string& dbName) {
    m_dbName = dbName;
}

const std::string& ServerConfig::getRedisHost() const {
    return m_redisHost;
}

void ServerConfig::setRedisHost(const std::string& host) {
    m_redisHost = host;
}

uint16_t ServerConfig::getRedisPort() const {
    return m_redisPort;
}

void ServerConfig::setRedisPort(uint16_t port) {
    m_redisPort = port;
}

const std::string& ServerConfig::getRedisPassword() const {
    return m_redisPassword;
}

void ServerConfig::setRedisPassword(const std::string& password) {
    m_redisPassword = password;
}

int ServerConfig::getRedisDb() const {
    return m_redisDb;
}

void ServerConfig::setRedisDb(int db) {
    m_redisDb = db;
}

int ServerConfig::getHeartbeatTimeout() const {
    return m_heartbeatTimeout;
}

void ServerConfig::setHeartbeatTimeout(int timeout) {
    m_heartbeatTimeout = timeout;
}

int ServerConfig::getUploadTokenExpire() const {
    return m_uploadTokenExpire;
}

void ServerConfig::setUploadTokenExpire(int expire) {
    m_uploadTokenExpire = expire;
}

void ServerConfig::initDefaultConfig() {
    // 初始化默认配置
    m_listenIp = "0.0.0.0";
    m_listenPort = 8080;
    
    m_dbHost = "localhost";
    m_dbPort = 3306;
    m_dbUser = "root";
    m_dbPassword = "";
    m_dbName = "chat_system";
    
    m_redisHost = "localhost";
    m_redisPort = 6379;
    m_redisPassword = "";
    m_redisDb = 0;
    
    m_heartbeatTimeout = 60;
    m_uploadTokenExpire = 3600;
}

} // namespace Config
} // namespace MetaServer
} // namespace ChatSystem
