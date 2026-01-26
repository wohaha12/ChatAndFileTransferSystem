# ChatAndFileTransferSystem

基于Qt的高性能网络通信与文件传输系统，采用控制平面与数据平面分离的分布式架构。

## 项目简介

本项目是一个现代化的分布式文件传输系统，采用双服务器架构（MetaServer + StorageServer），实现了实时聊天、文件传输、文件分享等功能。系统采用控制平面与数据平面分离的设计理念，支持多用户同时在线，提供了友好的图形界面。

## 功能特性

### 客户端功能
- ✅ 用户注册与登录
- ✅ 实时聊天功能
- ✅ 文件上传与下载（支持断点续传）
- ✅ 文件分享功能
- ✅ 好友管理
- ✅ 在线用户列表
- ✅ 文件秒传（基于哈希去重）
- ✅ 多界面切换
- ✅ 现代化UI设计（无QSS，纯C++实现）

### MetaServer功能
- ✅ 客户端连接管理
- ✅ 用户认证与会话管理
- ✅ 文件元数据管理
- ✅ 负载均衡（加权轮询算法）
- ✅ 消息转发与处理
- ✅ 服务器状态监控
- ✅ Redis缓存集成
- ✅ MySQL数据库连接池

### StorageServer功能
- ✅ Epoll高并发处理（Reactor模式）
- ✅ 文件分片传输
- ✅ Token验证机制
- ✅ 上传完成通知（RPC通信）
- ✅ 存储引擎优化
- ✅ 心跳机制

## 技术栈

### 开发框架
- **Qt 6.x**: Core, Gui, Widgets, Network, Sql
- **C++17**: 现代C++特性
- **CMake 3.16+**: 跨平台构建系统

### 后端技术
- **MySQL 8.0+**: 数据存储
- **Redis 6.0+**: 缓存与会话管理
- **Epoll**: Linux高性能IO多路复用
- **Reactor模式**: 事件驱动架构

### 数据库设计
- **InnoDB引擎**: 事务支持
- **连接池**: 优化数据库访问
- **索引优化**: 提升查询性能

## 项目结构

```
ChatAndFileTransferSystem/
├── Client/                    # Qt客户端
│   ├── Core/                 # 核心业务逻辑
│   ├── Network/               # 网络通信
│   ├── UI/                    # 用户界面
│   ├── Models/                # 数据模型
│   ├── Utils/                 # 客户端工具
│   └── images/                # 界面图片资源
├── Common/                    # 公共协议/工具库
│   ├── Protocol/               # 通信协议定义
│   └── Utils/                 # 通用工具类
├── MetaServer/                # Qt控制层
│   ├── Core/                  # 核心业务逻辑
│   ├── Database/               # 数据库操作
│   ├── Network/               # 网络通信
│   ├── Cache/                 # 缓存管理
│   ├── UI/                    # 用户界面
│   ├── Config/                # 配置管理
│   └── RPC/                   # 内部RPC通信
├── StorageServer/             # C++ Epoll数据层
│   ├── Core/                  # 核心业务逻辑
│   ├── Network/               # 网络通信（Epoll）
│   ├── IO/                    # IO操作
│   ├── RPC/                   # 内部RPC通信
│   └── Config/                # 配置管理
├── Tests/                     # 测试代码
├── Scripts/                   # 构建和部署脚本
├── Config/                    # 配置文件
├── Docs/                      # 文档
└── CMakeLists.txt             # 主CMake文件
```

## 环境要求

### 开发环境
- **操作系统**: Windows 10+, Linux (Ubuntu 20.04+), macOS 10.15+
- **编译器**: 
  - Windows: MinGW 7.3+ 或 MSVC 2019+
  - Linux: GCC 9+ 或 Clang 10+
  - macOS: Clang 12+
- **Qt版本**: Qt 6.2+ (推荐Qt 6.5+)
- **CMake版本**: 3.16+
- **CMake构建工具**: 
  - Windows: MinGW Makefiles 或 Ninja
  - Linux/macOS: Make 或 Ninja

### 运行环境
- **MySQL**: 8.0+
- **Redis**: 6.0+
- **磁盘空间**: 至少10GB可用空间
- **内存**: 至少4GB RAM

## 快速开始

### 1. 克隆项目

```bash
git clone https://github.com/yourusername/ChatAndFileTransferSystem.git
cd ChatAndFileTransferSystem
```

### 2. 配置数据库

```bash
# 创建数据库
mysql -u root -p < Scripts/create_database.sql

# 创建表结构
mysql -u root -p chat_file_transfer < Scripts/create_tables.sql

# 插入测试数据（可选）
mysql -u root -p chat_file_transfer < Scripts/insert_test_data.sql
```

### 3. 配置Redis

```bash
# 启动Redis服务
redis-server

# 验证Redis运行
redis-cli ping
# 应该返回: PONG
```

### 4. 配置服务器

编辑 `Config/metaserver.conf` 和 `Config/storageserver.conf`，配置数据库和Redis连接信息。

### 5. 构建项目

#### Windows

```cmd
# Release构建
Scripts\build.bat

# Debug构建
Scripts\build.bat debug
```

#### Linux/macOS

```bash
# 给脚本添加执行权限
chmod +x Scripts/*.sh

# Release构建
./Scripts/build.sh

# Debug构建
./Scripts/build.sh debug
```

### 6. 运行项目

```bash
# 启动MetaServer
./build/bin/MetaServer

# 启动StorageServer
./build/bin/StorageServer

# 启动客户端
./build/bin/Client
```

## 详细文档

- [构建指南](Docs/BUILD.md) - 详细的构建说明和常见问题
- [部署指南](Docs/DEPLOYMENT.md) - 生产环境部署指南
- [架构设计](Docs/聊天与文件传输系统重构技术详细设计文档.md) - 系统架构详细设计
- [接口规范](Docs/详细接口与逻辑规范文档.md) - API接口详细说明

## 配置说明

### MetaServer配置

配置文件: `Config/metaserver.conf`

主要配置项：
- `Network`: 网络监听配置
- `Database`: MySQL数据库连接配置
- `Redis`: Redis缓存配置
- `Storage`: 文件存储配置
- `LoadBalancer`: 负载均衡配置
- `Log`: 日志配置

### StorageServer配置

配置文件: `Config/storageserver.conf`

主要配置项：
- `Network`: 网络监听配置
- `MetaServer`: MetaServer连接配置
- `Storage`: 文件存储配置
- `Token`: Token管理配置
- `Log`: 日志配置

### Client配置

配置文件: `Config/client.conf`

主要配置项：
- `Network`: MetaServer连接配置
- `Transfer`: 文件传输配置
- `UI`: 界面配置
- `Log`: 日志配置

## 性能优化

### 已实现的优化
- ✅ 内存池：减少内存分配开销
- ✅ 数据库连接池：优化数据库访问
- ✅ Redis缓存：减少数据库查询
- ✅ Epoll高并发：支持数千连接
- ✅ 文件分片传输：支持大文件
- ✅ 加权轮询负载均衡：智能分配任务

### 性能指标
- **并发连接**: 支持1000+并发连接
- **文件传输**: 支持10GB+大文件
- **响应时间**: 平均<100ms
- **吞吐量**: 100MB/s+ (局域网)

## 测试

### 运行单元测试

#### Windows

```cmd
Scripts\test.bat
```

#### Linux/macOS

```bash
./Scripts/test.sh
```

### 测试覆盖
- 内存池测试
- 协议编解码测试
- 负载均衡算法测试
- 数据库操作测试
- Token验证测试

## 部署

### 快速部署

```bash
# 构建项目
./Scripts/build.sh

# 部署到deploy目录
./Scripts/deploy.sh

# 启动服务
cd deploy
./bin/MetaServer &
./bin/StorageServer &
```

### 生产环境部署

详见 [部署指南](Docs/DEPLOYMENT.md)

## 开发规范

### 代码规范
- 遵循C++17标准
- 使用现代C++特性（智能指针、lambda等）
- 所有代码必须添加中文注释
- 遵循项目编码规范（见.trae/rules/project_rules.md）

### 提交规范
- 提交前运行测试
- 提交前运行lint检查
- 提交信息清晰描述变更

## 常见问题

### Q: 编译时提示找不到Qt6？
A: 请确保已安装Qt 6.x，并设置Qt_DIR环境变量指向Qt6的lib/cmake目录。

### Q: 连接数据库失败？
A: 请检查Config/metaserver.conf中的数据库配置，确保MySQL服务已启动。

### Q: Redis连接失败？
A: 请检查Redis服务是否启动，以及Config/metaserver.conf中的Redis配置是否正确。

### Q: 文件上传失败？
A: 请检查StorageServer是否启动，以及磁盘空间是否充足。

更多问题请查看 [构建指南](Docs/BUILD.md)。

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

## 贡献指南

欢迎提交 Issue 和 Pull Request 来帮助改进项目。

### 贡献流程
1. Fork项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建Pull Request

## 更新日志

### v2.0.0 (2026-01)
- ✅ 重构为双服务器架构（MetaServer + StorageServer）
- ✅ 实现控制平面与数据平面分离
- ✅ 添加Epoll高并发支持
- ✅ 实现文件秒传功能
- ✅ 添加负载均衡机制
- ✅ 实现Token验证和续期
- ✅ 添加Redis缓存支持
- ✅ 实现数据库连接池
- ✅ 优化UI设计（无QSS）
- ✅ 添加完整的构建和部署脚本

### v1.0.0 (2024-12 ~ 2025-02)
- ✅ 基本聊天功能实现
- ✅ 文件上传下载功能
- ✅ 用户注册登录
- ✅ 好友管理
- ✅ 在线用户列表
- ✅ 文件分享功能
- ✅ 实现内存池
- ✅ 实现数据库连接池

## 联系方式

- 项目主页: https://github.com/yourusername/ChatAndFileTransferSystem
- 问题反馈: https://github.com/yourusername/ChatAndFileTransferSystem/issues

## 致谢

感谢所有为本项目做出贡献的开发者！
