#include "PathUtil.h"
#include <ctime>

std::string PathUtil::generatePath(const std::string& hash)
{
    // 参数校验
    if (hash.length() != 32) {
        return "";
    }
    
    // 提取一级目录名（前2位）
    std::string level1 = hash.substr(0, 2);
    
    // 提取二级目录名（第3-4位）
    std::string level2 = hash.substr(2, 2);
    
    // 构建完整路径
    std::ostringstream oss;
    oss << level1 << "/" << level2 << "/" << hash;
    
    return oss.str();
}

std::string PathUtil::generatePathWithTimestamp(const std::string& hash, uint64_t timestamp)
{
    // 将时间戳转换为日期（YYYY/MM/DD格式）
    time_t rawtime = static_cast<time_t>(timestamp);
    struct tm* timeinfo = localtime(&rawtime);
    
    std::ostringstream date_oss;
    date_oss << std::setfill('0')
             << (timeinfo->tm_year + 1900) << "/"
             << std::setw(2) << (timeinfo->tm_mon + 1) << "/"
             << std::setw(2) << timeinfo->tm_mday;
    
    // 生成哈希路径
    std::string hashPath = generatePath(hash);
    
    // 组合完整路径
    return date_oss.str() + "/" + hashPath;
}

std::string PathUtil::extractHashFromPath(const std::string& path)
{
    // 查找最后一个路径分隔符
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash == std::string::npos) {
        return "";
    }
    
    // 提取文件名（即哈希值）
    std::string hash = path.substr(lastSlash + 1);
    
    // 验证哈希长度
    if (hash.length() != 32) {
        return "";
    }
    
    return hash;
}

int PathUtil::calculatePathDepth(const std::string& path)
{
    int depth = 0;
    size_t pos = 0;
    
    while ((pos = path.find('/', pos)) != std::string::npos) {
        depth++;
        pos++;
    }
    
    return depth;
}
