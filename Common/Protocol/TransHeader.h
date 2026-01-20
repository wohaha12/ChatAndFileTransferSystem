#pragma once

#include <cstdint>

namespace ChatSystem {
namespace Protocol {

/**
 * @brief 定长包头结构，用于解决粘包问题
 */
#pragma pack(push, 1)  // 确保无内存对齐
struct TransHeader {
    uint32_t magic;        // 魔数，用于校验包的完整性 (固定值: 0x12345678)
    uint32_t cmd;          // 命令字，对应CommandType枚举
    uint32_t seq;          // 序列号，用于请求响应匹配
    uint32_t len;          // 包体长度(不包含包头)
    uint32_t checksum;     // 校验和，用于验证包完整性
    uint32_t reserved;     // 保留字段，未来扩展
};
#pragma pack(pop)  // 恢复默认内存对齐

} // namespace Protocol
} // namespace ChatSystem
