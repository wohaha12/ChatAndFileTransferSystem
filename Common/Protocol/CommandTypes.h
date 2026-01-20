#pragma once

#include <cstdint>

namespace ChatSystem {
namespace Protocol {

/**
 * @brief 命令字枚举
 */
enum CommandType : uint32_t {
    // 控制平面命令 (Client <-> Meta Server)
    CMD_LOGIN_REQ = 0x1001,          // 登录请求
    CMD_LOGIN_RES = 0x1002,           // 登录响应
    CMD_LOGOUT_REQ = 0x1003,          // 登出请求
    CMD_LOGOUT_RES = 0x1004,          // 登出响应
    
    CMD_CHAT_REQ = 0x1011,            // 聊天请求
    CMD_CHAT_RES = 0x1012,            // 聊天响应
    CMD_CHAT_NOTIFY = 0x1013,         // 聊天通知
    
    CMD_UPLOAD_REQ = 0x1021,          // 申请上传
    CMD_UPLOAD_RES = 0x1022,           // 上传申请响应
    CMD_DOWNLOAD_REQ = 0x1023,        // 申请下载
    CMD_DOWNLOAD_RES = 0x1024,        // 下载申请响应
    
    CMD_FILE_LIST_REQ = 0x1031,       // 文件列表请求
    CMD_FILE_LIST_RES = 0x1032,       // 文件列表响应
    CMD_DELETE_FILE_REQ = 0x1033,     // 删除文件请求
    CMD_DELETE_FILE_RES = 0x1034,     // 删除文件响应
    CMD_RENAME_FILE_REQ = 0x1035,     // 重命名文件请求
    CMD_RENAME_FILE_RES = 0x1036,     // 重命名文件响应
    
    // 数据平面命令 (Client <-> Storage Server)
    CMD_UPLOAD_CHUNK_REQ = 0x2001,    // 上传分片请求
    CMD_UPLOAD_CHUNK_RES = 0x2002,     // 上传分片响应
    CMD_DOWNLOAD_CHUNK_REQ = 0x2003,  // 下载分片请求
    CMD_DOWNLOAD_CHUNK_RES = 0x2004,  // 下载分片响应
    
    // 内部RPC命令 (Meta Server <-> Storage Server)
    CMD_INTERNAL_HEARTBEAT = 0x3001,  // 心跳检测
    CMD_INTERNAL_STATUS_REPORT = 0x3002, // 状态上报
    CMD_INTERNAL_UPLOAD_COMPLETE = 0x3003, // 上传完成通知
    CMD_INTERNAL_DELETE_FILE = 0x3004,  // 删除物理文件
};

} // namespace Protocol
} // namespace ChatSystem
