#pragma once

#include <string>
#include <vector>

namespace ChatSystem {
namespace MetaServer {
namespace Database {

class DatabaseManager;

/**
 * @brief 文件数据访问对象类
 */
class FileDAO {
public:
    /**
     * @brief 构造函数
     * @param dbManager 数据库管理器指针
     */
    explicit FileDAO(DatabaseManager* dbManager);
    
    /**
     * @brief 析构函数
     */
    ~FileDAO();
    
    /**
     * @brief 检查物理文件是否已存在
     * @param fileHash 文件哈希
     * @param storagePath 输出存储路径
     * @param serverId 输出存储服务器ID
     * @return 存在返回true，不存在返回false
     */
    bool checkPhysicalFileExists(const std::string& fileHash, std::string& storagePath, uint32_t& serverId);
    
    /**
     * @brief 创建物理文件记录
     * @param fileHash 文件哈希
     * @param fileSize 文件大小
     * @param storagePath 存储路径
     * @param serverId 存储服务器ID
     * @return 成功返回true，失败返回false
     */
    bool createPhysicalFileRecord(const std::string& fileHash, uint64_t fileSize, 
                                const std::string& storagePath, uint32_t serverId);
    
    /**
     * @brief 创建逻辑文件记录
     * @param userId 用户ID
     * @param fileName 文件名
     * @param filePath 文件路径
     * @param fileSize 文件大小
     * @param fileHash 文件哈希
     * @param fileType 文件类型
     * @param parentId 父目录ID
     * @param fileId 输出文件ID
     * @return 成功返回true，失败返回false
     */
    bool createLogicalFileRecord(uint64_t userId, const std::string& fileName, 
                               const std::string& filePath, uint64_t fileSize, 
                               const std::string& fileHash, uint8_t fileType, 
                               uint64_t parentId, uint64_t& fileId);
    
    /**
     * @brief 增加物理文件引用计数
     * @param fileHash 文件哈希
     * @return 成功返回true，失败返回false
     */
    bool increaseFileRef(const std::string& fileHash);
    
    /**
     * @brief 减少物理文件引用计数
     * @param fileHash 文件哈希
     * @return 成功返回true，失败返回false
     */
    bool decreaseFileRef(const std::string& fileHash);
    
    /**
     * @brief 获取用户文件列表
     * @param userId 用户ID
     * @param parentId 父目录ID
     * @param files 输出文件列表
     * @return 成功返回true，失败返回false
     */
    bool getUserFiles(uint64_t userId, uint64_t parentId, std::vector<Core::FileMetaInfo>& files);
    
    /**
     * @brief 获取文件信息
     * @param fileId 文件ID
     * @param fileInfo 输出文件信息
     * @return 成功返回true，失败返回false
     */
    bool getFileInfo(uint64_t fileId, Core::FileMetaInfo& fileInfo);
    
    /**
     * @brief 删除文件记录
     * @param userId 用户ID
     * @param fileId 文件ID
     * @return 成功返回true，失败返回false
     */
    bool deleteFileRecord(uint64_t userId, uint64_t fileId);
    
    /**
     * @brief 重命名文件
     * @param userId 用户ID
     * @param fileId 文件ID
     * @param newName 新文件名
     * @return 成功返回true，失败返回false
     */
    bool renameFile(uint64_t userId, uint64_t fileId, const std::string& newName);
    
    /**
     * @brief 移动文件
     * @param userId 用户ID
     * @param fileId 文件ID
     * @param newParentId 新父目录ID
     * @return 成功返回true，失败返回false
     */
    bool moveFile(uint64_t userId, uint64_t fileId, uint64_t newParentId);
    
    /**
     * @brief 获取存储服务器列表
     * @param servers 输出服务器列表
     * @return 成功返回true，失败返回false
     */
    bool getStorageServers(std::vector<std::tuple<uint32_t, std::string, uint16_t, int>>& servers);
    
    /**
     * @brief 更新存储服务器状态
     * @param serverId 服务器ID
     * @param status 服务器状态
     * @return 成功返回true，失败返回false
     */
    bool updateStorageServerStatus(uint32_t serverId, uint8_t status);
    
private:
    DatabaseManager* m_dbManager;  // 数据库管理器
};

} // namespace Database
} // namespace MetaServer
} // namespace ChatSystem
