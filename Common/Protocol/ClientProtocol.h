#pragma once

#include <cstdint>
#include "CommandTypes.h"

namespace ChatSystem {
namespace Protocol {

/**
 * @brief 登录请求
 */
struct LoginReq {
    char username[64];     // 用户名
    char password_hash[128]; // 密码哈希
    char client_version[16]; // 客户端版本
};

/**
 * @brief 登录响应
 */
struct LoginRes {
    uint32_t result_code;  // 结果码：0-成功，非0-错误码
    uint64_t user_id;      // 用户ID
    char nickname[64];     // 昵称
    char session_token[64]; // 会话令牌
    char error_msg[256];    // 错误信息
};

/**
 * @brief 上传请求
 */
struct UploadRequest {
    uint64_t user_id;      // 用户ID
    char file_name[256];   // 文件名
    uint64_t file_size;    // 文件大小
    char file_hash[64];    // 文件MD5哈希
    char parent_path[512]; // 父目录路径
    uint32_t chunk_size;   // 分片大小
};

/**
 * @brief 上传响应
 */
struct UploadResponse {
    uint32_t result_code;  // 结果码：0-成功，非0-错误码
    char upload_token[64]; // 上传令牌
    char storage_ip[45];   // 存储服务器IP
    uint16_t storage_port; // 存储服务器端口
    uint32_t chunk_size;  // 建议分片大小
    char error_msg[256];  // 错误信息
};

/**
 * @brief 文件分片数据包
 */
struct UploadChunk {
    char upload_token[64]; // 上传令牌
    uint32_t chunk_index; // 分片索引
    uint32_t chunk_size;  // 分片大小
    char chunk_data[];     // 分片数据(柔性数组)
};

/**
 * @brief 下载请求
 */
struct DownloadRequest {
    uint64_t user_id;      // 用户ID
    uint64_t file_id;      // 文件ID
    uint32_t chunk_size;   // 请求分片大小
};

/**
 * @brief 下载响应
 */
struct DownloadResponse {
    uint32_t result_code;  // 结果码：0-成功，非0-错误码
    uint64_t file_size;    // 文件大小
    char file_hash[64];    // 文件MD5哈希
    char storage_ip[45];   // 存储服务器IP
    uint16_t storage_port; // 存储服务器端口
    uint32_t chunk_size;  // 建议分片大小
    char error_msg[256];  // 错误信息
};

/**
 * @brief 下载分片数据包
 */
struct DownloadChunk {
    uint64_t file_id;      // 文件ID
    uint32_t chunk_index; // 分片索引
    uint32_t chunk_size;  // 分片大小
    char chunk_data[];     // 分片数据(柔性数组)
};

} // namespace Protocol
} // namespace ChatSystem
