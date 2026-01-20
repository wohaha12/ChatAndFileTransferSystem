#ifndef COMMON_PROTOCOL_BODIES_H
#define COMMON_PROTOCOL_BODIES_H

#include <cstdint>
#include "CommonProtocol.h"

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
 */
struct UploadRes {
    uint32_t result_code;      // 结果码
    uint64_t file_id;         // 文件ID（秒传时返回）
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
 */
struct DownloadReq {
    uint64_t user_id;         // 用户ID
    uint64_t file_id;         // 文件ID
    uint32_t chunk_size;      // 请求的分片大小
};

/**
 * @brief 下载申请响应包体
 */
struct DownloadRes {
    uint32_t result_code;      // 结果码
    uint64_t file_id;         // 文件ID
    uint64_t file_size;       // 文件大小
    char file_hash[64];       // 文件MD5哈希
    char file_name[256];      // 文件名
    char storage_ip[45];       // 存储服务器IP
    uint16_t storage_port;    // 存储服务器端口
    uint32_t chunk_size;      // 建议的分片大小
    uint32_t total_chunks;    // 总分片数
    char error_msg[256];      // 错误信息
};

/**
 * @brief 文件分片下载请求包体
 */
struct DownloadChunkReq {
    uint64_t file_id;         // 文件ID
    uint32_t chunk_index;     // 分片索引
    uint32_t chunk_size;      // 请求的分片大小
};

/**
 * @brief 文件分片下载响应包体
 * @details 包体长度可变，chunk_data为柔性数组
 */
struct DownloadChunkRes {
    uint32_t result_code;      // 结果码
    uint32_t chunk_index;     // 分片索引
    uint32_t chunk_size;      // 分片大小
    char chunk_data[];        // 分片数据（柔性数组）
};

/**
 * @brief 聊天消息请求包体
 */
struct ChatReq {
    uint64_t sender_id;       // 发送者ID
    char sender_name[64];     // 发送者用户名
    uint64_t receiver_id;      // 接收者ID
    char receiver_name[64];    // 接收者用户名
    uint32_t msg_len;        // 消息长度
    char msg_data[];         // 消息数据（柔性数组）
};

/**
 * @brief 聊天消息响应包体
 */
struct ChatRes {
    uint32_t result_code;      // 结果码
    char error_msg[256];      // 错误信息
};

/**
 * @brief 聊天消息通知包体
 */
struct ChatNotify {
    uint64_t sender_id;       // 发送者ID
    char sender_name[64];     // 发送者用户名
    uint64_t receiver_id;      // 接收者ID
    char receiver_name[64];    // 接收者用户名
    uint32_t msg_len;        // 消息长度
    char msg_data[];         // 消息数据（柔性数组）
};

/**
 * @brief 文件列表请求包体
 */
struct FileListReq {
    uint64_t user_id;         // 用户ID
    char parent_path[512];    // 父目录路径（相对路径）
};

/**
 * @brief 文件列表响应包体
 */
struct FileListRes {
    uint32_t result_code;      // 结果码
    uint32_t file_count;      // 文件数量
    char error_msg[256];      // 错误信息
    // 后续跟随file_count个FileInfo结构
};

/**
 * @brief 文件信息结构
 */
struct FileInfo {
    uint64_t file_id;         // 文件ID
    char file_name[256];     // 文件名
    uint64_t file_size;       // 文件大小
    char file_hash[64];       // 文件MD5哈希
    uint32_t file_type;       // 文件类型：0-目录，1-文件
    uint64_t parent_id;       // 父目录ID
    uint32_t create_time;     // 创建时间（Unix时间戳）
    uint32_t update_time;     // 更新时间（Unix时间戳）
};

/**
 * @brief 删除文件请求包体
 */
struct DeleteFileReq {
    uint64_t user_id;         // 用户ID
    uint64_t file_id;         // 文件ID
};

/**
 * @brief 删除文件响应包体
 */
struct DeleteFileRes {
    uint32_t result_code;      // 结果码
    char error_msg[256];      // 错误信息
};

/**
 * @brief 重命名文件请求包体
 */
struct RenameFileReq {
    uint64_t user_id;         // 用户ID
    uint64_t file_id;         // 文件ID
    char new_name[256];      // 新文件名
};

/**
 * @brief 重命名文件响应包体
 */
struct RenameFileRes {
    uint32_t result_code;      // 结果码
    char error_msg[256];      // 错误信息
};

/**
 * @brief 创建目录请求包体
 */
struct MkDirReq {
    uint64_t user_id;         // 用户ID
    char dir_name[256];      // 目录名
    char parent_path[512];    // 父目录路径（相对路径）
};

/**
 * @brief 创建目录响应包体
 */
struct MkDirRes {
    uint32_t result_code;      // 结果码
    uint64_t dir_id;          // 目录ID
    char error_msg[256];      // 错误信息
};

/**
 * @brief 删除目录请求包体
 */
struct RmDirReq {
    uint64_t user_id;         // 用户ID
    uint64_t dir_id;          // 目录ID
};

/**
 * @brief 删除目录响应包体
 */
struct RmDirRes {
    uint32_t result_code;      // 结果码
    char error_msg[256];      // 错误信息
};

/**
 * @brief 添加好友请求包体
 */
struct AddFriendReq {
    uint64_t user_id;         // 用户ID
    char friend_name[64];    // 好友用户名
};

/**
 * @brief 添加好友响应包体
 */
struct AddFriendRes {
    uint32_t result_code;      // 结果码
    char error_msg[256];      // 错误信息
};

/**
 * @brief 删除好友请求包体
 */
struct DeleteFriendReq {
    uint64_t user_id;         // 用户ID
    char friend_name[64];    // 好友用户名
};

/**
 * @brief 删除好友响应包体
 */
struct DeleteFriendRes {
    uint32_t result_code;      // 结果码
    char error_msg[256];      // 错误信息
};

/**
 * @brief 好友列表请求包体
 */
struct FriendListReq {
    uint64_t user_id;         // 用户ID
};

/**
 * @brief 好友列表响应包体
 */
struct FriendListRes {
    uint32_t result_code;      // 结果码
    uint32_t friend_count;    // 好友数量
    char error_msg[256];      // 错误信息
    // 后续跟随friend_count个FriendInfo结构
};

/**
 * @brief 好友信息结构
 */
struct FriendInfo {
    uint64_t friend_id;       // 好友ID
    char friend_name[64];    // 好友用户名
    uint32_t status;          // 状态：0-离线，1-在线，2-隐身
    uint32_t last_active;     // 最后活跃时间（Unix时间戳）
};

/**
 * @brief 在线用户列表请求包体
 */
struct OnlineUsersReq {
    uint64_t user_id;         // 用户ID
};

/**
 * @brief 在线用户列表响应包体
 */
struct OnlineUsersRes {
    uint32_t result_code;      // 结果码
    uint32_t user_count;      // 在线用户数量
    char error_msg[256];      // 错误信息
    // 后续跟随user_count个UserInfo结构
};

/**
 * @brief 用户信息结构
 */
struct UserInfo {
    uint64_t user_id;         // 用户ID
    char username[64];       // 用户名
    char nickname[64];       // 昵称
    uint32_t status;          // 状态：0-离线，1-在线，2-隐身
    uint32_t last_active;     // 最后活跃时间（Unix时间戳）
};

#pragma pack(pop)

#endif // COMMON_PROTOCOL_BODIES_H
