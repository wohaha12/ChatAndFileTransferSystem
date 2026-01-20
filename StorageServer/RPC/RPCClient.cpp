#include "RPCClient.h"
#include "../../Common/Protocol/InternalProtocol.h"
#include <iostream>

namespace ChatSystem {
namespace StorageServer {
namespace RPC {

RPCClient::RPCClient(QObject* parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_connected(false)
{
    m_socket = new QTcpSocket(this);
    
    connect(m_socket, &QTcpSocket::readyRead,
            this, &RPCClient::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred,
            this, &RPCClient::onSocketError);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &RPCClient::onSocketDisconnected);
}

RPCClient::~RPCClient()
{
    disconnect();
    delete m_socket;
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
                                uint64_t fileSize, const std::string& storagePath)
{
    if (!isConnected()) {
        std::cerr << "未连接到服务器" << std::endl;
        return false;
    }
    
    Protocol::UploadCompleteNotification notification;
    notification.userId = userId;
    notification.fileId = fileId;
    strncpy(notification.fileHash, fileHash.c_str(), sizeof(notification.fileHash) - 1);
    notification.fileHash[sizeof(notification.fileHash) - 1] = '\0';
    notification.fileSize = fileSize;
    strncpy(notification.storagePath, storagePath.c_str(), sizeof(notification.storagePath) - 1);
    notification.storagePath[sizeof(notification.storagePath) - 1] = '\0';
    
    QByteArray notificationBytes(reinterpret_cast<const char*>(&notification), sizeof(notification));
    
    return sendRequest(notificationBytes);
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