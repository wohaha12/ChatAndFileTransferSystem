#include <QApplication>
#include <QDir>
#include <QDebug>
#include "UI/MainWindow.h"
#include "Utils/ConfigManager.h"
#include "Network/NetworkManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("ChatAndFileTransferSystem");
    app.setOrganizationName("ChatAndFileTransferSystem");
    app.setApplicationVersion("1.0.0");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    ConfigManager configManager;
    NetworkManager networkManager;

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
