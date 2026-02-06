#include <QCoreApplication>
#include <iostream>
#include <string>

#include "Config/StorageServerConfig.h"
#include "Core/StorageServer.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

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
    ChatSystem::StorageServer::Core::StorageServer server;

    // 设置服务器ID
    server.setServerId(1);
    // 设置MetaServer信息
    server.setMetaServerInfo(config.GetMetaServerHost(),
                             config.GetMetaServerRpcPort());
    // 设置数据目录
    server.setDataDir(config.GetDataDir());

    // 启动服务器
    // StorageServer::start(ip, port)
    // Bind to all interfaces usually.
    if (!server.start("0.0.0.0", config.GetListenPort())) {
      std::cerr << "Failed to start StorageServer" << std::endl;
      return 1;
    }

    std::cout << "StorageServer started successfully, listening on port "
              << config.GetListenPort() << std::endl;

    // 进入Qt事件循环
    return a.exec();

  } catch (const std::exception &e) {
    std::cerr << "Exception caught: " << e.what() << std::endl;
    return 1;
  }
}
