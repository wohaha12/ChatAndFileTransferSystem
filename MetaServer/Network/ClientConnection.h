#pragma once

#include <string>
#include <atomic>
#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "CommonProtocol.h"

namespace ChatSystem {
namespace MetaServer {
namespace Network {

/**
 * @brief 客户端连接类
 */
class ClientConnection : public QObject {
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param socketDescriptor 套接字描述符
     * @param parent 父对象指针
     */
    explicit ClientConnection(qintptr socketDescriptor, QObject* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ClientConnection();
    
    /**
     * @brief 连接到客户端
     * @return 成功返回true，失败返回false
     */
    bool connectToClient();
    
    /**
     * @brief 断开连接
     */
    void disconnect();
    
    /**
     * @brief 获取套接字描述符
     * @return 套接字描述符
     */
    qintptr socketDescriptor() const;
    
    /**
     * @brief 获取连接状态
     * @return 连接是否正常
     */
    bool isConnected() const;
    
    /**
     * @brief 发送数据
     * @param header 数据包头部
     * @param data 数据包体
     * @return 成功返回true，失败返回false
     */
    bool sendData(const Protocol::TransHeader& header, const QByteArray& data);
    
    /**
     * @brief 设置用户ID
     * @param userId 用户ID
     */
    void setUserId(uint64_t userId);
    
    /**
     * @brief 获取用户ID
     * @return 用户ID
     */
    uint64_t getUserId() const;
    
signals:
    /**
     * @brief 连接关闭信号
     * @param socketDescriptor 套接字描述符
     */
    void connectionClosed(qintptr socketDescriptor);
    
    /**
     * @brief 收到消息信号
     * @param socketDescriptor 套接字描述符
     * @param header 数据包头部
     * @param data 数据包体
     */
    void messageReceived(qintptr socketDescriptor, const Protocol::TransHeader& header, const QByteArray& data);
    
private slots:
    /**
     * @brief 处理套接字就绪读取
     */
    void onReadyRead();
    
    /**
     * @brief 处理套接字错误
     * @param socketError 套接字错误
     */
    void onSocketError(QAbstractSocket::SocketError socketError);
    
    /**
     * @brief 处理套接字断开连接
     */
    void onDisconnected();
    
private:
    /**
     * @brief 处理接收到的数据
     */
    void processReceivedData();
    
private:
    qintptr m_socketDescriptor;        // 套接字描述符
    QTcpSocket* m_socket;             // TCP套接字
    QByteArray m_receiveBuffer;        // 接收缓冲区
    uint64_t m_userId;                 // 用户ID
    std::atomic<bool> m_connected;     // 连接状态
    std::atomic<uint32_t> m_sequenceNumber; // 序列号生成器
};

} // namespace Network
} // namespace MetaServer
} // namespace ChatSystem
