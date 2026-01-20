#pragma once

#include <string>
#include <vector>

namespace ChatSystem {
namespace Utils {

/**
 * @brief 哈希计算工具类
 */
class HashUtil {
public:
    /**
     * @brief 计算字符串的MD5哈希值
     * @param input 输入字符串
     * @return MD5哈希值（32位小写）
     */
    static std::string calculateMD5(const std::string& input);

    /**
     * @brief 计算文件的MD5哈希值
     * @param filePath 文件路径
     * @param md5Result 输出MD5结果
     * @return 成功返回true，失败返回false
     */
    static bool calculateFileMD5(const std::string& filePath, std::string& md5Result);

    /**
     * @brief 计算字符串的SHA256哈希值
     * @param input 输入字符串
     * @return SHA256哈希值（64位小写）
     */
    static std::string calculateSHA256(const std::string& input);

    /**
     * @brief 计算文件的SHA256哈希值
     * @param filePath 文件路径
     * @param sha256Result 输出SHA256结果
     * @return 成功返回true，失败返回false
     */
    static bool calculateFileSHA256(const std::string& filePath, std::string& sha256Result);

    /**
     * @brief 生成随机盐值
     * @param length 盐值长度
     * @return 随机盐值
     */
    static std::string generateSalt(size_t length = 16);

    /**
     * @brief 计算带盐值的密码哈希
     * @param password 原始密码
     * @param salt 盐值
     * @return 带盐值的哈希值
     */
    static std::string calculatePasswordHash(const std::string& password, const std::string& salt);
};

} // namespace Utils
} // namespace ChatSystem
