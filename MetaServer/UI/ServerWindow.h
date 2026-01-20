#pragma once

#include <QMainWindow>

namespace Ui {
class ServerWindow;
}

namespace ChatSystem {
namespace MetaServer {
namespace UI {

/**
 * @brief 服务器监控界面类
 */
class ServerWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit ServerWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ServerWindow();
    
    /**
     * @brief 更新服务器状态
     * @param isRunning 服务器是否运行
     * @param connectionCount 连接数
     * @param onlineUsers 在线用户数
     */
    void updateServerStatus(bool isRunning, int connectionCount, int onlineUsers);
    
    /**
     * @brief 更新存储服务器状态
     * @param serverId 服务器ID
     * @param status 状态
     * @param cpuUsage CPU使用率
     * @param memoryUsage 内存使用率
     * @param diskUsage 磁盘使用率
     */
    void updateStorageServerStatus(uint32_t serverId, const std::string& status, 
                                 float cpuUsage, float memoryUsage, float diskUsage);
    
    /**
     * @brief 添加日志信息
     * @param log 日志内容
     */
    void addLog(const std::string& log);
    
private slots:
    /**
     * @brief 处理启动服务器按钮点击
     */
    void onStartServerClicked();
    
    /**
     * @brief 处理停止服务器按钮点击
     */
    void onStopServerClicked();
    
    /**
     * @brief 处理配置按钮点击
     */
    void onConfigClicked();
    
    /**
     * @brief 处理关于按钮点击
     */
    void onAboutClicked();
    
signals:
    /**
     * @brief 启动服务器信号
     * @param ip IP地址
     * @param port 端口
     */
    void startServer(const std::string& ip, uint16_t port);
    
    /**
     * @brief 停止服务器信号
     */
    void stopServer();
    
private:
    Ui::ServerWindow *ui;  // UI界面指针
    
    /**
     * @brief 初始化UI
     */
    void initializeUI();
    
    /**
     * @brief 初始化信号槽连接
     */
    void initializeConnections();
};

} // namespace UI
} // namespace MetaServer
} // namespace ChatSystem
