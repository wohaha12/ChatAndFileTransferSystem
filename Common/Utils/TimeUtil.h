#pragma once

#include <string>
#include <cstdint>

namespace ChatSystem {
namespace Utils {

/**
 * @brief 时间处理工具类
 */
class TimeUtil {
public:
    /**
     * @brief 获取当前时间戳（秒）
     * @return 当前时间戳
     */
    static uint64_t getCurrentTimestamp();

    /**
     * @brief 获取当前时间戳（毫秒）
     * @return 当前时间戳
     */
    static uint64_t getCurrentTimestampMs();

    /**
     * @brief 获取当前时间戳（微秒）
     * @return 当前时间戳
     */
    static uint64_t getCurrentTimestampUs();

    /**
     * @brief 格式化时间戳为字符串
     * @param timestamp 时间戳（秒）
     * @param format 格式字符串（如 "%Y-%m-%d %H:%M:%S"）
     * @return 格式化后的时间字符串
     */
    static std::string formatTimestamp(uint64_t timestamp, const std::string& format = "%Y-%m-%d %H:%M:%S");

    /**
     * @brief 解析时间字符串为时间戳
     * @param timeStr 时间字符串
     * @param format 格式字符串（如 "%Y-%m-%d %H:%M:%S"）
     * @return 时间戳（秒），失败返回-1
     */
    static int64_t parseTimeString(const std::string& timeStr, const std::string& format = "%Y-%m-%d %H:%M:%S");

    /**
     * @brief 获取当前时间的格式化字符串
     * @param format 格式字符串
     * @return 格式化后的时间字符串
     */
    static std::string getCurrentTimeString(const std::string& format = "%Y-%m-%d %H:%M:%S");

    /**
     * @brief 计算两个时间戳之间的差值（秒）
     * @param timestamp1 第一个时间戳
     * @param timestamp2 第二个时间戳
     * @return 差值（timestamp2 - timestamp1）
     */
    static int64_t getTimeDifference(uint64_t timestamp1, uint64_t timestamp2);

    /**
     * @brief 将时间戳转换为本地时间
     * @param timestamp 时间戳（秒）
     * @return 本地时间的tm结构
     */
    static struct tm convertToLocalTime(uint64_t timestamp);

    /**
     * @brief 将时间戳转换为UTC时间
     * @param timestamp 时间戳（秒）
     * @return UTC时间的tm结构
     */
    static struct tm convertToUtcTime(uint64_t timestamp);
};

} // namespace Utils
} // namespace ChatSystem
