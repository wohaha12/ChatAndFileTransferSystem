# 项目架构图设计

---

## 1. 整体架构图

```mermaid
graph TD
    %% =======================
    %% 客户端区域
    %% =======================
    subgraph Client_Side [Qt6 客户端]
        UI[UI 界面层]
        Logic[业务逻辑层]
        TransferMgr[传输管理器]
        FileWorker[文件传输线程]
        LocalDisk[本地磁盘]
        
        %% 客户端内部连接
        UI -->|信号/槽| Logic
        Logic -->|任务调度| TransferMgr
        TransferMgr -->|管理| FileWorker
        FileWorker -.->|read/seek| LocalDisk
    end

    %% =======================
    %% 网络连接层
    %% =======================
    MetaSocket((TCP Socket<br>控制连接))
    DataSocket((TCP Socket<br>数据连接))
    InternalNet{内部 RPC<br>高速通道}

    Logic <--> MetaSocket
    FileWorker <--> DataSocket

    %% =======================
    %% 服务端 - 控制平面
    %% =======================
    subgraph Control_Plane [Meta Server Cluster]
        MetaCore[业务逻辑核心]
        Auth[鉴权模块]
        LoadBalancer[负载均衡器<br>调度核心]
        DB_Pool[数据库连接池]
        
        %% 存储组件
        MySQL[(MySQL<br>元数据/用户)]
        Redis[(Redis<br>Token/热点缓存)]
        
        %% 控制面连接
        MetaCore --> Auth
        MetaCore --> LoadBalancer
        MetaCore --> DB_Pool
        MetaCore --> Redis
        DB_Pool --> MySQL
    end

    %% =======================
    %% 服务端 - 数据平面
    %% =======================
    subgraph Data_Plane [Storage Server Cluster]
        StorageCore[Storage Server<br>Qt6 原生网络]
        IO_Engine[IO 读写引擎]
        TokenCheck[Token 校验]
        
        StorageCore --> IO_Engine
        IO_Engine -->|pwrite/sendfile| ServerDisk[服务器磁盘阵列]
    end

    %% =======================
    %% 交互流程
    %% =======================
    %% 1. 控制流
    MetaSocket <-->|1. 登录/申请上传| MetaCore
    
    %% 2. 调度流 (LB介入)
    LoadBalancer -.->|2. 选择最佳 Storage IP| MetaCore
    
    %% 3. 数据流
    DataSocket -->|3. 携带Token连接| TokenCheck
    TokenCheck --> StorageCore
    
    %% 4. 内部汇报流 (解耦关键)
    StorageCore -->|4. 汇报上传结果| InternalNet
    InternalNet -->|TCP/Protobuf| MetaCore

    %% 样式定义
    classDef client fill:#e3f2fd,stroke:#1565c0,stroke-width:2px;
    classDef control fill:#fff8e1,stroke:#ff6f00,stroke-width:2px;
    classDef data fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px;
    classDef storage fill:#f3e5f5,stroke:#7b1fa2,stroke-width:2px;
    classDef net fill:#eceff1,stroke:#546e7a,stroke-width:2px,stroke-dasharray: 5 5;

    class UI,Logic,TransferMgr,FileWorker client;
    class MetaCore,Auth,LoadBalancer,DB_Pool control;
    class StorageCore,IO_Engine,TokenCheck data;
    class MySQL,Redis,ServerDisk,LocalDisk storage;
    class InternalNet net;
```

---

## 2. 架构说明

### 2.1 客户端架构

- **UI 界面层**: 使用 Qt6 Widgets 实现用户界面
- **业务逻辑层**: 处理用户操作和业务逻辑
- **传输管理器**: 管理文件上传和下载任务
- **文件传输线程**: 执行文件传输操作
- **本地磁盘**: 存储本地文件

### 2.2 Meta Server 架构

- **业务逻辑核心**: 处理客户端请求和业务逻辑
- **鉴权模块**: 验证用户身份和权限
- **负载均衡器**: 调度存储服务器，选择最佳节点
- **数据库连接池**: 管理 MySQL 数据库连接
- **MySQL**: 存储元数据和用户信息
- **Redis**: 缓存 Token 和热点数据

### 2.3 Storage Server 架构

- **Qt6 原生网络**: 使用 Qt6 原生网络模块（QTcpServer）
  - Windows 上自动使用 IOCP
  - Linux 上自动使用 Epoll
  - macOS 上自动使用 kqueue
- **IO 读写引擎**: 处理文件读写操作
- **Token 校验**: 验证上传和下载 Token
- **服务器磁盘阵列**: 存储物理文件

---

## 3. 网络层架构

### 3.1 Qt6 原生网络优势

- **跨平台**: 自动适配不同平台的最佳 I/O 模型
- **高性能**: 零拷贝、事件驱动、非阻塞 I/O
- **易用性**: 信号槽机制、自动内存管理
- **可维护性**: 统一的 API、减少平台相关代码

### 3.2 网络连接

- **控制连接**: 客户端 <-> Meta Server
  - 用于登录、文件申请、聊天等控制命令
  - 使用 Qt6 原生 TCP Socket

- **数据连接**: 客户端 <-> Storage Server
  - 用于文件上传和下载
  - 使用 Qt6 原生 TCP Socket

- **内部 RPC**: Meta Server <-> Storage Server
  - 用于心跳、状态上报、文件操作通知
  - 使用 Qt6 原生 TCP Socket

---

## 4. 数据流架构

### 4.1 上传流程

1. 客户端向 Meta Server 申请上传
2. Meta Server 验证用户权限和文件哈希
3. 如果文件已存在，返回秒传信息
4. 如果文件不存在，选择最佳 Storage Server
5. 客户端连接到 Storage Server
6. Storage Server 验证 Token
7. 客户端上传文件分片
8. Storage Server 写入磁盘
9. Storage Server 向 Meta Server 汇报上传结果
10. Meta Server 更新数据库

### 4.2 下载流程

1. 客户端向 Meta Server 申请下载
2. Meta Server 验证用户权限
3. Meta Server 返回文件信息和 Storage Server 地址
4. 客户端连接到 Storage Server
5. Storage Server 验证用户权限
6. 客户端下载文件分片
7. Storage Server 读取磁盘
8. 客户端重组文件

---

## 5. 安全架构

### 5.1 认证与授权

- **用户认证**: 使用 SHA256 密码哈希 + 盐值
- **Token 验证**: 使用 Redis 缓存 Token，支持过期时间
- **权限验证**: 在下载时验证用户是否有权限访问文件

### 5.2 数据安全

- **路径遍历防护**: 在文件操作前清洗文件路径
- **缓冲区溢出防护**: 使用安全拷贝函数
- **协议版本验证**: 验证协议版本号，确保兼容性

### 5.3 传输安全

- **校验和验证**: 使用 CRC32 校验和验证数据完整性
- **分片传输**: 支持大文件分片上传和下载
- **断点续传**: 支持分片索引，支持断点续传

---

## 6. 性能优化架构

### 6.1 并发优化

- **Redis 异步化**: 使用 QtConcurrent::run 包装 Redis 操作
- **线程池**: 使用 QThreadPool 处理并发任务
- **连接池**: 使用数据库连接池减少连接开销

### 6.2 缓存优化

- **Redis 缓存**: 缓存 Token、热点数据、在线用户列表
- **秒传机制**: 基于文件哈希的秒传，减少重复上传

### 6.3 负载均衡

- **加权轮询**: 根据存储服务器权重选择节点
- **容量感知**: 根据存储服务器容量选择节点
- **健康检查**: 定期检查存储服务器状态

---

## 7. 日志与监控

### 7.1 日志系统

- **日志级别**: Debug、Info、Warning、Error、Fatal
- **输出目标**: 控制台、文件
- **日志格式**: `[时间] [级别] [线程ID] [文件:行号] 消息`
- **线程安全**: 使用 QMutex 保护文件写入

### 7.2 监控指标

- **服务器状态**: CPU、内存、磁盘、网络
- **连接数**: 当前连接数、历史连接数
- **传输统计**: 上传/下载速度、成功率、失败率

---

## 8. 总结

本文档详细描述了聊天与文件传输系统的架构设计，主要包含以下内容：

1. **整体架构**: 客户端、Meta Server、Storage Server 三层架构
2. **网络层**: 使用 Qt6 原生网络模块，实现跨平台
3. **数据流**: 上传和下载流程，支持秒传和断点续传
4. **安全架构**: 认证授权、数据安全、传输安全
5. **性能优化**: 并发优化、缓存优化、负载均衡
6. **日志监控**: 统一日志系统，便于调试和监控

所有架构设计遵循 C++17 标准和 Qt6 规范，确保代码质量和可维护性。
