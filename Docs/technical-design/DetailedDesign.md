# 聊天与文件传输系统重构技术详细设计文档

## 1. 项目目录结构设计

```
ChatAndFileTransferSystem/
├── Common/                           # 公共协议/工具库
│   ├── Protocol/                     # 通信协议定义
│   │   ├── TransHeader.h            # 定长包头结构（包含版本号）
│   │   ├── CommandTypes.h           # 命令字枚举
│   │   ├── ProtocolBodies.h         # 协议包体结构（包含安全字段）
│   │   ├── ClientProtocol.h         # 客户端-服务器协议
│   │   ├── InternalProtocol.h       # 内部RPC协议
│   │   ├── CommonProtocol.h        # 通用协议
│   │   └── CMakeLists.txt
│   ├── Utils/                       # 通用工具类
│   │   ├── HashUtil.h               # MD5/SHA256计算
│   │   ├── StringUtil.h             # 字符串处理（包含安全拷贝函数）
│   │   ├── FileUtil.h              # 文件操作工具
│   │   ├── TimeUtil.h              # 时间处理工具
│   │   ├── PathUtil.h              # 路径处理工具
│   │   ├── MemoryPool.h            # 内存池
│   │   ├── Logger.h/.cpp           # 日志系统
│   │   └── CMakeLists.txt
│   └── CMakeLists.txt
├── MetaServer/                       # Qt6控制层
│   ├── Core/                        # 核心业务逻辑
│   │   ├── MetaServer.h/.cpp        # 主服务器类
│   │   ├── AuthModule.h/.cpp        # 认证模块
│   │   ├── LoadBalancer.h/.cpp       # 负载均衡器
│   │   └── FileMetaMgr.h/.cpp       # 文件元数据管理
│   ├── Database/                     # 数据库操作
│   │   ├── DatabaseManager.h/.cpp    # 数据库连接管理
│   │   ├── UserDAO.h/.cpp          # 用户数据访问对象
│   │   ├── FileDAO.h/.cpp          # 文件数据访问对象
│   │   └── CMakeLists.txt
│   ├── Network/                     # 网络通信
│   │   ├── TcpServer.h/.cpp         # Qt6原生TCP服务器
│   │   ├── ClientConnection.h/.cpp   # 客户端连接
│   │   └── CMakeLists.txt
│   ├── Cache/                       # 缓存管理
│   │   ├── RedisManager.h/.cpp       # Redis连接管理（支持异步操作）
│   │   └── CMakeLists.txt
│   ├── UI/                          # 用户界面
│   │   ├── ServerWindow.h/.cpp/.ui   # 服务器监控界面
│   │   └── CMakeLists.txt
│   ├── Config/                      # 配置管理
│   │   ├── ServerConfig.h/.cpp       # 服务器配置
│   │   └── CMakeLists.txt
│   ├── main.cpp                     # 主函数入口
│   └── CMakeLists.txt
├── StorageServer/                    # C++ Qt6数据层
│   ├── Core/                        # 核心业务逻辑
│   │   ├── StorageServer.h/.cpp      # 主存储服务器
│   │   ├── TokenValidator.h/.cpp     # Token验证器
│   │   └── StorageEngine.h/.cpp     # 存储引擎（包含路径遍历防护）
│   ├── Network/                     # 网络通信
│   │   ├── TcpServer.h/.cpp         # Qt6原生TCP服务器
│   │   ├── Connection.h/.cpp         # 连接管理
│   │   ├── ConnectionPool.h/.cpp     # 连接池
│   │   └── CMakeLists.txt
│   ├── IO/                          # IO操作
│   │   ├── FileReader.h/.cpp         # 文件读取
│   │   ├── FileWriter.h/.cpp         # 文件写入
│   │   └── CMakeLists.txt
│   ├── RPC/                         # 内部RPC通信
│   │   ├── RPCClient.h/.cpp          # RPC客户端(向Meta汇报)
│   │   ├── RPCServer.h/.cpp          # RPC服务器(接收Meta指令)
│   │   └── CMakeLists.txt
│   ├── Config/                      # 配置管理
│   │   ├── StorageConfig.h/.cpp       # 存储服务器配置
│   │   └── CMakeLists.txt
│   ├── main.cpp                     # 主函数入口
│   └── CMakeLists.txt
├── Client/                          # Qt6客户端
│   ├── Core/                        # 核心业务逻辑
│   │   ├── ClientCore.h/.cpp         # 客户端核心
│   │   ├── TransferManager.h/.cpp     # 传输管理器
│   │   └── FileWorker.h/.cpp        # 文件传输线程
│   ├── Network/                     # 网络通信
│   │   ├── NetworkManager.h/.cpp     # 网络管理器
│   │   ├── MetaConnection.h/.cpp      # Meta服务器连接
│   │   ├── StorageConnection.h/.cpp   # Storage服务器连接
│   │   └── CMakeLists.txt
│   ├── UI/                          # 用户界面
│   │   ├── MainWindow.h/.cpp/.ui    # 主窗口
│   │   ├── LoginDialog.h/.cpp/.ui    # 登录对话框
│   │   ├── ChatWidget.h/.cpp/.ui     # 聊天界面
│   │   ├── FileWidget.h/.cpp/.ui     # 文件管理界面
│   │   └── CMakeLists.txt
│   ├── Models/                      # 数据模型
│   │   ├── UserModel.h/.cpp         # 用户模型
│   │   ├── FileModel.h/.cpp         # 文件模型
│   │   └── CMakeLists.txt
│   ├── Utils/                       # 客户端工具
│   │   ├── ConfigManager.h/.cpp       # 配置管理
│   │   └── CMakeLists.txt
│   ├── main.cpp                     # 主函数入口
│   └── CMakeLists.txt
├── Tests/                           # 测试代码
│   ├── UnitTests/                   # 单元测试
│   ├── IntegrationTests/            # 集成测试
│   └── CMakeLists.txt
├── Docs/                            # 文档
│   ├── technical-design/            # 技术设计文档
│   ├── development/                # 开发文档
│   ├── optimization/               # 优化文档
│   ├── README.md                   # 文档索引
│   ├── DEPLOYMENT.md               # 部署文档
│   └── DOCUMENTATION_GUIDELINES.md  # 文档规范
├── CMakeLists.txt                   # 主CMake文件（CMake 3.16+）
└── README.md                        # 项目说明
```

---

## 2. 构建系统设计 (CMake + Qt6)

### 2.1 构建系统特性

- **构建工具**: CMake 3.16+
- **C++标准**: C++17
- **Qt版本**: Qt 6.x
- **跨平台支持**: Windows、Linux、macOS
- **静态库**: Common、Protocol、Utils
- **动态库**: 无（全部静态链接）
- **可执行文件**: Client、MetaServer、StorageServer

### 2.2 CMake 配置说明

#### 根目录 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(ChatAndFileTransferSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Sql)

add_subdirectory(Common)
add_subdirectory(MetaServer)
add_subdirectory(StorageServer)
add_subdirectory(Client)
```

#### Common/CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(Common VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Protocol)
add_subdirectory(Utils)

add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Protocol Utils)
```

#### Protocol/CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(Protocol)

set(HEADERS
    TransHeader.h
    CommandTypes.h
    ProtocolBodies.h
    ClientProtocol.h
    InternalProtocol.h
    CommonProtocol.h
)

add_library(${PROJECT_NAME} STATIC ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

#### Utils/CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(Utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

set(HEADERS
    HashUtil.h
    StringUtil.h
    FileUtil.h
    TimeUtil.h
    PathUtil.h
    MemoryPool.h
    Logger.h
)

set(SOURCES
    PathUtil.cpp
    MemoryPool.cpp
    StringUtil.cpp
    Logger.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core)
```

---

## 3. 网络层设计 (Qt6 原生网络)

### 3.1 网络层架构

- **MetaServer**: 使用 Qt6 原生 `QTcpServer`
- **StorageServer**: 使用 Qt6 原生 `QTcpServer`
- **Client**: 使用 Qt6 原生 `QTcpSocket`

### 3.2 Qt6 原生网络优势

- **跨平台**: Windows 上自动使用 IOCP，Linux 上自动使用 Epoll
- **高性能**: 零拷贝、事件驱动、非阻塞 I/O
- **易用性**: 信号槽机制、自动内存管理
- **可维护性**: 统一的 API、减少平台相关代码

### 3.3 TcpServer 实现

```cpp
class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(QObject* parent = nullptr);
    bool start(quint16 port);
    void stop();
    bool isRunning() const;
    int getConnectionCount() const;
    
signals:
    void newConnection(qintptr socketDescriptor);
    void connectionClosed(qintptr socketDescriptor);
    void messageReceived(qintptr socketDescriptor, const TransHeader& header, const QByteArray& data);
    
protected:
    void incomingConnection(qintptr socketDescriptor) override;
    
private:
    QMap<qintptr, Connection*> m_connections;
    QMutex m_mutex;
    QAtomicInt m_connectionCount;
    QAtomicInt m_running;
};
```

---

## 4. 安全机制设计

### 4.1 路径遍历防护

在 `StorageEngine` 中实现路径遍历防护：

```cpp
std::string StorageEngine::sanitizeFilePath(const std::string& filePath) const
{
    std::string safePath = filePath;
    
    // 移除所有 ".."
    size_t pos = 0;
    while ((pos = safePath.find("..", pos)) != std::string::npos) {
        safePath.replace(pos, 2, "");
        pos += 2;
    }
    
    // 统一路径分隔符
    std::replace(safePath.begin(), safePath.end(), '\\', '/');
    
    // 移除前导的路径分隔符和点
    while (!safePath.empty() && (safePath[0] == '/' || safePath[0] == '.')) {
        safePath = safePath.substr(1);
    }
    
    return safePath;
}
```

### 4.2 缓冲区溢出防护

在 `StringUtil` 中实现安全拷贝函数：

```cpp
static void safeCopy(char* dest, const std::string& src, size_t destSize)
{
    if (destSize == 0) {
        return;
    }
    
    // 确保最多拷贝 destSize - 1 个字符
    size_t length = std::min(src.size(), destSize - 1);
    std::memcpy(dest, src.c_str(), length);
    dest[length] = '\0'; // 强制结束符
}
```

### 4.3 协议安全字段

在 `TransHeader` 中添加版本号：

```cpp
struct TransHeader {
    uint32_t magic;        // 魔数 (固定值: 0x12345678)
    uint32_t version;      // 协议版本号 (当前版本: 1)
    uint32_t cmd;          // 命令字
    uint32_t seq;          // 序列号
    uint32_t len;          // 包体长度
    uint32_t checksum;     // 校验和
    uint32_t reserved;     // 保留字段
};
```

在 `UploadRes` 中添加秒传标志：

```cpp
struct UploadRes {
    uint32_t result_code;      // 结果码
    uint64_t file_id;         // 文件ID
    uint8_t is_instant_upload; // 是否秒传（0-否，1-是）
    char upload_token[64];     // 上传令牌
    char storage_ip[45];       // 存储服务器IP
    uint16_t storage_port;    // 存储服务器端口
    uint32_t chunk_size;      // 建议的分片大小
    uint32_t expire_time;     // Token过期时间
    char error_msg[256];      // 错误信息
};
```

在 `DownloadReq` 中添加用户ID：

```cpp
struct DownloadReq {
    uint64_t user_id;         // 用户ID（用于权限验证）
    uint64_t file_id;         // 文件ID
    uint32_t chunk_size;      // 请求的分片大小
};
```

---

## 5. 并发与性能优化设计

### 5.1 Redis 异步化

在 `RedisManager` 中实现异步操作：

```cpp
class RedisManager : public QObject {
    Q_OBJECT
public:
    // 同步方法
    bool set(const std::string& key, const std::string& value, int expire = 0);
    bool get(const std::string& key, std::string& value);
    
    // 异步方法
    QFuture<bool> setAsync(const std::string& key, const std::string& value, int expire = 0);
    QFuture<std::pair<bool, std::string>> getAsync(const std::string& key);
    QFuture<bool> delAsync(const std::string& key);
    QFuture<bool> existsAsync(const std::string& key);
};
```

异步方法使用 `QtConcurrent::run` 实现：

```cpp
QFuture<bool> RedisManager::setAsync(const std::string& key, const std::string& value, int expire)
{
    return QtConcurrent::run([this, key, value, expire]() -> bool {
        return set(key, value, expire);
    });
}
```

### 5.2 使用 QFutureWatcher 监听异步结果

```cpp
void MetaServer::handleUploadRequest(...) {
    auto future = m_redisManager->validateTokenAsync(token);
    auto watcher = new QFutureWatcher<bool>();
    
    connect(watcher, &QFutureWatcher<bool>::finished, [this, watcher, socketDescriptor]() {
        if (watcher->result()) {
            // Token 有效，继续处理
        } else {
            // Token 无效，发送错误
        }
        watcher->deleteLater();
    });
    
    watcher->setFuture(future);
}
```

---

## 6. 日志系统设计

### 6.1 日志系统特性

- **日志级别**: Debug、Info、Warning、Error、Fatal
- **输出目标**: 控制台、文件
- **日志格式**: `[时间] [级别] [线程ID] [文件:行号] 消息`
- **线程安全**: 使用 QMutex 保护文件写入
- **自动初始化**: 使用 `qInstallMessageHandler` 全局接管日志

### 6.2 Logger 实现

```cpp
class Logger {
public:
    static void initialize(const QString& logFilePath = "");
    static void setLogLevel(LogLevel level);
    static LogLevel getLogLevel();
    
    static void debug(const QString& msg);
    static void info(const QString& msg);
    static void warning(const QString& msg);
    static void error(const QString& msg);
    static void fatal(const QString& msg);
    
private:
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    static QString formatMessage(LogLevel level, const QString& msg, const QMessageLogContext& context);
    static QString levelToString(LogLevel level);
};
```

### 6.3 使用示例

```cpp
int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    
    // 初始化日志系统
    Logger::initialize("application.log");
    Logger::setLogLevel(LogLevel::Info);
    
    // 使用日志
    Logger::info("应用程序启动");
    Logger::error("发生错误: " + errorMsg);
    
    return a.exec();
}
```

---

## 7. 数据库与缓存设计 (Schema Design)

### 7.1 MySQL 表设计 (Meta Server)

#### 用户表 (t_user)
```sql
CREATE TABLE `t_user` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '用户ID',
  `username` VARCHAR(64) NOT NULL COMMENT '用户名',
  `password_hash` VARCHAR(128) NOT NULL COMMENT '密码哈希',
  `salt` VARCHAR(32) NOT NULL COMMENT '密码盐值',
  `nickname` VARCHAR(64) DEFAULT '' COMMENT '昵称',
  `avatar_url` VARCHAR(255) DEFAULT '' COMMENT '头像URL',
  `status` TINYINT NOT NULL DEFAULT 0 COMMENT '状态：0-离线，1-在线，2-隐身',
  `create_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `update_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `uk_username` (`username`),
  KEY `idx_status` (`status`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='用户表';
```

#### 逻辑文件表 (t_file_info)
```sql
CREATE TABLE `t_file_info` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '文件ID',
  `user_id` BIGINT UNSIGNED NOT NULL COMMENT '用户ID',
  `file_name` VARCHAR(255) NOT NULL COMMENT '文件名',
  `file_path` VARCHAR(512) NOT NULL COMMENT '文件路径(相对路径)',
  `file_size` BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '文件大小(字节)',
  `file_hash` VARCHAR(64) DEFAULT '' COMMENT '文件哈希值(MD5)',
  `file_type` TINYINT NOT NULL DEFAULT 0 COMMENT '文件类型：0-文件，1-目录',
  `parent_id` BIGINT UNSIGNED DEFAULT 0 COMMENT '父目录ID，0表示根目录',
  `is_deleted` TINYINT NOT NULL DEFAULT 0 COMMENT '是否已删除：0-未删除，1-已删除',
  `create_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `update_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_user_id` (`user_id`),
  KEY `idx_parent_id` (`parent_id`),
  KEY `idx_file_hash` (`file_hash`),
  FOREIGN KEY (`user_id`) REFERENCES `t_user` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='逻辑文件表';
```

#### 物理文件表 (t_file_store)
```sql
CREATE TABLE `t_file_store` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '物理文件ID',
  `file_hash` VARCHAR(64) NOT NULL COMMENT '文件哈希值(MD5)',
  `file_size` BIGINT UNSIGNED NOT NULL COMMENT '文件大小(字节)',
  `storage_path` VARCHAR(512) NOT NULL COMMENT '物理存储路径',
  `storage_server_id` INT UNSIGNED NOT NULL COMMENT '存储服务器ID',
  `ref_count` INT UNSIGNED NOT NULL DEFAULT 1 COMMENT '引用计数',
  `first_upload_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '首次上传时间',
  `last_access_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后访问时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `uk_file_hash` (`file_hash`),
  KEY `idx_storage_server` (`storage_server_id`),
  KEY `idx_ref_count` (`ref_count`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='物理文件表';
```

#### 存储服务器表 (t_storage_server)
```sql
CREATE TABLE `t_storage_server` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '服务器ID',
  `server_name` VARCHAR(64) NOT NULL COMMENT '服务器名称',
  `ip_address` VARCHAR(45) NOT NULL COMMENT 'IP地址',
  `port` SMALLINT UNSIGNED NOT NULL COMMENT '端口',
  `status` TINYINT NOT NULL DEFAULT 0 COMMENT '状态：0-离线，1-在线，2-维护中',
  `capacity_total` BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '总容量(字节)',
  `capacity_used` BIGINT UNSIGNED NOT NULL DEFAULT 0 COMMENT '已用容量(字节)',
  `weight` INT UNSIGNED NOT NULL DEFAULT 1 COMMENT '负载均衡权重',
  `create_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `update_time` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `uk_ip_port` (`ip_address`, `port`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='存储服务器表';
```

### 7.2 Redis Key 设计

#### 上传Token
```
Key格式: upload_token:{token}
Value结构: JSON {
    "user_id": 12345,
    "file_hash": "d41d8cd98f00b204e9800998ecf8427e",
    "file_size": 1048576,
    "file_name": "example.txt",
    "expire_time": 1640995200
}
TTL: 1小时 (3600秒)
```

#### Storage Server状态上报
```
Key格式: storage_status:{server_id}
Value结构: JSON {
    "status": "online",
    "cpu_usage": 45.2,
    "memory_usage": 68.5,
    "disk_usage": 78.9,
    "network_io": 1024000,
    "active_connections": 256,
    "last_heartbeat": 1640995200
}
TTL: 5分钟 (300秒)
```

#### 用户会话信息
```
Key格式: user_session:{user_id}
Value结构: JSON {
    "login_time": 1640995200,
    "last_active": 1640998800,
    "client_ip": "192.168.1.100",
    "token": "abc123def456"
}
TTL: 24小时 (86400秒)
```

#### 在线用户列表
```
Key格式: online_users
Value结构: SET {
    "user_id:12345",
    "user_id:12346",
    ...
}
TTL: 无限制
```

---

## 8. 通信协议规范 (Protocol Specification)

### 8.1 定长包头结构

```cpp
#pragma pack(push, 1)

struct TransHeader {
    uint32_t magic;        // 魔数，用于校验包的完整性 (固定值: 0x12345678)
    uint32_t version;      // 协议版本号 (当前版本: 1)
    uint32_t cmd;          // 命令字，对应CommandType枚举
    uint32_t seq;          // 序列号，用于请求响应匹配
    uint32_t len;          // 包体长度(不包含包头)
    uint32_t checksum;     // 校验和，用于验证包完整性
    uint32_t reserved;     // 保留字段，未来扩展
};

enum CommandType : uint32_t {
    // 控制平面命令 (Client <-> Meta Server)
    CMD_LOGIN_REQ = 0x1001,
    CMD_LOGIN_RES = 0x1002,
    CMD_LOGOUT_REQ = 0x1003,
    CMD_LOGOUT_RES = 0x1004,
    
    CMD_CHAT_REQ = 0x1011,
    CMD_CHAT_RES = 0x1012,
    CMD_CHAT_NOTIFY = 0x1013,
    
    CMD_UPLOAD_REQ = 0x1021,
    CMD_UPLOAD_RES = 0x1022,
    CMD_DOWNLOAD_REQ = 0x1023,
    CMD_DOWNLOAD_RES = 0x1024,
    
    CMD_FILE_LIST_REQ = 0x1031,
    CMD_FILE_LIST_RES = 0x1032,
    CMD_DELETE_FILE_REQ = 0x1033,
    CMD_DELETE_FILE_RES = 0x1034,
    CMD_RENAME_FILE_REQ = 0x1035,
    CMD_RENAME_FILE_RES = 0x1036,
    
    // 数据平面命令 (Client <-> Storage Server)
    CMD_UPLOAD_CHUNK_REQ = 0x2001,
    CMD_UPLOAD_CHUNK_RES = 0x2002,
    CMD_DOWNLOAD_CHUNK_REQ = 0x2003,
    CMD_DOWNLOAD_CHUNK_RES = 0x2004,
    
    // 内部RPC命令 (Meta Server <-> Storage Server)
    CMD_INTERNAL_HEARTBEAT = 0x3001,
    CMD_INTERNAL_STATUS_REPORT = 0x3002,
    CMD_INTERNAL_UPLOAD_COMPLETE = 0x3003,
    CMD_INTERNAL_DELETE_FILE = 0x3004,
};

#pragma pack(pop)
```

### 8.2 核心包体结构

```cpp
struct LoginReq {
    char username[64];
    char password_hash[128];
    char client_version[16];
};

struct LoginRes {
    uint32_t result_code;
    uint64_t user_id;
    char nickname[64];
    char session_token[64];
    char error_msg[256];
};

struct UploadReq {
    uint64_t user_id;
    char file_name[256];
    uint64_t file_size;
    char file_hash[64];
    char parent_path[512];
    uint32_t chunk_size;
};

struct UploadRes {
    uint32_t result_code;
    uint64_t file_id;
    uint8_t is_instant_upload;
    char upload_token[64];
    char storage_ip[45];
    uint16_t storage_port;
    uint32_t chunk_size;
    uint32_t expire_time;
    char error_msg[256];
};

struct DownloadReq {
    uint64_t user_id;
    uint64_t file_id;
    uint32_t chunk_size;
};

struct DownloadRes {
    uint32_t result_code;
    uint64_t file_size;
    char file_hash[64];
    char storage_ip[45];
    uint16_t storage_port;
    uint32_t chunk_size;
    char error_msg[256];
};
```

---

## 9. 核心类结构设计 (Class Hierarchy)

### 9.1 StorageServer 核心类

```cpp
class StorageEngine : public QObject {
    Q_OBJECT
public:
    explicit StorageEngine(QObject* parent = nullptr);
    ~StorageEngine();
    
    bool setBasePath(const std::string& basePath);
    bool createFile(const std::string& filePath, uint64_t fileSize);
    bool writeChunk(const std::string& filePath, uint32_t chunkIndex, const QByteArray& chunkData);
    QByteArray readChunk(const std::string& filePath, uint32_t chunkIndex, uint32_t chunkSize);
    bool deleteFile(const std::string& filePath);
    bool fileExists(const std::string& filePath) const;
    uint64_t getFileSize(const std::string& filePath) const;
    std::string calculateFileHash(const std::string& filePath) const;
    
signals:
    void writeProgress(const std::string& filePath, uint32_t chunkIndex, uint32_t progress);
    
private:
    static std::string sanitizeFilePath(const std::string& filePath) const;
    bool ensureDirectoryExists(const std::string& dirPath) const;
    
private:
    std::string m_basePath;
    std::mutex m_mutex;
};
```

### 9.2 MetaServer 核心类

```cpp
class MetaServer : public QObject {
    Q_OBJECT
public:
    explicit MetaServer(QObject* parent = nullptr);
    ~MetaServer();
    
    bool start(const QString& configPath);
    void stop();
    
private slots:
    void onNewConnection(qintptr socketDescriptor);
    void onMessageReceived(qintptr socketDescriptor, const TransHeader& header, const QByteArray& data);
    void onConnectionClosed(qintptr socketDescriptor);
    
private:
    void handleLoginRequest(qintptr socketDescriptor, const QByteArray& data);
    void handleUploadRequest(qintptr socketDescriptor, const QByteArray& data);
    void handleDownloadRequest(qintptr socketDescriptor, const QByteArray& data);
    
private:
    Network::TcpServer* m_tcpServer;
    Cache::RedisManager* m_redisManager;
    Database::DatabaseManager* m_dbManager;
    Core::LoadBalancer* m_loadBalancer;
    Core::FileMetaMgr* m_fileMetaMgr;
};
```

---

## 10. 部署与运维

### 10.1 构建命令

```bash
# Windows
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release

# Linux
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 10.2 运行命令

```bash
# 启动 MetaServer
./bin/MetaServer config/meta_server.conf

# 启动 StorageServer
./bin/StorageServer config/storage_server.conf

# 启动 Client
./bin/Client
```

### 10.3 日志查看

```bash
# 查看日志文件
tail -f logs/meta_server.log
tail -f logs/storage_server.log
tail -f logs/client.log
```

---

## 11. 总结

本文档详细描述了聊天与文件传输系统的重构设计，主要包含以下内容：

1. **构建系统**: 从 QMake 迁移到 CMake，支持 Qt6
2. **网络层**: 使用 Qt6 原生网络模块，实现跨平台
3. **安全机制**: 添加路径遍历防护和缓冲区溢出修复
4. **并发优化**: Redis 异步化，避免阻塞主线程
5. **日志系统**: 实现统一的日志输出，便于调试和监控
6. **数据库设计**: MySQL + Redis 双存储架构
7. **协议设计**: 定长包头 + 柔性包体，支持版本控制

所有代码遵循 C++17 标准和 Qt6 规范，确保代码质量和可维护性。
