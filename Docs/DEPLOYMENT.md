# 部署指南

本文档详细说明如何在不同环境中部署ChatAndFileTransferSystem项目。

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

- **Client**: 用户终端，连接到MetaServer
- **MetaServer**: 控制服务器，管理用户、文件元数据、负载均衡
- **StorageServer**: 存储服务器，负责文件存储和传输
- **MySQL**: 存储用户、文件、好友等元数据
- **Redis**: 缓存Token、会话信息、在线用户列表

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
2. 解压到指定目录
3. 运行 `redis-server.exe`

### 配置Redis

编辑 `/etc/redis/redis.conf` (Linux) 或 `redis.windows.conf` (Windows):

```ini
# 绑定地址
bind 0.0.0.0

# 端口
port 6379

# 最大内存
maxmemory 2gb

# 内存淘汰策略
maxmemory-policy allkeys-lru

# 持久化
save 900 1
save 300 10
save 60 10000

# 日志级别
loglevel notice

# 日志文件
logfile /var/log/redis/redis-server.log
```

重启Redis服务：

```bash
sudo systemctl restart redis
```

### 验证Redis

```bash
redis-cli ping
# 应该返回: PONG

redis-cli info
# 查看Redis信息
```

## 服务器部署

### 部署MetaServer

#### 1. 构建项目

```bash
# 使用构建脚本
./Scripts/build.sh

# 或者手动构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### 2. 配置MetaServer

编辑 `Config/metaserver.conf`：

```ini
[Network]
host=0.0.0.0
port=8000
max_connections=1000

[Database]
host=localhost
port=3306
database=chat_file_transfer
username=chatapp
password=your_password
pool_size=10
timeout=5

[Redis]
host=localhost
port=6379
password=
db=0
timeout=5
```

#### 3. 部署MetaServer

```bash
# 使用部署脚本
./Scripts/deploy.sh

# 或者手动部署
mkdir -p /opt/chatfiletransfer/metaserver/{bin,config,logs,data}
cp build/bin/MetaServer /opt/chatfiletransfer/metaserver/bin/
cp Config/metaserver.conf /opt/chatfiletransfer/metaserver/config/
```

#### 4. 创建systemd服务（Linux）

创建 `/etc/systemd/system/metaserver.service`：

```ini
[Unit]
Description=ChatAndFileTransfer MetaServer
After=network.target mysql.service redis.service

[Service]
Type=simple
User=chatapp
Group=chatapp
WorkingDirectory=/opt/chatfiletransfer/metaserver
ExecStart=/opt/chatfiletransfer/metaserver/bin/MetaServer -c /opt/chatfiletransfer/metaserver/config/metaserver.conf
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

启动服务：

```bash
sudo systemctl daemon-reload
sudo systemctl start metaserver
sudo systemctl enable metaserver
sudo systemctl status metaserver
```

### 部署StorageServer

#### 1. 构建项目

```bash
# 使用构建脚本
./Scripts/build.sh

# 或者手动构建
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### 2. 配置StorageServer

编辑 `Config/storageserver.conf`：

```ini
[Network]
host=0.0.0.0
port=9001
max_connections=1000

[MetaServer]
host=metaserver_ip
port=8000
heartbeat_interval=30

[Storage]
storage_path=/opt/chatfiletransfer/storageserver/storage
temp_path=/opt/chatfiletransfer/storageserver/temp
chunk_size=1048576
max_file_size=10737418240
disk_usage_threshold=90
```

#### 3. 部署StorageServer

```bash
# 使用部署脚本
./Scripts/deploy.sh

# 或者手动部署
mkdir -p /opt/chatfiletransfer/storageserver/{bin,config,logs,data/{storage,temp}}
cp build/bin/StorageServer /opt/chatfiletransfer/storageserver/bin/
cp Config/storageserver.conf /opt/chatfiletransfer/storageserver/config/
```

#### 4. 创建systemd服务（Linux）

创建 `/etc/systemd/system/storageserver.service`：

```ini
[Unit]
Description=ChatAndFileTransfer StorageServer
After=network.target

[Service]
Type=simple
User=chatapp
Group=chatapp
WorkingDirectory=/opt/chatfiletransfer/storageserver
ExecStart=/opt/chatfiletransfer/storageserver/bin/StorageServer -c /opt/chatfiletransfer/storageserver/config/storageserver.conf
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

启动服务：

```bash
sudo systemctl daemon-reload
sudo systemctl start storageserver
sudo systemctl enable storageserver
sudo systemctl status storageserver
```

#### 5. 注册StorageServer到MetaServer

在MySQL中插入StorageServer记录：

```sql
INSERT INTO t_storage_server (server_name, ip_address, port, status, capacity_total, capacity_used, weight)
VALUES ('StorageServer1', 'storage_server_ip', 9001, 1, 107374182400, 0, 1);
```

## 客户端部署

### Windows客户端

#### 1. 构建项目

```cmd
Scripts\build.bat
```

#### 2. 打包客户端

```cmd
Scripts\deploy.bat
```

#### 3. 安装客户端

1. 将 `deploy` 目录复制到目标机器
2. 运行 `deploy\bin\Client.exe`
3. 配置 `Config\client.conf`

### Linux/macOS客户端

#### 1. 构建项目

```bash
./Scripts/build.sh
```

#### 2. 打包客户端

```bash
./Scripts/deploy.sh
```

#### 3. 安装客户端

```bash
# 复制到目标目录
cp -r deploy /opt/chatfiletransfer/client

# 运行客户端
/opt/chatfiletransfer/client/bin/Client
```

## 生产环境部署

### 高可用部署

#### 多MetaServer部署

使用负载均衡器（如Nginx）部署多个MetaServer实例：

```nginx
upstream metaserver {
    server metaserver1:8000;
    server metaserver2:8000;
    server metaserver3:8000;
}

server {
    listen 80;
    server_name chat.example.com;

    location / {
        proxy_pass http://metaserver;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

#### 多StorageServer部署

部署多个StorageServer实例，并在MySQL中注册：

```sql
INSERT INTO t_storage_server (server_name, ip_address, port, status, capacity_total, capacity_used, weight)
VALUES 
('StorageServer1', '192.168.1.101', 9001, 1, 107374182400, 0, 1),
('StorageServer2', '192.168.1.102', 9002, 1, 107374182400, 0, 1),
('StorageServer3', '192.168.1.103', 9003, 1, 107374182400, 0, 1);
```

### 数据库主从复制

#### 配置MySQL主从复制

**主库配置** (`/etc/mysql/mysql.conf.d/mysqld.cnf`):

```ini
[mysqld]
server-id = 1
log-bin = mysql-bin
binlog-format = ROW
binlog-do-db = chat_file_transfer
```

**从库配置** (`/etc/mysql/mysql.conf.d/mysqld.cnf`):

```ini
[mysqld]
server-id = 2
relay-log = mysql-relay-bin
read-only = 1
```

**创建复制用户**（在主库上执行）：

```sql
CREATE USER 'repl'@'%' IDENTIFIED BY 'replication_password';
GRANT REPLICATION SLAVE ON *.* TO 'repl'@'%';
FLUSH PRIVILEGES;
```

**启动复制**（在从库上执行）：

```sql
CHANGE MASTER TO
  MASTER_HOST='master_ip',
  MASTER_USER='repl',
  MASTER_PASSWORD='replication_password',
  MASTER_LOG_FILE='mysql-bin.000001',
  MASTER_LOG_POS=154;

START SLAVE;
```

### Redis哨兵模式

#### 配置Redis哨兵

创建 `/etc/redis/sentinel.conf`：

```ini
port 26379
sentinel monitor mymaster 127.0.0.1 6379 2
sentinel down-after-milliseconds mymaster 5000
sentinel parallel-syncs mymaster 1
sentinel failover-timeout mymaster 10000
```

启动哨兵：

```bash
redis-sentinel /etc/redis/sentinel.conf
```

## 监控与维护

### 日志管理

#### 日志位置

- **MetaServer**: `/opt/chatfiletransfer/metaserver/logs/metaserver.log`
- **StorageServer**: `/opt/chatfiletransfer/storageserver/logs/storageserver.log`
- **MySQL**: `/var/log/mysql/`
- **Redis**: `/var/log/redis/`

#### 日志轮转

创建 `/etc/logrotate.d/chatfiletransfer`：

```
/opt/chatfiletransfer/metaserver/logs/*.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
    create 0640 chatapp chatapp
}

/opt/chatfiletransfer/storageserver/logs/*.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
    create 0640 chatapp chatapp
}
```

### 性能监控

#### 使用Prometheus + Grafana

1. 安装Prometheus:

```bash
sudo apt install -y prometheus
```

2. 配置Prometheus (`/etc/prometheus/prometheus.yml`):

```yaml
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'metaserver'
    static_configs:
      - targets: ['localhost:8000']
  - job_name: 'storageserver'
    static_configs:
      - targets: ['localhost:9001']
```

3. 安装Grafana:

```bash
sudo apt install -y grafana
sudo systemctl start grafana-server
sudo systemctl enable grafana-server
```

### 备份策略

#### 数据库备份

创建备份脚本 `/opt/chatfiletransfer/scripts/backup_db.sh`：

```bash
#!/bin/bash
BACKUP_DIR="/opt/chatfiletransfer/backups"
DATE=$(date +%Y%m%d_%H%M%S)
mysqldump -u chatapp -p'your_password' chat_file_transfer > $BACKUP_DIR/chat_file_transfer_$DATE.sql
find $BACKUP_DIR -name "chat_file_transfer_*.sql" -mtime +7 -delete
```

添加到crontab：

```bash
# 每天凌晨2点备份
0 2 * * * /opt/chatfiletransfer/scripts/backup_db.sh
```

#### 文件备份

```bash
#!/bin/bash
BACKUP_DIR="/opt/chatfiletransfer/backups/storage"
DATE=$(date +%Y%m%d_%H%M%S)
rsync -avz --delete /opt/chatfiletransfer/storageserver/storage/ $BACKUP_DIR/storage_$DATE/
find $BACKUP_DIR -type d -name "storage_*" -mtime +7 -exec rm -rf {} \;
```

### 故障排查

#### MetaServer无法启动

1. 检查日志：`journalctl -u metaserver -n 50`
2. 检查配置文件：`cat /opt/chatfiletransfer/metaserver/config/metaserver.conf`
3. 检查数据库连接：`mysql -u chatapp -p -h localhost`
4. 检查Redis连接：`redis-cli ping`

#### StorageServer无法启动

1. 检查日志：`journalctl -u storageserver -n 50`
2. 检查配置文件：`cat /opt/chatfiletransfer/storageserver/config/storageserver.conf`
3. 检查磁盘空间：`df -h`
4. 检查端口占用：`netstat -tlnp | grep 9001`

#### 客户端无法连接

1. 检查网络连接：`ping metaserver_ip`
2. 检查防火墙规则：`sudo iptables -L`
3. 检查MetaServer状态：`sudo systemctl status metaserver`

## 安全建议

### 网络安全

1. 使用防火墙限制访问：
   ```bash
   sudo ufw allow from 192.168.1.0/24 to any port 8000
   sudo ufw allow from 192.168.1.0/24 to any port 9001
   ```

2. 使用SSL/TLS加密通信

3. 定期更新系统和依赖

### 数据安全

1. 使用强密码
2. 定期备份数据
3. 限制数据库用户权限
4. 使用Redis密码认证

### 应用安全

1. 定期更新应用
2. 监控异常访问
3. 实施访问控制

## 获取帮助

如果遇到部署问题，请：

1. 查看本文档的[故障排查](#故障排查)部分
2. 查看项目Issues: https://github.com/yourusername/ChatAndFileTransferSystem/issues
3. 提交新的Issue，包含以下信息：
   - 操作系统版本
   - 硬件配置
   - 错误日志
   - 配置文件内容（敏感信息已脱敏）

## 下一步

部署成功后，请查看[使用指南](README.md)了解如何使用系统。
