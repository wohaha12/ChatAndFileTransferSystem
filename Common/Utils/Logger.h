#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QMessageLogContext>

namespace ChatSystem {
namespace Utils {

/**
 * @brief 日志级别枚举
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Fatal = 4
};

/**
 * @brief 日志管理器类
 * @details 使用Qt的全局消息处理器接管日志输出
 *          支持输出到控制台和文件
 *          包含时间戳、线程ID、日志级别
 */
class Logger {
public:
    /**
     * @brief 初始化日志系统
     * @param logFilePath 日志文件路径，空字符串表示不输出到文件
     */
    static void initialize(const QString& logFilePath = "");
    
    /**
     * @brief 设置日志级别
     * @param level 日志级别，低于此级别的日志不会输出
     */
    static void setLogLevel(LogLevel level);
    
    /**
     * @brief 获取日志级别
     * @return 当前日志级别
     */
    static LogLevel getLogLevel();
    
    /**
     * @brief 输出调试日志
     * @param msg 日志消息
     */
    static void debug(const QString& msg);
    
    /**
     * @brief 输出信息日志
     * @param msg 日志消息
     */
    static void info(const QString& msg);
    
    /**
     * @brief 输出警告日志
     * @param msg 日志消息
     */
    static void warning(const QString& msg);
    
    /**
     * @brief 输出错误日志
     * @param msg 日志消息
     */
    static void error(const QString& msg);
    
    /**
     * @brief 输出致命错误日志
     * @param msg 日志消息
     */
    static void fatal(const QString& msg);
    
private:
    /**
     * @brief 日志消息处理函数
     * @param type 消息类型
     * @param context 消息上下文
     * @param msg 消息内容
     */
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    
    /**
     * @brief 格式化日志消息
     * @param level 日志级别
     * @param msg 消息内容
     * @param context 消息上下文
     * @return 格式化后的日志消息
     */
    static QString formatMessage(LogLevel level, const QString& msg, const QMessageLogContext& context);
    
    /**
     * @brief 将日志级别转换为字符串
     * @param level 日志级别
     * @return 日志级别字符串
     */
    static QString levelToString(LogLevel level);
    
private:
    static QMutex s_mutex;              // 互斥锁，保护文件写入
    static QFile* s_logFile;           // 日志文件指针
    static QTextStream* s_logStream;     // 日志文件流
    static LogLevel s_logLevel;          // 当前日志级别
    static bool s_initialized;            // 是否已初始化
};

} // namespace Utils
} // namespace ChatSystem
