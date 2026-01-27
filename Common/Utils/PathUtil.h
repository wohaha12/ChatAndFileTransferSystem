#ifndef COMMON_UTILS_PATHUTIL_H
#define COMMON_UTILS_PATHUTIL_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>

/**
 * @brief 路径工具类
 * @details 提供文件存储路径生成和管理功能
 */
class PathUtil {
public:
    /**
     * @brief 生成文件存储路径
     * @details 采用二级目录结构，将文件均匀分布在多个目录中
     *          一级目录：Hash的前2位字符
     *          二级目录：Hash的第3-4位字符
     *          文件名：完整的Hash值
     * 
     * @param hash 文件MD5哈希值（32位十六进制字符串）
     * @return 生成的存储路径（相对路径）
     * 
     * @example
     * 输入: "d41d8cd98f00b204e9800998ecf8427e"
     * 输出: "d4/1d/d41d8cd98f00b204e9800998ecf8427e"
     * 
     * @设计原理：
     * 1. 哈希值具有均匀分布特性，前几位字符可以均匀分布在0-9和a-f之间
     * 2. 二级目录结构可以将文件分散到 16 * 16 = 256 个目录中
     * 3. 每个目录下的文件数量可控，避免单个目录文件过多
     * 4. Linux文件系统对目录下的文件数量有限制（ext4约32000个），二级结构有效避免此限制
     * 
     * @性能优势：
     * 1. 减少目录扫描时间：查找文件时只需扫描少量目录
     * 2. 提高文件系统缓存命中率：目录元数据可以缓存在内存中
     * 3. 并行访问：不同目录的文件可以并行访问，提高并发性能
     * 4. 易于维护：可以按目录进行备份、迁移、清理操作
     */
    static std::string generatePath(const std::string& hash);
    
    /**
     * @brief 生成带时间戳的存储路径
     * @details 在哈希路径基础上增加日期目录，便于按时间归档
     * 
     * @param hash 文件MD5哈希值
     * @param timestamp Unix时间戳
     * @return 带时间戳的存储路径
     * 
     * @example
     * 输入: hash="d41d8cd98f00b204e9800998ecf8427e", timestamp=1640995200
     * 输出: "2022/01/01/d4/1d/d41d8cd98f00b204e9800998ecf8427e"
     */
    static std::string generatePathWithTimestamp(const std::string& hash, uint64_t timestamp);
    
    /**
     * @brief 解析存储路径，提取文件哈希
     * @details 从生成的路径中反向提取文件哈希
     * 
     * @param path 存储路径
     * @return 文件哈希值，失败返回空字符串
     * 
     * @example
     * 输入: "d4/1d/d41d8cd98f00b204e9800998ecf8427e"
     * 输出: "d41d8cd98f00b204e9800998ecf8427e"
     */
    static std::string extractHashFromPath(const std::string& path);
    
    /**
     * @brief 计算路径的目录层级
     * @details 计算给定路径包含的目录层级数
     * 
     * @param path 文件路径
     * @return 目录层级数
     * 
     * @example
     * 输入: "d4/1d/d41d8cd98f00b204e9800998ecf8427e"
     * 输出: 2
     */
    static int calculatePathDepth(const std::string& path);
};

#endif // COMMON_UTILS_PATHUTIL_H
