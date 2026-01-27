#include "StorageEngine.h"
#include "../../Common/Utils/HashUtil.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <algorithm>

namespace ChatSystem {
namespace StorageServer {
namespace Core {

StorageEngine::StorageEngine(QObject* parent)
    : QObject(parent)
{
}

StorageEngine::~StorageEngine()
{
}

/**
 * @brief 安全处理文件路径，防止路径遍历攻击
 * @param filePath 原始文件路径
 * @return 安全的文件路径
 */
std::string StorageEngine::sanitizeFilePath(const std::string& filePath) const
{
    std::string safePath = filePath;
    
    size_t pos = 0;
    while ((pos = safePath.find("..", pos)) != std::string::npos) {
        safePath.replace(pos, 2, "");
        pos += 2;
    }
    
    std::replace(safePath.begin(), safePath.end(), '\\', '/');
    
    while (!safePath.empty() && (safePath[0] == '/' || safePath[0] == '.')) {
        safePath = safePath.substr(1);
    }
    
    return safePath;
}

bool StorageEngine::initialize(const std::string& basePath)
{
    m_basePath = basePath;
    
    if (!ensureDirectoryExists(basePath)) {
        std::cerr << "创建基础存储路径失败: " << basePath << std::endl;
        return false;
    }
    
    std::cout << "存储引擎初始化成功: " << basePath << std::endl;
    return true;
}

bool StorageEngine::writeChunk(const std::string& filePath, uint32_t chunkIndex, 
                            const QByteArray& chunkData)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string safePath = sanitizeFilePath(filePath);
    std::string fullPath = m_basePath + "/" + safePath;
    
    std::string dirPath = fullPath.substr(0, fullPath.find_last_of('/'));
    if (!ensureDirectoryExists(dirPath)) {
        std::cerr << "创建目录失败: " << dirPath << std::endl;
        return false;
    }
    
    std::ofstream file(fullPath, std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "打开文件失败: " << fullPath << std::endl;
        return false;
    }
    
    file.seekp(chunkIndex * chunkData.size(), std::ios::beg);
    file.write(chunkData.constData(), chunkData.size());
    file.close();
    
    emit writeProgress(filePath, chunkIndex, 0);
    return true;
}

QByteArray StorageEngine::readChunk(const std::string& filePath, uint32_t chunkIndex, 
                                 uint32_t chunkSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string safePath = sanitizeFilePath(filePath);
    std::string fullPath = m_basePath + "/" + safePath;
    
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "打开文件失败: " << fullPath << std::endl;
        return QByteArray();
    }
    
    file.seekg(chunkIndex * chunkSize, std::ios::beg);
    
    QByteArray chunkData(chunkSize, 0);
    file.read(chunkData.data(), chunkSize);
    
    chunkData.resize(file.gcount());
    file.close();
    
    return chunkData;
}

bool StorageEngine::createFile(const std::string& filePath, uint64_t fileSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string safePath = sanitizeFilePath(filePath);
    std::string fullPath = m_basePath + "/" + safePath;
    
    std::string dirPath = fullPath.substr(0, fullPath.find_last_of('/'));
    if (!ensureDirectoryExists(dirPath)) {
        std::cerr << "创建目录失败: " << dirPath << std::endl;
        return false;
    }
    
    std::ofstream file(fullPath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "创建文件失败: " << fullPath << std::endl;
        return false;
    }
    
    file.seekp(fileSize - 1);
    file.write("", 1);
    file.close();
    
    std::cout << "创建文件成功: " << fullPath << " (大小: " << fileSize << " 字节)" << std::endl;
    return true;
}

bool StorageEngine::deleteFile(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string safePath = sanitizeFilePath(filePath);
    std::string fullPath = m_basePath + "/" + safePath;
    
    if (unlink(fullPath.c_str()) != 0) {
        std::cerr << "删除文件失败: " << fullPath << std::endl;
        return false;
    }
    
    std::cout << "删除文件成功: " << fullPath << std::endl;
    return true;
}

bool StorageEngine::fileExists(const std::string& filePath) const
{
    std::string fullPath = m_basePath + "/" + filePath;
    
    struct stat buffer;
    return (stat(fullPath.c_str(), &buffer) == 0);
}

uint64_t StorageEngine::getFileSize(const std::string& filePath) const
{
    std::string fullPath = m_basePath + "/" + filePath;
    
    struct stat buffer;
    if (stat(fullPath.c_str(), &buffer) != 0) {
        std::cerr << "获取文件大小失败: " << fullPath << std::endl;
        return 0;
    }
    
    return buffer.st_size;
}

std::string StorageEngine::calculateFileHash(const std::string& filePath) const
{
    std::string safePath = sanitizeFilePath(filePath);
    std::string fullPath = m_basePath + "/" + safePath;
    
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "打开文件失败: " << fullPath << std::endl;
        return "";
    }
    
    std::string hash = HashUtil::calculateSHA256(file);
    file.close();
    
    return hash;
}

bool StorageEngine::ensureDirectoryExists(const std::string& dirPath) const
{
    struct stat buffer;
    
    if (stat(dirPath.c_str(), &buffer) == 0) {
        if (S_ISDIR(buffer.st_mode)) {
            return true;
        } else {
            std::cerr << "路径存在但不是目录: " << dirPath << std::endl;
            return false;
        }
    }
    
    std::string parentPath = dirPath.substr(0, dirPath.find_last_of('/'));
    if (!parentPath.empty() && !ensureDirectoryExists(parentPath)) {
        return false;
    }
    
    if (mkdir(dirPath.c_str(), 0755) != 0 && errno != EEXIST) {
        std::cerr << "创建目录失败: " << dirPath << " (" << strerror(errno) << ")" << std::endl;
        return false;
    }
    
    return true;
}

} // namespace Core
} // namespace StorageServer
} // namespace ChatSystem