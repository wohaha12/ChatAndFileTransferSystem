#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace ChatSystem {
namespace Utils {

/**
 * @brief 文件操作工具类
 */
class FileUtil {
public:
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 存在返回true，否则返回false
     */
    static bool exists(const std::string& filePath);

    /**
     * @brief 检查目录是否存在
     * @param dirPath 目录路径
     * @return 存在返回true，否则返回false
     */
    static bool dirExists(const std::string& dirPath);

    /**
     * @brief 创建目录
     * @param dirPath 目录路径
     * @param recursive 是否递归创建父目录
     * @return 成功返回true，失败返回false
     */
    static bool createDir(const std::string& dirPath, bool recursive = true);

    /**
     * @brief 获取文件大小
     * @param filePath 文件路径
     * @return 文件大小（字节），失败返回-1
     */
    static int64_t getFileSize(const std::string& filePath);

    /**
     * @brief 获取文件名（不包含路径）
     * @param filePath 文件路径
     * @return 文件名
     */
    static std::string getFileName(const std::string& filePath);

    /**
     * @brief 获取文件扩展名
     * @param filePath 文件路径
     * @return 文件扩展名（包含点号）
     */
    static std::string getFileExtension(const std::string& filePath);

    /**
     * @brief 获取文件所在目录
     * @param filePath 文件路径
     * @return 目录路径
     */
    static std::string getFileDirectory(const std::string& filePath);

    /**
     * @brief 列出目录下的所有文件
     * @param dirPath 目录路径
     * @param recursive 是否递归列出子目录
     * @return 文件路径向量
     */
    static std::vector<std::string> listFiles(const std::string& dirPath, bool recursive = false);

    /**
     * @brief 删除文件
     * @param filePath 文件路径
     * @return 成功返回true，失败返回false
     */
    static bool deleteFile(const std::string& filePath);

    /**
     * @brief 删除目录
     * @param dirPath 目录路径
     * @param recursive 是否递归删除子目录
     * @return 成功返回true，失败返回false
     */
    static bool deleteDir(const std::string& dirPath, bool recursive = false);

    /**
     * @brief 复制文件
     * @param srcPath 源文件路径
     * @param dstPath 目标文件路径
     * @return 成功返回true，失败返回false
     */
    static bool copyFile(const std::string& srcPath, const std::string& dstPath);

    /**
     * @brief 移动文件
     * @param srcPath 源文件路径
     * @param dstPath 目标文件路径
     * @return 成功返回true，失败返回false
     */
    static bool moveFile(const std::string& srcPath, const std::string& dstPath);

    /**
     * @brief 确保目录存在
     * @param dirPath 目录路径
     * @return 存在或创建成功返回true，失败返回false
     */
    static bool ensureDirExists(const std::string& dirPath);
};

} // namespace Utils
} // namespace ChatSystem
