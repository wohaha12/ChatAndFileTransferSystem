#include "ClientConnection.h"
#include "../../Common/Protocol/TransHeader.h"
#include <iostream>

namespace ChatSystem {
namespace MetaServer {
namespace Network {

ClientConnection::ClientConnection(qintptr socketDescriptor, QObject* parent)
    : QObject(parent)
    , m_socketDescriptor(socketDescriptor)
    , m_socket(nullptr)
    , m_userId(0)
    , m_connected(false)
    , m_sequenceNumber(0)
{
    m_socket = new QTcpSocket(this);
    
    if (!m_socket->setSocketDescriptor(socketDescriptor)) {
        std::cerr << "设置套接字描述符失败: " << socketDescriptor << std::endl;
        delete m_socket;
        m_socket = nullptr;
        return;
    }
    
    connect(m_socket, &QTcpSocket::readyRead,
            this, &ClientConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred,
            this, &ClientConnection::onSocketError);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &ClientConnection::onDisconnected);
    
    m_connected = true;
}

ClientConnection::~ClientConnection()
{
    disconnect();
    delete m_socket;
}

bool ClientConnection::connectToClient()
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        std::cerr << "套接字未连接" << std::endl;
        return false;
    }
    
    m_connected = true;
    return true;
}

void ClientConnection::disconnect()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
    }
    
    m_connected = false;
}

qintptr ClientConnection::socketDescriptor() const
{
    return m_socketDescriptor;
}

bool ClientConnection::isConnected() const
{
    return m_connected && m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

bool ClientConnection::sendData(const Protocol::TransHeader& header, const QByteArray& data)
{
    if (!isConnected()) {
        std::cerr << "连接未建立，无法发送数据" << std::endl;
        return false;
    }
    
    QByteArray headerData;
    headerData.resize(sizeof(Protocol::TransHeader));
    memcpy(headerData.data(), &header, sizeof(Protocol::TransHeader));
    
    qint64 totalBytes = m_socket->write(headerData + data);
    if (totalBytes == -1) {
        std::cerr << "发送数据失败: " << m_socket->errorString().toStdString() << std::endl;
        return false;
    }
    
    m_socket->flush();
    return true;
}

void ClientConnection::setUserId(uint64_t userId)
{
    m_userId = userId;
}

uint64_t ClientConnection::getUserId() const
{
    return m_userId;
}

void ClientConnection::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    m_receiveBuffer.append(data);
    
    processReceivedData();
}

void ClientConnection::onSocketError(QAbstractSocket::SocketError socketError)
{
    std::cerr << "套接字错误: " << socketError 
              << " (" << m_socket->errorString().toStdString() << ")" << std::endl;
    
    disconnect();
}

void ClientConnection::onDisconnected()
{
    std::cout << "客户端断开连接: " << m_socketDescriptor << std::endl;
    
    m_connected = false;
    emit connectionClosed(m_socketDescriptor);
}

void ClientConnection::processReceivedData()
{
    while (m_receiveBuffer.size() >= sizeof(Protocol::TransHeader)) {
        Protocol::TransHeader header;
        memcpy(&header, m_receiveBuffer.constData(), sizeof(Protocol::TransHeader));
        
        header.toHostOrder();
        
        if (!header.validateMagic()) {
            std::cerr << "无效的魔数: 0x" << std::hex << header.magic << std::endl;
            m_receiveBuffer.clear();
            return;
        }
        
        uint32_t bodySize = header.len;
        uint32_t totalSize = sizeof(Protocol::TransHeader) + bodySize;
        
        if (m_receiveBuffer.size() < totalSize) {
            break;
        }
        
        QByteArray body = m_receiveBuffer.mid(sizeof(Protocol::TransHeader), bodySize);
        m_receiveBuffer.remove(0, totalSize);
        
        if (!header.validateChecksum(body.constData())) {
            std::cerr << "校验和失败" << std::endl;
            continue;
        }
        
        emit messageReceived(m_socketDescriptor, header, body);
    }
}

} // namespace Network
} // namespace MetaServer
} // namespace ChatSystem