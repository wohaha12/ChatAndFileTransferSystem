#ifndef COMMON_PROTOCOL_H
  #define COMMON_PROTOCOL_H

  #include <cstdint>
  #include <cstring>
  #include <QtEndian>

  #ifdef _WIN32
      #include <winsock2.h>
  #else
      #include <arpa/inet.h>
  #endif

  #pragma pack(push, 1)

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
    ERR_UNKNOWN = 1,                      // 未知错误
    ERR_INVALID_PARAM = 2,                // 参数错误
    ERR_NETWORK = 3,                      // 网络错误
    ERR_TIMEOUT = 4,                      // 超时
    ERR_PROTOCOL = 5,                     // 协议错误
    ERR_BUFFER_OVERFLOW = 6,              // 缓冲区溢出
    ERR_CHECKSUM_FAILED = 7,              // 校验和失败
    ERR_NOT_IMPLEMENTED = 8,              // 功能未实现
    ERR_SERVER_BUSY = 9,                  // 服务器繁忙
    
    // ==================== 认证类错误 (1001-2000) ====================
    ERR_AUTH_FAILED = 1001,              // 认证失败
    ERR_USER_NOT_FOUND = 1002,           // 用户不存在
    ERR_PASSWORD_WRONG = 1003,           // 密码错误
    ERR_TOKEN_EXPIRED = 1004,            // Token过期
    ERR_TOKEN_INVALID = 1005,            // Token无效
    ERR_PERMISSION_DENIED = 1006,        // 权限不足
    ERR_USER_ALREADY_EXISTS = 1007,      // 用户已存在
    ERR_USER_LOCKED = 1008,              // 用户被锁定
    ERR_SESSION_EXPIRED = 1009,          // 会话过期
    ERR_LOGIN_TOO_MANY = 1010,           // 登录次数过多
    
    // ==================== 业务类错误 (2001-3000) ====================
    ERR_FILE_NOT_FOUND = 2001,           // 文件不存在
    ERR_FILE_ALREADY_EXISTS = 2002,      // 文件已存在
    ERR_FILE_IN_USE = 2003,              // 文件正在使用
    ERR_FILE_TOO_LARGE = 2004,           // 文件过大
    ERR_DIR_NOT_FOUND = 2005,            // 目录不存在
    ERR_DIR_NOT_EMPTY = 2006,            // 目录非空
    ERR_INVALID_FILE_NAME = 2007,        // 无效文件名
    ERR_QUOTA_EXCEEDED = 2008,          // 配额超限
    ERR_FRIEND_NOT_FOUND = 2009,         // 好友不存在
    ERR_FRIEND_ALREADY_EXISTS = 2010,   // 好友已存在
    ERR_CHAT_FAILED = 2011,             // 聊天失败
    ERR_OPERATION_NOT_ALLOWED = 2012,   // 操作不允许
    
    // ==================== 存储类错误 (3001-4000) ====================
    ERR_STORAGE_FULL = 3001,            // 存储空间不足
    ERR_DISK_IO_ERROR = 3002,           // 磁盘IO错误
    ERR_STORAGE_SERVER_OFFLINE = 3003,  // 存储服务器离线
    ERR_CHUNK_FAILED = 3004,            // 分片失败
    ERR_CHUNK_MISMATCH = 3005,          // 分片不匹配
    ERR_FILE_CORRUPTED = 3006,          // 文件损坏
    ERR_NO_AVAILABLE_STORAGE = 3007,    // 无可用存储服务器
    ERR_UPLOAD_TOKEN_INVALID = 3008,    // 上传令牌无效
    ERR_UPLOAD_TIMEOUT = 3009,         // 上传超时
    ERR_DOWNLOAD_FAILED = 3010,        // 下载失败
};

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

/**
 * @brief 传输协议头结构
 * @details 定长包头，用于解决TCP粘包问题
 *          总长度：24字节
 *          字节序：网络字节序（大端序，Big-Endian）
 * 
 * @字段说明：
 * - magic: 魔数，用于校验包的完整性，固定值 0x12345678
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
 * 4      | 4    | cmd          | uint32_t  | 命令字
 * 8      | 4    | seq          | uint32_t  | 序列号
 * 12     | 4    | len          | uint32_t  | 包体长度
 * 16     | 4    | checksum     | uint32_t  | 校验和
 * 20     | 4    | reserved     | uint32_t  | 保留字段
 */
struct TransHeader {
    uint32_t magic;        // 魔数，固定值 0x12345678
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
        : magic(0x12345678), cmd(cmd), seq(seq), len(len), checksum(0), reserved(0) {}
    
    /**
     * @brief 转换为网络字节序（大端序）
     */
    void toNetworkOrder() {
        magic = qToBigEndian(magic);
        cmd = qToBigEndian(cmd);
        seq = qToBigEndian(seq);
        len = qToBigEndian(len);
        checksum = qToBigEndian(checksum);
        reserved = qToBigEndian(reserved);
    }
    
    /**
     * @brief 转换为主机字节序
     */
    void toHostOrder() {
        magic = qFromBigEndian(magic);
        cmd = qFromBigEndian(cmd);
        seq = qFromBigEndian(seq);
        len = qFromBigEndian(len);
        checksum = qFromBigEndian(checksum);
        reserved = qFromBigEndian(reserved);
    }
    
    /**
     * @brief 计算校验和
     * @param data 包体数据
     * @param dataLen 数据长度
     * @return 校验和（CRC32）
     */
    static uint32_t calculateChecksum(const char* data, uint32_t dataLen) {
        uint32_t crc = 0xFFFFFFFF;
        for (uint32_t i = 0; i < dataLen; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        return ~crc;
    }
    
    /**
     * @brief 验证校验和
     * @param data 包体数据
     * @param dataLen 数据长度
     * @param expectedChecksum 期望的校验和
     * @return true-校验成功，false-校验失败
     */
    static bool validateChecksum(const char* data, uint32_t dataLen, uint32_t expectedChecksum) {
        uint32_t calculated = calculateChecksum(data, dataLen);
        return calculated == expectedChecksum;
    }
    
    /**
     * @brief 验证魔数
     * @return true-魔数正确，false-魔数错误
     */
    bool validateMagic() const {
        return magic == 0x12345678;
    }
};

#pragma pack(pop)

#endif // COMMON_PROTOCOL_H
