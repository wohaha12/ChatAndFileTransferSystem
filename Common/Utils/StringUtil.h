#pragma once

#include <string>
#include <vector>
#include <string_view>

namespace ChatSystem {
namespace Utils {

/**
 * @brief 字符串处理工具类
 */
class StringUtil {
public:
    /**
     * @brief 分割字符串
     * @param str 原始字符串
     * @param delimiter 分隔符
     * @return 分割后的字符串向量
     */
    static std::vector<std::string> split(const std::string& str, char delimiter);

    /**
     * @brief 分割字符串（使用字符串分隔符）
     * @param str 原始字符串
     * @param delimiter 分隔符字符串
     * @return 分割后的字符串向量
     */
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);

    /**
     * @brief 字符串替换
     * @param str 原始字符串
     * @param from 要替换的子串
     * @param to 替换后的子串
     * @return 替换后的字符串
     */
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);

    /**
     * @brief 字符串转小写
     * @param str 原始字符串
     * @return 小写字符串
     */
    static std::string toLower(const std::string& str);

    /**
     * @brief 字符串转大写
     * @param str 原始字符串
     * @return 大写字符串
     */
    static std::string toUpper(const std::string& str);

    /**
     * @brief 修剪字符串两端的空白字符
     * @param str 原始字符串
     * @return 修剪后的字符串
     */
    static std::string trim(const std::string& str);

    /**
     * @brief 修剪字符串左侧的空白字符
     * @param str 原始字符串
     * @return 修剪后的字符串
     */
    static std::string trimLeft(const std::string& str);

    /**
     * @brief 修剪字符串右侧的空白字符
     * @param str 原始字符串
     * @return 修剪后的字符串
     */
    static std::string trimRight(const std::string& str);

    /**
     * @brief 检查字符串是否以指定前缀开头
     * @param str 原始字符串
     * @param prefix 前缀
     * @return 是返回true，否则返回false
     */
    static bool startsWith(const std::string& str, const std::string& prefix);

    /**
     * @brief 检查字符串是否以指定后缀结尾
     * @param str 原始字符串
     * @param suffix 后缀
     * @return 是返回true，否则返回false
     */
    static bool endsWith(const std::string& str, const std::string& suffix);

    /**
     * @brief 格式化字符串
     * @param format 格式字符串
     * @param args 可变参数
     * @return 格式化后的字符串
     */
    template<typename... Args>
    static std::string format(const std::string& format, Args... args);

    /**
     * @brief 检查字符串是否为空或只包含空白字符
     * @param str 原始字符串
     * @return 是返回true，否则返回false
     */
    static bool isEmptyOrWhitespace(const std::string& str);
};

} // namespace Utils
} // namespace ChatSystem
