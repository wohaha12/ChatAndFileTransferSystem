#pragma once
#include <string>
#include <vector>
#include <optional>

namespace storage_server {
namespace io {

/**
 * @struct ChunkInfo
 * @brief 文件块信息结构体
 */
struct ChunkInfo {
    std::string chunk_id;     ///< 块ID
    std::string file_path;    ///< 块存储路径
    size_t offset;            ///< 块在文件中的偏移量
    size_t size;              ///< 块大小
    std::string md5;          ///< 块MD5校验值
};

/**
 * @class ChunkManager
 * @brief 文件块管理器，用于管理文件块的存储和检索
 */
class ChunkManager {
public:
    /**
     * @brief 构造函数
     */
    ChunkManager();
    
    /**
     * @brief 析构函数
     */
    ~ChunkManager();
    
    /**
     * @brief 初始化块管理器
     * @param chunk_dir 块存储目录
     * @return 成功返回true，失败返回false
     */
    bool Init(const std::string& chunk_dir);
    
    /**
     * @brief 存储文件块
     * @param file_id 文件ID
     * @param chunk_index 块索引
     * @param offset 块在文件中的偏移量
     * @param size 块大小
     * @param data 块数据
     * @return 块信息，失败返回空
     */
    std::optional<ChunkInfo> StoreChunk(const std::string& file_id, 
                                       size_t chunk_index, 
                                       size_t offset, 
                                       size_t size, 
                                       const char* data);
    
    /**
     * @brief 读取文件块
     * @param chunk_id 块ID
     * @param buffer 输出缓冲区
     * @param buffer_size 缓冲区大小
     * @return 实际读取的字节数，失败返回-1
     */
    ssize_t ReadChunk(const std::string& chunk_id, char* buffer, size_t buffer_size);
    
    /**
     * @brief 删除文件块
     * @param chunk_id 块ID
     * @return 成功返回true，失败返回false
     */
    bool DeleteChunk(const std::string& chunk_id);
    
    /**
     * @brief 获取文件块信息
     * @param chunk_id 块ID
     * @return 块信息，不存在返回空
     */
    std::optional<ChunkInfo> GetChunkInfo(const std::string& chunk_id) const;
    
    /**
     * @brief 生成块ID
     * @param file_id 文件ID
     * @param chunk_index 块索引
     * @return 块ID
     */
    std::string GenerateChunkId(const std::string& file_id, size_t chunk_index) const;
    
private:
    std::string chunk_dir_;  ///< 块存储目录
};

} // namespace io
} // namespace storage_server
