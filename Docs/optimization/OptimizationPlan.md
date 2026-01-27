# 项目优化计划

## 一、内存池实现总结

### 已完成的工作
1. **分析项目结构和内存使用情况**
   - 分析了Server端的核心组件（MyTcpServer、MyTcpSocket、MsgHandler）
   - 确定了PDU结构是主要的内存分配热点

2. **设计内存池的接口和实现方案**
   - 采用单例模式实现
   - 支持固定大小内存块分配
   - 线程安全设计（使用QMutex）

3. **实现内存池**
   - 创建了`memorypool.h`和`memorypool.cpp`文件
   - 实现了内存块管理、分配、释放等核心功能

4. **集成内存池到项目中**
   - 修改了`protocol.cpp`中的`mkPDU`函数，使用内存池分配内存
   - 修改了`mytcpsocket.cpp`和`msghandler.cpp`中的内存释放逻辑，使用内存池的`deallocate`函数
   - 在`server.cpp`中初始化内存池
   - 更新了`Server.pro`文件，添加了内存池相关文件

5. **测试内存池**
   - 成功编译项目
   - 服务器程序正常启动，内存池工作正常

### 内存池的主要特性
- 支持多种大小的内存块管理
- 自动扩展内存池（当没有可用内存时自动分配新的内存页）
- 线程安全的分配和释放操作
- 提供状态查询功能

## 二、数据库连接池设计与实现计划

### 设计目标
- 提高数据库操作的并发性能
- 避免频繁创建和关闭数据库连接的开销
- 提供线程安全的连接获取和释放机制
- 支持连接的有效性检查

### 实现方案

1. **核心组件设计**
   - `ConnectionPool`类：单例模式，管理数据库连接池
   - `Connection`类：封装数据库连接和状态

2. **主要接口**
   ```cpp
   class ConnectionPool {
   public:
       static ConnectionPool& getInstance();
       QSqlDatabase getConnection();
       void releaseConnection(const QSqlDatabase& db);
       void init(int minSize = 5, int maxSize = 20, int timeout = 30);
       void destroy();
   };
   ```

3. **实现要点**
   - 使用队列管理可用连接
   - 使用互斥锁保证线程安全
   - 定期检查连接的有效性
   - 当没有可用连接且未达到最大连接数时，创建新连接
   - 当获取连接超时后，返回错误

4. **集成计划**
   - 修改`OperateDB`类，使用连接池获取数据库连接
   - 在服务器启动时初始化连接池
   - 在数据库操作完成后释放连接

## 三、大文件多线程分片上传设计与实现计划

### 设计目标
- 支持大文件的高效上传
- 利用多线程提高上传速度
- 支持断点续传
- 减少网络传输失败的影响

### 实现方案

1. **核心组件设计**
   - `FileUploadManager`类：管理文件上传任务
   - `UploadTask`类：封装单个文件分片的上传任务
   - `FileSlice`类：表示文件的一个分片

2. **通信协议设计**
   - 增加文件上传相关的消息类型：
     - `ENUM_MSG_TYPE_FILE_UPLOAD_INIT_REQUEST`：初始化上传
     - `ENUM_MSG_TYPE_FILE_UPLOAD_INIT_RESPOND`：初始化响应
     - `ENUM_MSG_TYPE_FILE_UPLOAD_SLICE_REQUEST`：上传分片
     - `ENUM_MSG_TYPE_FILE_UPLOAD_SLICE_RESPOND`：分片响应
     - `ENUM_MSG_TYPE_FILE_UPLOAD_COMPLETE_REQUEST`：上传完成
     - `ENUM_MSG_TYPE_FILE_UPLOAD_COMPLETE_RESPOND`：完成响应

3. **实现要点**
   - 客户端将文件分成固定大小的分片（如1MB）
   - 客户端使用多线程并行上传分片
   - 服务器接收分片并存储临时文件
   - 所有分片上传完成后，服务器合并分片为完整文件
   - 支持断点续传（记录已上传的分片）

## 四、文件秒传设计与实现计划

### 设计目标
- 当用户上传已存在的文件时，直接返回成功，无需实际传输
- 提高用户体验
- 减少服务器存储和网络带宽消耗

### 实现方案

1. **核心组件设计**
   - `FileHashManager`类：管理文件哈希值和文件路径的映射
   - `HashCalculator`类：计算文件的哈希值（如MD5、SHA1）

2. **通信协议设计**
   - 增加文件秒传相关的消息类型：
     - `ENUM_MSG_TYPE_FILE_FAST_UPLOAD_REQUEST`：请求秒传
     - `ENUM_MSG_TYPE_FILE_FAST_UPLOAD_RESPOND`：秒传响应

3. **实现要点**
   - 客户端在上传文件前，先计算文件的哈希值
   - 客户端发送哈希值到服务器，请求秒传
   - 服务器查询哈希值是否已存在
   - 如果存在，直接创建文件的引用，返回秒传成功
   - 如果不存在，返回需要正常上传
   - 使用数据库存储文件哈希值和路径的映射关系

## 五、后续工作安排

1. **实现数据库连接池**
   - 创建连接池的头文件和源文件
   - 实现连接池的核心功能
   - 集成到项目中，替换现有的数据库连接方式

2. **实现大文件多线程分片上传**
   - 设计并实现客户端和服务器端的上传逻辑
   - 实现分片合并功能
   - 测试大文件上传的性能和稳定性

3. **实现文件秒传**
   - 实现文件哈希计算功能
   - 设计并实现秒传的通信协议
   - 测试秒传功能

4. **性能优化和测试**
   - 对各个模块进行性能测试
   - 优化内存池、连接池的参数配置
   - 进行压力测试，确保系统在高并发场景下的稳定性