#pragma once
#include <cstddef>
#include <string>
#include <fstream>

namespace storage_server {
namespace io {

/**
 * @class FileWriter
 * @brief 文件写入器，用于高效写入文件内容
 */
class FileWriter {
public:
    /**
     * @brief 构造函数
     */
    FileWriter();
    
    /**
     * @brief 析构函数
     */
    ~FileWriter();
    
    /**
     * @brief 打开文件
     * @param file_path 文件路径
     * @param append 是否追加写入
     * @return 成功返回true，失败返回false
     */
    bool Open(const std::string& file_path, bool append = false);
    
    /**
     * @brief 关闭文件
     */
    void Close();
    
    /**
     * @brief 写入数据到文件指定偏移量
     * @param offset 偏移量
     * @param length 写入长度
     * @param buffer 输入缓冲区
     * @return 实际写入的字节数，失败返回-1
     */
    ssize_t Write(size_t offset, size_t length, const char* buffer);
    
    /**
     * @brief 检查文件是否打开
     * @return 打开返回true，否则返回false
     */
    bool IsOpen() const;
    
    /**
     * @brief 获取当前文件大小
     * @return 文件大小，失败返回-1
     */
    off_t GetFileSize() const;
    
private:
    std::ofstream file_stream_;  ///< 文件流
    std::string file_path_;       ///< 文件路径
    bool append_mode_;            ///< 是否为追加模式
};

} // namespace io
} // namespace storage_server
