#pragma once

#include <cstdint>
#include "CommandTypes.h"

namespace ChatSystem {
namespace Protocol {

/**
 * @brief 心跳包
 */
struct Heartbeat {
    uint64_t timestamp;    // 时间戳
    uint32_t server_id;    // 服务器ID
    uint32_t status;       // 状态：0-正常，1-异常
};

/**
 * @brief 状态上报
 */
struct StatusReport {
    uint64_t timestamp;    // 时间戳
    uint32_t server_id;    // 服务器ID
    float cpu_usage;       // CPU使用率(百分比)
    float memory_usage;    // 内存使用率(百分比)
    float disk_usage;      // 磁盘使用率(百分比)
    uint64_t network_in;   // 网络入流量(字节)
    uint64_t network_out;  // 网络出流量(字节)
    uint32_t connections;   // 当前连接数
};

/**
 * @brief 上传完成通知
 */
struct UploadComplete {
    uint64_t user_id;      // 用户ID
    char file_hash[64];    // 文件哈希
    uint64_t file_size;    // 文件大小
    char storage_path[512]; // 存储路径
    uint32_t server_id;    // 存储服务器ID
    uint64_t timestamp;    // 完成时间戳
};

/**
 * @brief 删除物理文件请求
 */
struct DeletePhysicalFile {
    char file_hash[64];    // 文件哈希
    uint32_t server_id;    // 存储服务器ID
};

/**
 * @brief 上传完成通知（增强版，支持重试和幂等性）
 */
#pragma pack(push, 1)
struct UploadCompleteNotify {
    char upload_token[64];        // 上传令牌（唯一标识）
    uint64_t user_id;             // 用户ID
    char file_hash[64];           // 文件哈希
    uint64_t file_size;           // 文件大小
    char file_name[256];          // 文件名
    uint32_t server_id;           // 存储服务器ID
    char server_ip[45];           // 服务器IP
    uint16_t server_port;         // 服务器端口
    char storage_path[512];       // 物理存储路径
    uint32_t status;              // 状态：0-成功，1-部分失败
    uint32_t total_chunks;        // 总分片数
    uint32_t success_chunks;     // 成功分片数
    uint32_t failed_chunks;      // 失败分片数
    uint64_t upload_time;         // 上传完成时间戳
    uint64_t avg_speed;           // 平均速度（字节/秒）
    uint64_t start_time;          // 开始时间
    uint64_t end_time;            // 结束时间
    uint32_t retry_count;         // 重试次数（用于幂等性判断）
    uint32_t reserved;            // 保留字段
    char error_msg[256];          // 错误信息
};
#pragma pack(pop)

/**
 * @brief 下载完成通知
 */
#pragma pack(push, 1)
struct DownloadCompleteNotification {
    uint64_t user_id;             // 用户ID
    uint64_t file_id;             // 文件ID
    uint64_t timestamp;           // 完成时间戳
    uint32_t reserved;            // 保留字段
};
#pragma pack(pop)

} // namespace Protocol
} // namespace ChatSystem
