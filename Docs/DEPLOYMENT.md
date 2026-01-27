# 部署指南

本文档详细说明如何在不同环境中部署 ChatAndFileTransferSystem 项目。

## 目录

- [部署架构](#部署架构)
- [环境准备](#环境准备)
- [数据库部署](#数据库部署)
- [Redis部署](#redis部署)
- [服务器部署](#服务器部署)
- [客户端部署](#客户端部署)
- [生产环境部署](#生产环境部署)
- [监控与维护](#监控与维护)

## 部署架构

### 系统架构

```
                    ┌─────────────┐
                    │   Client   │
                    └──────┬──────┘
                           │
                    ┌────────▼────────┐
                    │  MetaServer    │
                    │  (控制平面）    │
                    └──────┬────────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
┌───────▼──────┐  ┌───────▼──────┐  ┌───────▼──────┐
│ StorageServer1 │  │ StorageServer2 │  │ StorageServerN │
│  (数据平面）   │  │  (数据平面）   │  │  (数据平面）   │
└──────────────┘  └──────────────┘  └──────────────┘
```

### 网络拓扑

- **Client**: 用户终端，连接到 MetaServer
- **MetaServer**: 控制服务器，管理用户、文件元数据、负载均衡
- **StorageServer**: 存储服务器，负责文件存储和传输
- **MySQL**: 存储用户、文件、好友等元数据
- **Redis**: 缓存 Token、会话信息、在线用户列表

## 环境准备

### 系统要求

#### MetaServer

- **CPU**: 2核或更高
- **内存**: 4GB或更高
- **磁盘**: 20GB或更高（SSD推荐）
- **网络**: 1Gbps或更高

#### StorageServer

- **CPU**: 4核或更高
- **内存**: 8GB或更高
- **磁盘**: 100GB或更高（SSD推荐）
- **网络**: 1Gbps或更高

#### Client

- **CPU**: 2核或更高
- **内存**: 4GB或更高
- **磁盘**: 10GB或更高
- **网络**: 100Mbps或更高

### 操作系统

- **Linux**: Ubuntu 20.04+, CentOS 8+, Debian 11+
- **Windows**: Windows Server 2019+, Windows 10+
- **macOS**: macOS 10.15+

### 依赖软件

- **MySQL**: 8.0+
- **Redis**: 6.0+
- **Qt**: 6.2+ (仅客户端和MetaServer）
- **CMake**: 3.16+
- **C++编译器**: GCC 7.3+, MSVC 2019+, Clang 5.0+

## 数据库部署

### 安装MySQL

#### Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y mysql-server mysql-client
# 安全配置
sudo mysql_secure_installation
```

#### CentOS/RHEL

```bash
sudo yum install -y mysql-server mysql
# 启动MySQL服务
sudo systemctl start mysqld
sudo systemctl enable mysqld
```

#### Windows

1. 下载MySQL安装程序: https://dev.mysql.com/downloads/mysql/
2. 运行安装程序，按照向导完成安装
3. 配置MySQL服务

### 配置MySQL

#### 1. 创建数据库

```bash
mysql -u root -p < Scripts/create_database.sql
```

#### 2. 创建表结构

```bash
mysql -u root -p chat_file_transfer < Scripts/create_tables.sql
```

#### 3. 插入测试数据（可选）

```bash
mysql -u root -p chat_file_transfer < Scripts/insert_test_data.sql
```

#### 4. 创建专用用户

```bash
mysql -u root -p
```

```sql
-- 创建专用用户
CREATE USER 'chatapp'@'%' IDENTIFIED BY 'your_password';

-- 授予权限
GRANT ALL PRIVILEGES ON chat_file_transfer.* TO 'chatapp'@'%';

-- 刷新权限
FLUSH PRIVILEGES;

-- 退出
EXIT;
```

### MySQL优化配置

编辑 `/etc/mysql/mysql.conf.d/mysqld.cnf` (Linux) 或 `my.ini` (Windows):

```ini
[mysqld]
# 连接数
max_connections = 1000

# 缓冲区大小
innodb_buffer_pool_size = 1G
innodb_log_file_size = 256M

# 查询缓存
query_cache_size = 64M

# 慢查询日志
slow_query_log = /var/log/mysql/slow-query.log
long_query_time = 2
```

重启MySQL服务：

```bash
sudo systemctl restart mysql
```

## Redis部署

### 安装Redis

#### Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y redis-server
```

#### CentOS/RHEL

```bash
sudo yum install -y redis
# 启动Redis服务
sudo systemctl start redis
sudo systemctl enable redis
```

#### Windows

1. 下载Redis for Windows: https://github.com/microsoftarchive/redis/releases
2. 解压到目标目录
3. 运行 `redis-server.exe`

### 配置Redis

编辑 `/etc/redis/redis.conf` (Linux) 或 `redis.windows.conf` (Windows):

```ini
# 绑定地址
bind 0.0.0.0

# 端口
port 6379

# 密码（生产环境必须设置）
requirepass your_redis_password

# 最大内存
maxmemory 2gb

# 内存淘汰策略
maxmemory-policy allkeys-lru

# 持久化
save 900 1
save 300 10
save 60 10000
```

重启Redis服务：

```bash
sudo systemctl restart redis
```

## 服务器部署

### 构建系统

#### 安装CMake

```bash
# Ubuntu/Debian
sudo apt install -y cmake

# CentOS/RHEL
sudo yum install -y cmake

# Windows
# 下载并安装 CMake: https://cmake.org/download/
```

#### 安装Qt6

```bash
# Ubuntu/Debian
sudo apt install -y qt6-base-dev qt6-network-dev qt6-sql-dev

# CentOS/RHEL
sudo yum install -y qt6-qtbase-devel qt6-qtnetwork-devel qt6-qtsql-devel

# Windows
# 下载并安装 Qt6: https://www.qt.io/download-qt-installer
```

### 编译项目

```bash
# 克隆项目
git clone <repository-url>
cd ChatAndFileTransferSystem

# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release -j$(nproc)
```

### 部署MetaServer

```bash
# 复制可执行文件
cp bin/MetaServer /opt/chat/MetaServer

# 复制配置文件
cp config/meta_server.conf /opt/chat/config/

# 创建日志目录
mkdir -p /var/log/chat

# 设置权限
chmod +x /opt/chat/MetaServer
chown -R chat:chat /opt/chat
```

### 部署StorageServer

```bash
# 复制可执行文件
cp bin/StorageServer /opt/chat/StorageServer

# 复制配置文件
cp config/storage_server.conf /opt/chat/config/

# 创建存储目录
mkdir -p /data/chat/storage

# 设置权限
chmod +x /opt/chat/StorageServer
chown -R chat:chat /opt/chat
```

### 启动服务

```bash
# 启动MetaServer
/opt/chat/MetaServer /opt/chat/config/meta_server.conf

# 启动StorageServer
/opt/chat/StorageServer /opt/chat/config/storage_server.conf
```

### 配置systemd服务

创建 `/etc/systemd/system/chat-meta.service`:

```ini
[Unit]
Description=Chat Meta Server
After=network.target mysql.service redis.service

[Service]
Type=simple
User=chat
WorkingDirectory=/opt/chat
ExecStart=/opt/chat/MetaServer /opt/chat/config/meta_server.conf
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

创建 `/etc/systemd/system/chat-storage.service`:

```ini
[Unit]
Description=Chat Storage Server
After=network.target

[Service]
Type=simple
User=chat
WorkingDirectory=/opt/chat
ExecStart=/opt/chat/StorageServer /opt/chat/config/storage_server.conf
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

启用服务：

```bash
sudo systemctl daemon-reload
sudo systemctl enable chat-meta
sudo systemctl enable chat-storage
sudo systemctl start chat-meta
sudo systemctl start chat-storage
```

## 客户端部署

### Windows客户端

1. 下载客户端安装程序
2. 运行安装程序，按照向导完成安装
3. 启动客户端，配置服务器地址

### Linux客户端

```bash
# 解压客户端包
tar -xzf ChatClient-linux.tar.gz

# 运行客户端
./ChatClient
```

### macOS客户端

```bash
# 解压客户端包
unzip ChatClient-macos.zip

# 运行客户端
open ChatClient.app
```

## 生产环境部署

### 负载均衡

使用 Nginx 作为负载均衡器：

```nginx
upstream meta_servers {
    server meta1.example.com:8080 weight=3;
    server meta2.example.com:8080 weight=2;
    server meta3.example.com:8080 weight=1;
}

server {
    listen 80;
    server_name chat.example.com;

    location / {
        proxy_pass http://meta_servers;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

### SSL/TLS配置

```nginx
server {
    listen 443 ssl;
    server_name chat.example.com;

    ssl_certificate /etc/ssl/certs/chat.example.com.crt;
    ssl_certificate_key /etc/ssl/private/chat.example.com.key;

    location / {
        proxy_pass http://meta_servers;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

### 防火墙配置

```bash
# 开放MySQL端口
sudo ufw allow 3306/tcp

# 开放Redis端口
sudo ufw allow 6379/tcp

# 开放MetaServer端口
sudo ufw allow 8080/tcp

# 开放StorageServer端口
sudo ufw allow 8081/tcp
```

## 监控与维护

### 日志查看

```bash
# 查看MetaServer日志
tail -f /var/log/chat/meta_server.log

# 查看StorageServer日志
tail -f /var/log/chat/storage_server.log

# 查看MySQL慢查询日志
tail -f /var/log/mysql/slow-query.log
```

### 性能监控

使用 Prometheus + Grafana 进行监控：

```yaml
# prometheus.yml
scrape_configs:
  - job_name: 'chat-meta'
    static_configs:
      - targets: ['localhost:9090']
  - job_name: 'chat-storage'
    static_configs:
      - targets: ['localhost:9091']
```

### 备份策略

```bash
# MySQL备份
mysqldump -u chatapp -p chat_file_transfer > backup_$(date +%Y%m%d).sql

# Redis备份
redis-cli --rdb /backup/redis_$(date +%Y%m%d).rdb

# 文件备份
rsync -avz /data/chat/storage/ /backup/storage/
```

### 故障排查

```bash
# 检查服务状态
sudo systemctl status chat-meta
sudo systemctl status chat-storage

# 检查端口监听
sudo netstat -tlnp | grep -E '8080|8081'

# 检查连接数
sudo netstat -an | grep ESTABLISHED | wc -l
```

## 总结

本文档详细描述了聊天与文件传输系统的部署流程，主要包含以下内容：

1. **部署架构**: 系统架构和网络拓扑
2. **环境准备**: 系统要求、操作系统、依赖软件
3. **数据库部署**: MySQL 安装、配置、优化
4. **Redis部署**: Redis 安装、配置、持久化
5. **服务器部署**: CMake构建、编译、部署、systemd服务
6. **客户端部署**: Windows、Linux、macOS客户端部署
7. **生产环境**: 负载均衡、SSL/TLS、防火墙
8. **监控维护**: 日志查看、性能监控、备份策略、故障排查

所有部署流程遵循 CMake + Qt6 构建系统，确保跨平台兼容性和可维护性。
