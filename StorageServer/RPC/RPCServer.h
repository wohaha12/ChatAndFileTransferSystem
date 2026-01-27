#pragma once
#include <memory>
#include "Protocol/InternalProtocol.h"
#include "Protocol/TransHeader.h"

namespace storage_server {
namespace rpc {

class StorageServer;

/**
 * @class RPCServer
 * @brief RPC服务器，用于接收MetaServer的RPC指令
 */
class RPCServer {
public:
    /**
     * @brief 构造函数
     * @param server StorageServer实例指针
     */
    explicit RPCServer(StorageServer* server);
    
    /**
     * @brief 析构函数
     */
    ~RPCServer();
    
    /**
     * @brief 启动RPC服务器
     * @param port 监听端口
     * @return 成功返回true，失败返回false
     */
    bool Start(uint16_t port);
    
    /**
     * @brief 停止RPC服务器
     */
    void Stop();
    
    /**
     * @brief 处理RPC请求
     * @param header 传输头
     * @param data 数据缓冲区
     * @param length 数据长度
     * @return 处理结果，成功返回true，失败返回false
     */
    bool HandleRpcRequest(const Protocol::TransHeader& header, 
                         const char* data, 
                         size_t length);
    
private:
    /**
     * @brief 处理心跳请求
     * @param heartbeat 心跳请求
     * @return 处理结果
     */
    bool HandleHeartbeatRequest(const InternalProtocol::Heartbeat& heartbeat);
    
    /**
     * @brief 处理状态查询请求
     * @param request 状态查询请求
     * @return 处理结果
     */
    bool HandleStatusQuery(const InternalProtocol::StatusQuery& request);
    
    /**
     * @brief 处理文件块删除请求
     * @param request 文件块删除请求
     * @return 处理结果
     */
    bool HandleDeleteChunkRequest(const InternalProtocol::DeleteChunkRequest& request);
    
    /**
     * @brief 处理文件迁移请求
     * @param request 文件迁移请求
     * @return 处理结果
     */
    bool HandleFileMigrationRequest(const InternalProtocol::FileMigrationRequest& request);
    
private:
    StorageServer* server_;  ///< StorageServer实例指针
};

} // namespace rpc
} // namespace storage_server
