# 技术文档目录

本目录存放项目相关的技术文档，按照功能和用途进行分类管理。

## 文档结构

### 基础文档
- **README.md** - 项目文档说明
- **DEPLOYMENT.md** - 部署文档

### 技术设计文档 (technical-design/)
- **ArchitectureDesign.md** - 项目架构图设计
- **DetailedDesign.md** - 聊天与文件传输系统重构技术详细设计文档
- **InterfaceSpecification.md** - 详细接口与逻辑规范文档

### 开发过程文档 (development/)
- **RefactoringProgress.md** - 服务端重构进度总结
- **ProjectEvaluationReport.md** - 项目评估报告
- **TestReport.md** - 测试报告

### 性能优化文档 (optimization/)
- **OptimizationPlan.md** - 优化计划与实现

## 文档说明

这些文档详细记录了项目的开发过程、技术设计和实现细节，包括：

- 架构设计与实现
- 接口规范与协议定义
- 性能优化措施
- 重构进度与成果
- 项目评估与测试
- 部署与运维指南

## 更新日期

- 创建时间: 2026-01-20
- 最后更新: 2026-01-27

## 重构亮点

本次重构实现了以下重要改进：

### 1. 构建系统升级
- 从 QMake 迁移到 CMake 3.16+
- 支持 Qt6.2+ 版本
- 统一 C++17 标准
- 改进静态库链接方式

### 2. 网络层跨平台统一
- 移除 Linux 独有的 Epoll 代码
- 使用 Qt6 原生网络模块（QTcpServer/QTcpSocket）
- Windows 上自动使用 IOCP
- Linux 上自动使用 Epoll
- macOS 上自动使用 kqueue

### 3. 安全机制增强
- 添加路径遍历防护（sanitizeFilePath）
- 添加缓冲区溢出防护（safeCopy）
- 协议头添加版本号字段
- 上传响应添加秒传标志
- 下载请求添加用户ID验证

### 4. 并发与性能优化
- Redis 操作异步化（QtConcurrent::run）
- 使用 QFutureWatcher 监听异步结果
- 避免阻塞主事件循环
- 提升系统并发性能

### 5. 运维基础设施
- 实现统一的日志系统（Logger）
- 支持多级别日志（Debug、Info、Warning、Error、Fatal）
- 输出到控制台和文件
- 包含时间戳、线程ID、文件行号

### 6. 文档一致性
- 更新所有技术文档，反映 Qt6 + CMake 构建系统
- 更新架构图，展示 Qt6 原生网络
- 更新协议定义，包含安全字段
- 更新部署文档，支持 CMake 构建
- 确保文档与代码实现保持一致

## 技术栈

- **构建工具**: CMake 3.16+
- **C++标准**: C++17
- **Qt版本**: Qt 6.2+
- **数据库**: MySQL 8.0+
- **缓存**: Redis 6.0+
- **跨平台**: Windows、Linux、macOS

## 文档维护

所有文档遵循 [DOCUMENTATION_GUIDELINES.md](DOCUMENTATION_GUIDELINES.md) 中定义的规范进行维护和更新。
