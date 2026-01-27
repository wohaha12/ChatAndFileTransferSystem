# 详细接口与逻辑规范文档

---

## 1. 详细通信协议定义 (Interface Definition)

### 1.1 全局状态码定义

```cpp
/**
 * @brief 全局错误码枚举
 * @details 采用分段设计，便于错误分类和管理
 *          - 通用错误(0-1000): 系统级、网络级错误
 *          - 认证类(1001-2000): 登录、权限验证相关错误
 *          - 业务类(2001-3000): 文件操作、用户管理相关错误
 *          - 存储类(3001-4000): 文件存储、磁盘IO相关错误
 */
enum ErrorCode : uint32_t {
    // ==================== 通用错误 (0-1000) ====================
    SUCCESS = 0,                          // 操作成功
    ERROR_UNKNOWN = 1,                    // 未知错误
    ERROR_INVALID_PARAM = 2,               // 参数错误
    ERROR_NETWORK = 3,                     // 网络错误
    ERROR_TIMEOUT = 4,                     // 超时
    ERROR_PROTOCOL = 5,                    // 协议错误
    ERROR_BUFFER_OVERFLOW = 6,             // 缓冲区溢出
    ERROR_CHECKSUM_FAILED = 7,             // 校验和失败
    ERROR_NOT_IMPLEMENTED = 8,            // 功能未实现
    ERROR_SERVER_BUSY = 9,                 // 服务器繁忙
    
    // ==================== 认证类错误 (1001-2000) ====================
    ERROR_AUTH_FAILED = 1001,              // 认证失败
    ERROR_USER_NOT_FOUND = 1002,           // 用户不存在
    ERROR_PASSWORD_WRONG = 1003,           // 密码错误
    ERROR_TOKEN_EXPIRED = 1004,            // Token过期
    ERROR_TOKEN_INVALID = 1005,            // Token无效
    ERROR_PERMISSION_DENIED = 1006,        // 权限不足
    ERROR_USER_ALREADY_EXISTS = 1007,      // 用户已存在
    ERROR_USER_LOCKED = 1008,             // 用户被锁定
    ERROR_SESSION_EXPIRED = 1009,          // 会话过期
    ERROR_LOGIN_TOO_MANY = 1010,           // 登录次数过多
    
    // ==================== 业务类错误 (2001-3000) ====================
    ERROR_FILE_NOT_FOUND = 2001,           // 文件不存在
    ERROR_FILE_ALREADY_EXISTS = 2002,      // 文件已存在
    ERROR_FILE_IN_USE = 2003,             // 文件正在使用
    ERROR_FILE_TOO_LARGE = 2004,           // 文件过大
    ERROR_DIR_NOT_FOUND = 2005,            // 目录不存在
    ERROR_DIR_NOT_EMPTY = 2006,           // 目录非空
    ERROR_INVALID_FILE_NAME = 2007,        // 无效文件名
    ERROR_QUOTA_EXCEEDED = 2008,          // 配额超限
    ERROR_FRIEND_NOT_FOUND = 2009,         // 好友不存在
    ERROR_FRIEND_ALREADY_EXISTS = 2010,     // 好友已存在
    ERROR_CHAT_FAILED = 2011,              // 聊天失败
    ERROR_OPERATION_NOT_ALLOWED = 2012,     // 操作不允许
    
    // ==================== 存储类错误 (3001-4000) ====================
    ERROR_STORAGE_FULL = 3001,             // 存储空间不足
    ERROR_DISK_IO_ERROR = 3002,            // 磁盘IO错误
    ERROR_STORAGE_SERVER_OFFLINE = 3003,    // 存储服务器离线
    ERROR_CHUNK_FAILED = 3004,              // 分片失败
    ERROR_CHUNK_MISMATCH = 3005,           // 分片不匹配
    ERROR_FILE_CORRUPTED = 3006,          // 文件损坏
    ERROR_NO_AVAILABLE_STORAGE = 3007,     // 无可用存储服务器
    ERROR_UPLOAD_TOKEN_INVALID = 3008,      // 上传令牌无效
    ERROR_UPLOAD_TIMEOUT = 3009,           // 上传超时
    ERROR_DOWNLOAD_FAILED = 3010,           // 下载失败
};
```

### 1.2 命令字定义

```cpp
/**
 * @brief 命令字枚举
 * @details 区分控制平面和数据平面的命令
 *          - 0x1000-0x1FFF: 客户端 <-> Meta Server 控制命令
 *          - 0x2000-0x2FFF: 客户端 <-> Storage Server 数据命令
 *          - 0x3000-0x3FFF: Meta Server <-> Storage Server 内部RPC命令
 */
enum CommandType : uint32_t {
    // ==================== 控制平面命令 (0x1000-0x1FFF) ====================
    
    // 用户认证相关 (0x1001-0x10FF)
    CMD_META_LOGIN_REQ = 0x1001,          // 登录请求
    CMD_META_LOGIN_RES = 0x1002,          // 登录响应
    CMD_META_LOGOUT_REQ = 0x1003,         // 登出请求
    CMD_META_LOGOUT_RES = 0x1004,         // 登出响应
    CMD_META_REGISTER_REQ = 0x1005,       // 注册请求
    CMD_META_REGISTER_RES = 0x1006,       // 注册响应
    
    // 聊天相关 (0x1100-0x11FF)
    CMD_META_CHAT_REQ = 0x1101,           // 发送聊天消息请求
    CMD_META_CHAT_RES = 0x1102,           // 发送聊天消息响应
    CMD_META_CHAT_NOTIFY = 0x1103,        // 接收聊天消息通知
    
    // 文件操作相关 (0x1200-0x12FF)
    CMD_META_UPLOAD_REQ = 0x1201,         // 申请上传文件
    CMD_META_UPLOAD_RES = 0x1202,         // 上传申请响应
    CMD_META_DOWNLOAD_REQ = 0x1203,       // 申请下载文件
    CMD_META_DOWNLOAD_RES = 0x1204,       // 下载申请响应
    CMD_META_DELETE_FILE_REQ = 0x1205,    // 删除文件请求
    CMD_META_DELETE_FILE_RES = 0x1206,    // 删除文件响应
    CMD_META_RENAME_FILE_REQ = 0x1207,    // 重命名文件请求
    CMD_META_RENAME_FILE_RES = 0x1208,    // 重命名文件响应
    CMD_META_MOVE_FILE_REQ = 0x1209,      // 移动文件请求
    CMD_META_MOVE_FILE_RES = 0x120A,      // 移动文件响应
    CMD_META_MKDIR_REQ = 0x120B,         // 创建目录请求
    CMD_META_MKDIR_RES = 0x120C,         // 创建目录响应
    CMD_META_RMDIR_REQ = 0x120D,         // 删除目录请求
    CMD_META_RMDIR_RES = 0x120E,         // 删除目录响应
    CMD_META_FILE_LIST_REQ = 0x120F,      // 获取文件列表请求
    CMD_META_FILE_LIST_RES = 0x1210,      // 获取文件列表响应
    
    // 好友管理相关 (0x1300-0x13FF)
    CMD_META_ADD_FRIEND_REQ = 0x1301,     // 添加好友请求
    CMD_META_ADD_FRIEND_RES = 0x1302,     // 添加好友响应
    CMD_META_DELETE_FRIEND_REQ = 0x1303,  // 删除好友请求
    CMD_META_DELETE_FRIEND_RES = 0x1304,  // 删除好友响应
    CMD_META_FRIEND_LIST_REQ = 0x1305,    // 获取好友列表请求
    CMD_META_FRIEND_LIST_RES = 0x1306,    // 获取好友列表响应
    CMD_META_ONLINE_USERS_REQ = 0x1307,   // 获取在线用户请求
    CMD_META_ONLINE_USERS_RES = 0x1308,   // 获取在线用户响应
    
    // ==================== 数据平面命令 (0x2000-0x2FFF) ====================
    
    // 文件上传相关 (0x2000-0x20FF)
    CMD_STORAGE_UPLOAD_CHUNK_REQ = 0x2001, // 上传文件分片请求
    CMD_STORAGE_UPLOAD_CHUNK_RES = 0x2002, // 上传文件分片响应
    CMD_STORAGE_UPLOAD_COMPLETE_REQ = 0x2003, // 上传完成请求
    CMD_STORAGE_UPLOAD_COMPLETE_RES = 0x2004, // 上传完成响应
    
    // 文件下载相关 (0x2100-0x21FF)
    CMD_STORAGE_DOWNLOAD_CHUNK_REQ = 0x2101, // 下载文件分片请求
    CMD_STORAGE_DOWNLOAD_CHUNK_RES = 0x2102, // 下载文件分片响应
    
    // ==================== 内部RPC命令 (0x3000-0x3FFF) ====================
    
    // 心跳与状态 (0x3000-0x30FF)
    CMD_INTERNAL_HEARTBEAT = 0x3001,      // 心跳检测
    CMD_INTERNAL_STATUS_REPORT = 0x3002,   // 状态上报
    CMD_INTERNAL_SERVER_REGISTER = 0x3003,  // 服务器注册
    
    // 文件操作通知 (0x3100-0x31FF)
    CMD_INTERNAL_UPLOAD_COMPLETE = 0x3101, // 上传完成通知
    CMD_INTERNAL_DELETE_FILE = 0x3102,    // 删除物理文件通知
    CMD_INTERNAL_FILE_MOVED = 0x3103,     // 文件移动通知
};
```

### 1.3 二进制协议头定义

```cpp
#pragma pack(push, 1)  // 设置1字节对齐，避免内存填充

/**
 * @brief 传输协议头结构
 * @details 定长包头，用于解决TCP粘包问题
 *          总长度：28字节（包含版本号）
 *          字节序：网络字节序（大端序，Big-Endian）
 * 
 * @字段说明：
 * - magic: 魔数，用于校验包的完整性，固定值 0x12345678
 * - version: 协议版本号，用于协议兼容性检查（当前版本：1）
 * - cmd: 命令字，对应CommandType枚举
 * - seq: 序列号，用于请求响应匹配，客户端生成
 * - len: 包体长度（不包含包头），单位：字节
 * - checksum: 校验和，使用CRC32算法计算包体数据
 * - reserved: 保留字段，用于未来扩展
 * 
 * @字节布局：
 * Offset | Size | Field        | Type      | Description
 * -------|------|--------------|-----------|----------------
 * 0      | 4    | magic        | uint32_t  | 魔数
 * 4      | 4    | version      | uint32_t  | 协议版本号
 * 8      | 4    | cmd          | uint32_t  | 命令字
 * 12     | 4    | seq          | uint32_t  | 序列号
 * 16     | 4    | len          | uint32_t  | 包体长度
 * 20     | 4    | checksum     | uint32_t  | 校验和
 * 24     | 4    | reserved     | uint32_t  | 保留字段
 */
struct TransHeader {
    uint32_t magic;        // 魔数，固定值 0x12345678
    uint32_t version;      // 协议版本号（当前版本：1）
    uint32_t cmd;          // 命令字
    uint32_t seq;          // 序列号
    uint32_t len;          // 包体长度（不包含包头）
    uint32_t checksum;     // 校验和（CRC32）
    uint32_t reserved;     // 保留字段
    
    /**
     * @brief 构造函数
     * @param cmd 命令字
     * @param seq 序列号
     * @param len 包体长度
     */
    TransHeader(uint32_t cmd = 0, uint32_t seq = 0, uint32_t len = 0)
        : magic(0x12345678), version(1), cmd(cmd), seq(seq), len(len), checksum(0), reserved(0) {}
    
    /**
     * @brief 转换为网络字节序
     * @details 将所有字段转换为大端序（网络字节序）
     */
    void toNetworkOrder() {
        magic = htonl(magic);
        version = htonl(version);
        cmd = htonl(cmd);
        seq = htonl(seq);
        len = htonl(len);
        checksum = htonl(checksum);
        reserved = htonl(reserved);
    }
    
    /**
     * @brief 转换为主机字节序
     * @details 将所有字段从大端序转换为主机字节序
     */
    void toHostOrder() {
        magic = ntohl(magic);
        version = ntohl(version);
        cmd = ntohl(cmd);
        seq = ntohl(seq);
        len = ntohl(len);
        checksum = ntohl(checksum);
        reserved = ntohl(reserved);
    }
    
    /**
     * @brief 计算校验和
     * @param data 包体数据
     * @return 校验和（CRC32）
     */
    uint32_t calculateChecksum(const char* data, uint32_t dataLen) const {
        // 使用CRC32算法计算校验和
        uint32_t crc = 0xFFFFFFFF;
        for (uint32_t i = 0; i < dataLen; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
            }
        }
        return ~crc;
    }
    
    /**
     * @brief 验证校验和
     * @param data 包体数据
     * @return true-校验成功，false-校验失败
     */
    bool validateChecksum(const char* data) const {
        uint32_t calculated = calculateChecksum(data, len);
        return calculated == checksum;
    }
    
    /**
     * @brief 验证魔数
     * @return true-魔数正确，false-魔数错误
     */
    bool validateMagic() const {
        return magic == 0x12345678;
    }
    
    /**
     * @brief 验证协议版本
     * @return true-版本兼容，false-版本不兼容
     */
    bool validateVersion() const {
        return version == 1;
    }
};

#pragma pack(pop)  // 恢复默认对齐方式
```

### 1.4 核心包体结构定义

```cpp
#pragma pack(push, 1)

/**
 * @brief 登录请求包体
 * @details 包含用户名和密码哈希
 */
struct LoginReq {
    char username[64];          // 用户名（UTF-8编码）
    char password_hash[128];    // 密码哈希（SHA256）
    char client_version[16];    // 客户端版本号（如 "1.0.0"）
    char device_info[64];       // 设备信息（如 "Windows 10"）
};

/**
 * @brief 登录响应包体
 */
struct LoginRes {
    uint32_t result_code;      // 结果码（ErrorCode枚举）
    uint64_t user_id;         // 用户ID
    char nickname[64];         // 昵称
    char session_token[64];    // 会话令牌
    uint32_t expire_time;      // 过期时间（Unix时间戳）
    char error_msg[256];       // 错误信息
};

/**
 * @brief 上传申请请求包体
 */
struct UploadReq {
    uint64_t user_id;         // 用户ID
    char file_name[256];      // 文件名（含扩展名）
    uint64_t file_size;       // 文件大小（字节）
    char file_hash[64];       // 文件MD5哈希（32字节十六进制）
    char parent_path[512];    // 父目录路径（相对路径）
    uint32_t chunk_size;      // 请求的分片大小（字节）
};

/**
 * @brief 上传申请响应包体
 * @details 包含秒传标志，用于快速上传
 */
struct UploadRes {
    uint32_t result_code;      // 结果码
    uint64_t file_id;         // 文件ID（秒传时返回）
    uint8_t is_instant_upload; // 是否秒传（0-否，1-是）
    char upload_token[64];     // 上传令牌
    char storage_ip[45];       // 存储服务器IP（支持IPv6）
    uint16_t storage_port;    // 存储服务器端口
    uint32_t chunk_size;      // 建议的分片大小
    uint32_t expire_time;     // Token过期时间（Unix时间戳）
    char error_msg[256];      // 错误信息
};

/**
 * @brief 文件分片上传请求包体
 * @details 包体长度可变，chunk_data为柔性数组
 */
struct UploadChunkReq {
    char upload_token[64];     // 上传令牌
    uint32_t chunk_index;     // 分片索引（从0开始）
    uint32_t chunk_size;      // 分片大小
    uint32_t chunk_offset;    // 分片在文件中的偏移量
    char chunk_data[];        // 分片数据（柔性数组）
};

/**
 * @brief 文件分片上传响应包体
 */
struct UploadChunkRes {
    uint32_t result_code;      // 结果码
    uint32_t chunk_index;     // 分片索引
    uint32_t next_chunk_index; // 建议的下一个分片索引
    char error_msg[256];      // 错误信息
};

/**
 * @brief 下载申请请求包体
 * @details 包含用户ID，用于权限验证
 */
struct DownloadReq {
    uint64_t user_id;         // 用户ID（用于权限验证）
    uint64_t file_id;         // 文件ID
    uint32_t chunk_size;      // 请求的分片大小
};

/**
 * @brief 下载申请响应包体
 */
struct DownloadRes {
    uint32_t result_code;      // 结果码
    uint64_t file_size;      // 文件大小
    char file_hash[64];       // 文件MD5哈希
    char storage_ip[45];      // 存储服务器IP（支持IPv6）
    uint16_t storage_port;    // 存储服务器端口
    uint32_t chunk_size;      // 建议的分片大小
    char error_msg[256];      // 错误信息
};

/**
 * @brief 文件分片下载请求包体
 * @details 包体长度可变，chunk_data为柔性数组
 */
struct DownloadChunkReq {
    uint64_t file_id;         // 文件ID
    uint32_t chunk_index;     // 分片索引（从0开始）
    uint32_t chunk_size;      // 分片大小
    uint32_t chunk_offset;    // 分片在文件中的偏移量
};

/**
 * @brief 文件分片下载响应包体
 */
struct DownloadChunkRes {
    uint32_t result_code;      // 结果码
    uint32_t chunk_index;     // 分片索引
    uint32_t chunk_size;      // 分片大小
    char chunk_data[];        // 分片数据（柔性数组）
};

#pragma pack(pop)
```

---

## 2. 安全机制说明

### 2.1 路径遍历防护

在文件上传和下载时，必须对文件路径进行安全处理：

```cpp
std::string sanitizeFilePath(const std::string& filePath) {
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

### 2.2 缓冲区溢出防护

在字符串拷贝时，必须使用安全拷贝函数：

```cpp
void safeCopy(char* dest, const std::string& src, size_t destSize) {
    if (destSize == 0) {
        return;
    }
    
    // 确保最多拷贝 destSize - 1 个字符
    size_t length = std::min(src.size(), destSize - 1);
    std::memcpy(dest, src.c_str(), length);
    dest[length] = '\0'; // 强制结束符
}
```

### 2.3 协议版本验证

在处理协议包时，必须验证协议版本：

```cpp
bool TransHeader::validateVersion() const {
    return version == 1;
}
```

---

## 3. 总结

本文档详细描述了聊天与文件传输系统的接口与逻辑规范，主要包含以下内容：

1. **错误码定义**: 分段设计，便于错误分类和管理
2. **命令字定义**: 区分控制平面和数据平面的命令
3. **协议头定义**: 定长包头，包含版本号，支持协议兼容性
4. **包体结构**: 核心包体结构，包含安全字段
5. **安全机制**: 路径遍历防护、缓冲区溢出防护、协议版本验证

所有协议定义遵循 C++17 标准和 Qt6 规范，确保代码质量和可维护性。
