#include "RPCServer.h"
#include "../../Common/Protocol/InternalProtocol.h"
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace RPC {

RPCServer::RPCServer(QObject* parent)
    : QObject(parent)
    , m_server(nullptr)
    , m_running(false)
{
    m_server = new QTcpServer(this);
    
    connect(m_server, &QTcpServer::newConnection,
            this, &RPCServer::onNewConnection);
}

RPCServer::~RPCServer()
{
    stop();
    
    for (auto& pair : m_receiveBuffers) {
        delete pair.first;
    }
    m_receiveBuffers.clear();
    
    delete m_server;
}

bool RPCServer::start(const std::string& ip, uint16_t port)
{
    if (m_running) {
        std::cerr << "RPC服务器已在运行" << std::endl;
        return false;
    }
    
    QHostAddress address;
    if (ip == "0.0.0.0" || ip == "*") {
        address = QHostAddress::Any;
    } else {
        address = QHostAddress(QString::fromStdString(ip));
        if (address.isNull()) {
            std::cerr << "无效的IP地址: " << ip << std::endl;
            return false;
        }
    }
    
    if (!m_server->listen(address, port)) {
        std::cerr << "RPC服务器监听失败: " << m_server->errorString().toStdString() << std::endl;
        return false;
    }
    
    m_running = true;
    std::cout << "RPC服务器启动成功: " << ip << ":" << port << std::endl;
    return true;
}

void RPCServer::stop()
{
    if (!m_running) {
        return;
    }
    
    m_running = false;
    
    for (auto& pair : m_receiveBuffers) {
        delete pair.first;
    }
    m_receiveBuffers.clear();
    
    m_server->close();
    
    std::cout << "RPC服务器已停止" << std::endl;
}

bool RPCServer::isRunning() const
{
    return m_running;
}

void RPCServer::onNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();
    if (!socket) {
        return;
    }
    
    connect(socket, &QTcpSocket::readyRead,
            this, &RPCServer::onSocketReadyRead);
    connect(socket, &QTcpSocket::errorOccurred,
            this, &RPCServer::onSocketError);
    connect(socket, &QTcpSocket::disconnected,
            this, &RPCServer::onSocketDisconnected);
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_receiveBuffers[socket] = QByteArray();
    }
    
    std::cout << "RPC新连接到达" << std::endl;
}

void RPCServer::onSocketReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    
    QByteArray data = socket->readAll();
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_receiveBuffers[socket].append(data);
    }
    
    handleRequest(socket, data);
}

void RPCServer::onSocketError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    
    std::cerr << "RPC套接字错误: " << socketError 
              << " (" << socket->errorString().toStdString() << ")" << std::endl;
    
    socket->disconnectFromHost();
}

void RPCServer::onSocketDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    
    std::cout << "RPC连接断开" << std::endl;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_receiveBuffers.erase(socket);
    }
    
    socket->deleteLater();
}

void RPCServer::handleRequest(QTcpSocket* socket, const QByteArray& data)
{
    if (data.size() < sizeof(uint8_t)) {
        return;
    }
    
    uint8_t messageType = *reinterpret_cast<const uint8_t*>(data.constData());
    
    switch (messageType) {
        case 0x01: {
            if (data.size() < sizeof(Protocol::HeartbeatRequest)) {
                return;
            }
            
            const Protocol::HeartbeatRequest* request = 
                reinterpret_cast<const Protocol::HeartbeatRequest*>(data.constData());
            
            emit heartbeatReceived(request->serverId, request->cpuUsage, 
                              request->memoryUsage, request->diskUsage, 
                              request->connections);
            
            std::cout << "收到心跳: 服务器ID=" << request->serverId 
                      << ", 连接数=" << request->connections << std::endl;
            break;
        }
        
        case 0x02: {
            if (data.size() < sizeof(Protocol::UploadCompleteNotification)) {
                return;
            }
            
            const Protocol::UploadCompleteNotification* notification = 
                reinterpret_cast<const Protocol::UploadCompleteNotification*>(data.constData());
            
            std::string fileHash(notification->fileHash);
            std::string storagePath(notification->storagePath);
            
            emit uploadCompleteReceived(notification->userId, notification->fileId, 
                                   fileHash, notification->fileSize, storagePath);
            
            std::cout << "收到上传完成通知: 用户ID=" << notification->userId 
                      << ", 文件ID=" << notification->fileId << std::endl;
            break;
        }
        
        case 0x03: {
            if (data.size() < sizeof(Protocol::DownloadCompleteNotification)) {
                return;
            }
            
            const Protocol::DownloadCompleteNotification* notification = 
                reinterpret_cast<const Protocol::DownloadCompleteNotification*>(data.constData());
            
            emit downloadCompleteReceived(notification->userId, notification->fileId);
            
            std::cout << "收到下载完成通知: 用户ID=" << notification->userId 
                      << ", 文件ID=" << notification->fileId << std::endl;
            break;
        }
        
        default:
            std::cerr << "未知的RPC消息类型: 0x" << std::hex 
                      << static_cast<int>(messageType) << std::endl;
            break;
    }
}

} // namespace RPC
} // namespace MetaServer
} // namespace ChatSystem