#include "StringUtil.h"
#include <cstring>
#include <algorithm>
#include <sstream>
#include <memory>

namespace ChatSystem {
namespace Utils {

std::vector<std::string> StringUtil::split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::vector<std::string> StringUtil::split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    tokens.push_back(str.substr(start));
    
    return tokens;
}

std::string StringUtil::replace(const std::string& str, const std::string& from, const std::string& to)
{
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

std::string StringUtil::toLower(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtil::toUpper(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtil::trim(const std::string& str)
{
    return trimRight(trimLeft(str));
}

std::string StringUtil::trimLeft(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtil::trimRight(const std::string& str)
{
    size_t end = str.find_last_not_of(" \t\n\r");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix)
{
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix)
{
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

template<typename... Args>
std::string StringUtil::format(const std::string& format, Args... args)
{
    int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

bool StringUtil::isEmptyOrWhitespace(const std::string& str)
{
    if (str.empty()) {
        return true;
    }
    
    return str.find_first_not_of(" \t\n\r") == std::string::npos;
}

void StringUtil::safeCopy(char* dest, const std::string& src, size_t destSize)
{
    if (destSize == 0) {
        return;
    }
    
    size_t length = std::min(src.size(), destSize - 1);
    std::memcpy(dest, src.c_str(), length);
    dest[length] = '\0';
}

} // namespace Utils
} // namespace ChatSystem
