#include <iostream>
#include <string>
#include "StorageServer/Core/StorageServer.h"
#include "StorageServer/Config/StorageServerConfig.h"

/**
 * @brief 主函数，StorageServer的入口点
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码
 */
int main(int argc, char* argv[]) {
    std::cout << "StorageServer starting..." << std::endl;
    
    // 解析命令行参数
    std::string config_path = "storage_server.conf";
    if (argc > 1) {
        config_path = argv[1];
    }
    
    // 加载配置
    storage_server::config::StorageServerConfig config;
    if (!config.LoadConfig(config_path)) {
        std::cerr << "Failed to load config file: " << config_path << std::endl;
        return 1;
    }
    
    try {
        // 创建并初始化StorageServer
        storage_server::core::StorageServer server(config);
        
        // 启动服务器
        if (!server.Start()) {
            std::cerr << "Failed to start StorageServer" << std::endl;
            return 1;
        }
        
        std::cout << "StorageServer started successfully, listening on port " 
                  << config.GetListenPort() << std::endl;
        
        // 等待退出信号
        server.WaitForShutdown();
        
        // 停止服务器
        server.Stop();
        
        std::cout << "StorageServer stopped gracefully" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
}
