#pragma once
#include <string>
#include <memory>
#include "Common/Protocol/InternalProtocol.h"

namespace storage_server {
namespace rpc {

/**
 * @class RPCClient
 * @brief RPC客户端，用于与MetaServer进行RPC通信
 */
class RPCClient {
public:
    /**
     * @brief 构造函数
     */
    RPCClient();
    
    /**
     * @brief 析构函数
     */
    ~RPCClient();
    
    /**
     * @brief 连接到MetaServer
     * @param host MetaServer主机地址
     * @param port MetaServer端口
     * @return 成功返回true，失败返回false
     */
    bool Connect(const std::string& host, uint16_t port);
    
    /**
     * @brief 断开与MetaServer的连接
     */
    void Disconnect();
    
    /**
     * @brief 发送心跳包
     * @param status 服务器状态
     * @return 成功返回true，失败返回false
     */
    bool SendHeartbeat(const InternalProtocol::Heartbeat& status);
    
    /**
     * @brief 上报文件上传完成
     * @param report 上传完成报告
     * @return 成功返回true，失败返回false
     */
    bool ReportUploadComplete(const InternalProtocol::UploadComplete& report);
    
    /**
     * @brief 发送服务器状态报告
     * @param status 状态报告
     * @return 成功返回true，失败返回false
     */
    bool SendStatusReport(const InternalProtocol::StatusReport& status);
    
    /**
     * @brief 检查连接状态
     * @return 连接返回true，否则返回false
     */
    bool IsConnected() const;
    
private:
    // 实现细节
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace rpc
} // namespace storage_server
