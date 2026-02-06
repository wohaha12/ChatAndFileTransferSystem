#include <QApplication>
#include <QDir>
#include <QDebug>
#include "UI/MainWindow.h"
#include "UI/LoginDialog.h"
#include "Utils/ConfigManager.h"
#include "Network/NetworkManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("ChatAndFileTransferSystem");
    app.setOrganizationName("ChatAndFileTransferSystem");
    app.setApplicationVersion("1.0.0");

    ConfigManager configManager;
    NetworkManager networkManager;

    // 首先显示登录对话框
    LoginDialog loginDialog;
    int result = loginDialog.exec();

    // 根据登录结果决定是否显示主窗口
    if (result == QDialog::Accepted) {
        MainWindow mainWindow;
        mainWindow.show();
        return app.exec();
    } else {
        // 登录失败或取消，退出程序
        return 0;
    }
}
