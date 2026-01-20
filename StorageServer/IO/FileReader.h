#pragma once
#include <cstddef>
#include <string>
#include <fstream>

namespace storage_server {
namespace io {

/**
 * @class FileReader
 * @brief 文件读取器，用于高效读取文件内容
 */
class FileReader {
public:
    /**
     * @brief 构造函数
     */
    FileReader();
    
    /**
     * @brief 析构函数
     */
    ~FileReader();
    
    /**
     * @brief 打开文件
     * @param file_path 文件路径
     * @return 成功返回true，失败返回false
     */
    bool Open(const std::string& file_path);
    
    /**
     * @brief 关闭文件
     */
    void Close();
    
    /**
     * @brief 读取文件指定偏移量和长度的数据
     * @param offset 偏移量
     * @param length 读取长度
     * @param buffer 输出缓冲区
     * @return 实际读取的字节数，失败返回-1
     */
    ssize_t Read(size_t offset, size_t length, char* buffer);
    
    /**
     * @brief 获取文件大小
     * @return 文件大小，失败返回-1
     */
    off_t GetFileSize() const;
    
    /**
     * @brief 检查文件是否打开
     * @return 打开返回true，否则返回false
     */
    bool IsOpen() const;
    
private:
    std::ifstream file_stream_;  ///< 文件流
    std::string file_path_;       ///< 文件路径
    off_t file_size_;             ///< 文件大小
};

} // namespace io
} // namespace storage_server
