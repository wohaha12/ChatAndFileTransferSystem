#pragma once

#include <string>
#include <mutex>
#include <QObject>

namespace ChatSystem {
namespace StorageServer {
namespace Core {

/**
 * @brief 存储引擎类
 */
class StorageEngine : public QObject {
    Q_OBJECT
public:
    explicit StorageEngine(QObject* parent = nullptr);
    ~StorageEngine();
    
    /**
     * @brief 初始化存储引擎
     * @param basePath 基础存储路径
     * @return 成功返回true，失败返回false
     */
    bool initialize(const std::string& basePath);
    
    /**
     * @brief 写入文件分片
     * @param filePath 文件路径
     * @param chunkIndex 分片索引
     * @param chunkData 分片数据
     * @return 成功返回true，失败返回false
     */
    bool writeChunk(const std::string& filePath, uint32_t chunkIndex, 
                  const QByteArray& chunkData);
    
    /**
     * @brief 读取文件分片
     * @param filePath 文件路径
     * @param chunkIndex 分片索引
     * @param chunkSize 分片大小
     * @return 分片数据，失败返回空QByteArray
     */
    QByteArray readChunk(const std::string& filePath, uint32_t chunkIndex, 
                      uint32_t chunkSize);
    
    /**
     * @brief 创建文件
     * @param filePath 文件路径
     * @param fileSize 文件大小
     * @return 成功返回true，失败返回false
     */
    bool createFile(const std::string& filePath, uint64_t fileSize);
    
    /**
     * @brief 删除文件
     * @param filePath 文件路径
     * @return 成功返回true，失败返回false
     */
    bool deleteFile(const std::string& filePath);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 存在返回true，不存在返回false
     */
    bool fileExists(const std::string& filePath) const;
    
    /**
     * @brief 获取文件大小
     * @param filePath 文件路径
     * @return 文件大小（字节），失败返回0
     */
    uint64_t getFileSize(const std::string& filePath) const;
    
    /**
     * @brief 计算文件哈希
     * @param filePath 文件路径
     * @return 文件哈希值，失败返回空字符串
     */
    std::string calculateFileHash(const std::string& filePath) const;
    
signals:
    /**
     * @brief 写入进度信号
     * @param filePath 文件路径
     * @param chunkIndex 当前分片索引
     * @param totalChunks 总分片数
     */
    void writeProgress(const std::string& filePath, uint32_t chunkIndex, uint32_t totalChunks);
    
    /**
     * @brief 写入完成信号
     * @param filePath 文件路径
     * @param success 是否成功
     */
    void writeCompleted(const std::string& filePath, bool success);
    
private:
    /**
     * @brief 确保目录存在
     * @param dirPath 目录路径
     * @return 存在或创建成功返回true，失败返回false
     */
    bool ensureDirectoryExists(const std::string& dirPath) const;
    
    /**
     * @brief 安全处理文件路径，防止路径遍历攻击
     * @param filePath 原始文件路径
     * @return 安全的文件路径
     */
    static std::string sanitizeFilePath(const std::string& filePath);
    
private:
    std::string m_basePath;                    // 基础存储路径
    std::mutex m_mutex;                       // 互斥锁
};

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem
