#include <QApplication>
#include <QDebug>

#include "Core/MetaServer.h"
#include "Config/ServerConfig.h"
#include "UI/ServerWindow.h"

using namespace ChatSystem::MetaServer;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    try {
        // 加载配置
        Config::ServerConfig* config = Config::ServerConfig::instance();
        if (!config->loadFromFile("metaserver.conf")) {
            qDebug() << "Failed to load config file, using default config";
        }
        
        // 创建服务器实例
        Core::MetaServer metaServer;
        
        // 创建UI窗口
        UI::ServerWindow window;
        
        // 连接信号槽
        QObject::connect(&window, &UI::ServerWindow::startServer, 
                        &metaServer, &Core::MetaServer::start);
        QObject::connect(&window, &UI::ServerWindow::stopServer, 
                        &metaServer, &Core::MetaServer::stop);
        
        // 显示窗口
        window.show();
        
        return a.exec();
    } catch (const std::exception& e) {
        qCritical() << "Exception:" << e.what();
        return 1;
    }
}
