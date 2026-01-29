#include "RPCClient.h"
#include "../../Common/Protocol/InternalProtocol.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace ChatSystem {
namespace StorageServer {
namespace RPC {

RPCClient::RPCClient(QObject* parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_connected(false)
    , m_uploadRecordDB(nullptr)
    , m_retryTimer(nullptr)
{
    m_socket = new QTcpSocket(this);
    
    connect(m_socket, &QTcpSocket::readyRead,
            this, &RPCClient::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred,
            this, &RPCClient::onSocketError);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &RPCClient::onSocketDisconnected);
    
    m_uploadRecordDB = new UploadRecordDB(this);
    
    m_retryTimer = new QTimer(this);
    connect(m_retryTimer, &QTimer::timeout, this, &RPCClient::onRetryTimer);
}

RPCClient::~RPCClient()
{
    disconnect();
    
    if (m_retryTimer) {
        m_retryTimer->stop();
        delete m_retryTimer;
    }
    
    if (m_uploadRecordDB) {
        m_uploadRecordDB->close();
        delete m_uploadRecordDB;
    }
    
    delete m_socket;
}

bool RPCClient::initialize(const std::string& dbPath)
{
    if (!m_uploadRecordDB->initialize(dbPath)) {
        std::cerr << "初始化上传记录数据库失败" << std::endl;
        return false;
    }
    
    m_retryTimer->start(RETRY_INTERVAL_MS);
    std::cout << "RPC客户端初始化成功，重试定时器已启动（间隔: " 
              << RETRY_INTERVAL_MS / 1000 << "秒）" << std::endl;
    return true;
}

bool RPCClient::connectToServer(const std::string& host, uint16_t port)
{
    if (m_connected) {
        std::cerr << "已经连接到服务器" << std::endl;
        return false;
    }
    
    m_socket->connectToHost(QString::fromStdString(host), port);
    
    if (!m_socket->waitForConnected(5000)) {
        std::cerr << "连接服务器失败: " << m_socket->errorString().toStdString() << std::endl;
        return false;
    }
    
    m_connected = true;
    emit connectionStateChanged(true);
    
    std::cout << "连接到MetaServer成功: " << host << ":" << port << std::endl;
    return true;
}

void RPCClient::disconnect()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
    }
    
    m_connected = false;
    m_receiveBuffer.clear();
    
    emit connectionStateChanged(false);
}

bool RPCClient::isConnected() const
{
    return m_connected && m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

bool RPCClient::sendHeartbeat(uint32_t serverId, float cpuUsage, float memoryUsage, 
                            float diskUsage, uint32_t connections)
{
    if (!isConnected()) {
        std::cerr << "未连接到服务器" << std::endl;
        return false;
    }
    
    Protocol::HeartbeatRequest request;
    request.serverId = serverId;
    request.cpuUsage = cpuUsage;
    request.memoryUsage = memoryUsage;
    request.diskUsage = diskUsage;
    request.connections = connections;
    
    QByteArray requestBytes(reinterpret_cast<const char*>(&request), sizeof(request));
    
    return sendRequest(requestBytes);
}

bool RPCClient::sendUploadComplete(uint64_t userId, uint64_t fileId, const std::string& fileHash,
                                uint64_t fileSize, const std::string& storagePath,
                                const std::string& uploadToken)
{
    if (!isConnected()) {
        std::cerr << "未连接到服务器" << std::endl;
        return false;
    }
    
    Protocol::UploadCompleteNotify notification;
    strncpy(notification.upload_token, uploadToken.c_str(), sizeof(notification.upload_token) - 1);
    notification.upload_token[sizeof(notification.upload_token) - 1] = '\0';
    notification.user_id = userId;
    notification.fileId = fileId;
    strncpy(notification.file_hash, fileHash.c_str(), sizeof(notification.file_hash) - 1);
    notification.file_hash[sizeof(notification.file_hash) - 1] = '\0';
    notification.file_size = fileSize;
    strncpy(notification.storage_path, storagePath.c_str(), sizeof(notification.storage_path) - 1);
    notification.storage_path[sizeof(notification.storage_path) - 1] = '\0';
    notification.status = 0;  // 成功
    notification.total_chunks = 0;  // 待填充
    notification.success_chunks = 0;  // 待填充
    notification.failed_chunks = 0;
    notification.upload_time = static_cast<uint64_t>(std::time(nullptr));
    notification.avg_speed = 0;  // 待计算
    notification.start_time = 0;  // 待填充
    notification.end_time = 0;  // 待填充
    notification.retry_count = 0;
    notification.reserved = 0;
    memset(notification.error_msg, 0, sizeof(notification.error_msg));
    
    QByteArray notificationBytes(reinterpret_cast<const char*>(&notification), sizeof(notification));
    
    bool success = false;
    
    for (int i = 0; i < 3; ++i) {
        if (sendRequest(notificationBytes)) {
            success = true;
            std::cout << "上传完成通知发送成功（重试次数: " << i << ")" << std::endl;
            break;
        }
        
        std::cerr << "上传完成通知发送失败，重试 " << (i + 1) << "/3" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    if (!success) {
        std::cerr << "上传完成通知发送失败，保存到本地数据库" << std::endl;
        
        LocalUploadRecord record;
        record.uploadToken = uploadToken;
        record.userId = userId;
        record.fileHash = fileHash;
        record.fileSize = fileSize;
        record.fileName = "";  // 待填充
        record.serverId = 0;  // 待填充
        record.serverIp = "";  // 待填充
        record.serverPort = 0;  // 待填充
        record.storagePath = storagePath;
        record.status = 0;  // 待确认
        record.totalChunks = 0;  // 待填充
        record.successChunks = 0;  // 待填充
        record.failedChunks = 0;
        record.uploadTime = static_cast<uint64_t>(std::time(nullptr));
        record.retryCount = 0;
        record.lastRetryTime = 0;
        record.errorMsg = "RPC发送失败";
        
        m_uploadRecordDB->saveUploadRecord(record);
    }
    
    return success;
}

bool RPCClient::sendDownloadComplete(uint64_t userId, uint64_t fileId)
{
    if (!isConnected()) {
        std::cerr << "未连接到服务器" << std::endl;
        return false;
    }
    
    Protocol::DownloadCompleteNotification notification;
    notification.userId = userId;
    notification.fileId = fileId;
    
    QByteArray notificationBytes(reinterpret_cast<const char*>(&notification), sizeof(notification));
    
    return sendRequest(notificationBytes);
}

void RPCClient::onSocketReadyRead()
{
    QByteArray data = m_socket->readAll();
    m_receiveBuffer.append(data);
    
    emit responseReceived(m_receiveBuffer);
}

void RPCClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    std::cerr << "套接字错误: " << socketError 
              << " (" << m_socket->errorString().toStdString() << ")" << std::endl;
    
    disconnect();
}

void RPCClient::onSocketDisconnected()
{
    std::cout << "与MetaServer断开连接" << std::endl;
    
    m_connected = false;
    emit connectionStateChanged(false);
}

void RPCClient::onRetryTimer()
{
    retryPendingNotifications();
}

void RPCClient::retryPendingNotifications()
{
    std::vector<LocalUploadRecord> records = m_uploadRecordDB->getPendingRetryRecords(RETRY_INTERVAL_MS / 1000);
    
    if (records.empty()) {
        return;
    }
    
    std::cout << "开始重试 " << records.size() << " 条待确认的上传记录" << std::endl;
    
    for (const auto& record : records) {
        if (record.retryCount >= MAX_RETRY_COUNT) {
            std::cerr << "上传记录 " << record.uploadToken 
                      << " 已达到最大重试次数，标记为失败" << std::endl;
            m_uploadRecordDB->updateUploadRecordStatus(record.uploadToken, 2, 
                                                        record.retryCount, 
                                                        "超过最大重试次数");
            continue;
        }
        
        Protocol::UploadCompleteNotify notification;
        strncpy(notification.upload_token, record.uploadToken.c_str(), sizeof(notification.upload_token) - 1);
        notification.upload_token[sizeof(notification.upload_token) - 1] = '\0';
        notification.user_id = record.userId;
        strncpy(notification.file_hash, record.fileHash.c_str(), sizeof(notification.file_hash) - 1);
        notification.file_hash[sizeof(notification.file_hash) - 1] = '\0';
        notification.file_size = record.fileSize;
        strncpy(notification.storage_path, record.storagePath.c_str(), sizeof(notification.storage_path) - 1);
        notification.storage_path[sizeof(notification.storage_path) - 1] = '\0';
        notification.status = record.status;
        notification.total_chunks = record.totalChunks;
        notification.success_chunks = record.successChunks;
        notification.failed_chunks = record.failedChunks;
        notification.upload_time = record.uploadTime;
        notification.retry_count = record.retryCount + 1;
        notification.reserved = 0;
        strncpy(notification.error_msg, record.errorMsg.c_str(), sizeof(notification.error_msg) - 1);
        notification.error_msg[sizeof(notification.error_msg) - 1] = '\0';
        
        QByteArray notificationBytes(reinterpret_cast<const char*>(&notification), sizeof(notification));
        
        bool success = false;
        for (int i = 0; i < 3; ++i) {
            if (sendRequest(notificationBytes)) {
                success = true;
                std::cout << "重试上传完成通知成功: " << record.uploadToken 
                          << " (重试次数: " << (record.retryCount + 1) << ")" << std::endl;
                
                m_uploadRecordDB->updateUploadRecordStatus(record.uploadToken, 1, 
                                                            record.retryCount + 1, 
                                                            "");
                break;
            }
            
            std::cerr << "重试上传完成通知失败: " << record.uploadToken 
                      << " (重试次数: " << (i + 1) << "/3)" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        
        if (!success) {
            std::cerr << "重试上传完成通知失败，更新重试次数: " << record.uploadToken << std::endl;
            m_uploadRecordDB->updateUploadRecordStatus(record.uploadToken, 0, 
                                                        record.retryCount + 1, 
                                                        "重试失败");
        }
    }
}

bool RPCClient::sendRequest(const QByteArray& request)
{
    if (!isConnected()) {
        std::cerr << "未连接到服务器" << std::endl;
        return false;
    }
    
    qint64 bytesWritten = m_socket->write(request);
    if (bytesWritten == -1) {
        std::cerr << "发送请求失败: " << m_socket->errorString().toStdString() << std::endl;
        return false;
    }
    
    m_socket->flush();
    return true;
}

} // namespace RPC
} // namespace StorageServer
} // namespace ChatSystem