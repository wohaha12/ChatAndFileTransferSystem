#include "Logger.h"
#include <QMutexLocker>
#include <QCoreApplication>
#include <QThread>
#include <iostream>

namespace ChatSystem {
namespace Utils {

QMutex Logger::s_mutex;
QFile* Logger::s_logFile = nullptr;
QTextStream* Logger::s_logStream = nullptr;
LogLevel Logger::s_logLevel = LogLevel::Info;
bool Logger::s_initialized = false;

void Logger::initialize(const QString& logFilePath)
{
    QMutexLocker locker(&s_mutex);
    
    if (!logFilePath.isEmpty()) {
        s_logFile = nullptr;
        s_logStream = nullptr;
    } else {
        s_logFile = new QFile(logFilePath);
        if (s_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            s_logStream = new QTextStream(s_logFile);
        } else {
            delete s_logFile;
            s_logFile = nullptr;
            s_logStream = nullptr;
        }
    }
    
    qInstallMessageHandler(messageHandler);
    s_initialized = true;
    
    info(QString("日志系统初始化成功，日志文件: %1").arg(logFilePath));
}

void Logger::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&s_mutex);
    s_logLevel = level;
}

LogLevel Logger::getLogLevel()
{
    QMutexLocker locker(&s_mutex);
    return s_logLevel;
}

void Logger::debug(const QString& msg)
{
    if (s_logLevel <= LogLevel::Debug) {
        qDebug() << msg;
    }
}

void Logger::info(const QString& msg)
{
    if (s_logLevel <= LogLevel::Info) {
        qInfo() << msg;
    }
}

void Logger::warning(const QString& msg)
{
    if (s_logLevel <= LogLevel::Warning) {
        qWarning() << msg;
    }
}

void Logger::error(const QString& msg)
{
    if (s_logLevel <= LogLevel::Error) {
        qCritical() << msg;
    }
}

void Logger::fatal(const QString& msg)
{
    if (s_logLevel <= LogLevel::Fatal) {
        qFatal() << msg;
    }
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QMutexLocker locker(&s_mutex);
    
    LogLevel level;
    switch (type) {
    case QtDebugMsg:
        level = LogLevel::Debug;
        break;
    case QtInfoMsg:
        level = LogLevel::Info;
        break;
    case QtWarningMsg:
        level = LogLevel::Warning;
        break;
    case QtCriticalMsg:
        level = LogLevel::Error;
        break;
    case QtFatalMsg:
        level = LogLevel::Fatal;
        break;
    default:
        level = LogLevel::Info;
        break;
    }
    
    if (level < s_logLevel) {
        return;
    }
    
    QString formattedMsg = formatMessage(level, msg, context);
    
    std::cout << formattedMsg.toStdString() << std::endl;
    
    if (s_logStream && s_logFile) {
        *s_logStream << formattedMsg << "\n";
        s_logStream->flush();
    }
}

QString Logger::formatMessage(LogLevel level, const QString& msg, const QMessageLogContext& context)
{
    QString levelStr = levelToString(level);
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString threadStr = QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    
    return QString("[%1] [%2] [%3] [%4:%5] %6")
            .arg(timeStr)
            .arg(levelStr)
            .arg(threadStr)
            .arg(context.file)
            .arg(context.line)
            .arg(msg);
}

QString Logger::levelToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:
        return "Debug";
    case LogLevel::Info:
        return "Info ";
    case LogLevel::Warning:
        return "Warn ";
    case LogLevel::Error:
        return "Error";
    case LogLevel::Fatal:
        return "Fatal";
    default:
        return "Info ";
    }
}

} // namespace Utils
} // namespace ChatSystem
