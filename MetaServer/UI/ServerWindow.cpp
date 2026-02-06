#include "ServerWindow.h"
#include "ui_ServerWindow.h"
#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTimer>

namespace ChatSystem {
namespace MetaServer {
namespace UI {

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ServerWindow) {
  ui->setupUi(this);

  initializeUI();
  initializeConnections();

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, [this]() {
    ui->statusLabel->setText(
        QString("运行中 | 时间: %1")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
  });
  timer->start(1000);
}

ServerWindow::~ServerWindow() { delete ui; }

void ServerWindow::updateServerStatus(bool isRunning, int connectionCount,
                                      int onlineUsers) {
  QString status = isRunning ? "运行中" : "已停止";
  QString statusColor = isRunning ? "green" : "red";

  ui->statusLabel->setText(QString("状态: %1 | 连接数: %2 | 在线用户: %3")
                               .arg(status)
                               .arg(connectionCount)
                               .arg(onlineUsers));

  ui->statusLabel->setStyleSheet(QString("color: %1;").arg(statusColor));

  ui->connectionCountLabel->setText(QString::number(connectionCount));
  ui->onlineUsersLabel->setText(QString::number(onlineUsers));

  if (isRunning) {
    ui->startServerButton->setEnabled(false);
    ui->stopServerButton->setEnabled(true);
  } else {
    ui->startServerButton->setEnabled(true);
    ui->stopServerButton->setEnabled(false);
  }
}

void ServerWindow::updateStorageServerStatus(uint32_t serverId,
                                             const std::string &status,
                                             float cpuUsage, float memoryUsage,
                                             float diskUsage) {
  QString statusText = QString::fromStdString(status);
  QString statusColor = (status == "online") ? "green" : "red";

  QString serverInfo =
      QString("服务器 %1: %2 | CPU: %3% | 内存: %4% | 磁盘: %5%")
          .arg(serverId)
          .arg(statusText)
          .arg(cpuUsage, 0, 'f', 1)
          .arg(memoryUsage, 0, 'f', 1)
          .arg(diskUsage, 0, 'f', 1);

  ui->storageServerListWidget->addItem(serverInfo);

  QListWidgetItem *item = ui->storageServerListWidget->item(
      ui->storageServerListWidget->count() - 1);
  if (item) {
    item->setForeground(QBrush(QColor(statusColor)));
  }
}

void ServerWindow::addLog(const std::string &log) {
  QString timestamp =
      QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
  QString logEntry =
      QString("[%1] %2").arg(timestamp).arg(QString::fromStdString(log));

  ui->logTextEdit->appendPlainText(logEntry);

  ui->logTextEdit->moveCursor(QTextCursor::End);
}

void ServerWindow::onStartServerClicked() {
  std::string ip = ui->ipLineEdit->text().toStdString();
  std::string portStr = ui->portLineEdit->text().toStdString();

  if (ip.empty() || portStr.empty()) {
    addLog("错误: IP地址和端口不能为空");
    return;
  }

  uint16_t port = static_cast<uint16_t>(std::stoul(portStr));

  addLog("启动服务器: " + ip + ":" + portStr);

  emit startServer(ip, port);
}

void ServerWindow::onStopServerClicked() {
  addLog("停止服务器");

  emit stopServer();
}

void ServerWindow::onConfigClicked() {
  addLog("打开配置对话框");

  QString configInfo = QString("配置信息:\n"
                               "数据库主机: %1\n"
                               "数据库端口: %2\n"
                               "数据库用户: %3\n"
                               "Redis主机: %4\n"
                               "Redis端口: %5")
                           .arg("127.0.0.1")
                           .arg("3306")
                           .arg("root")
                           .arg("127.0.0.1")
                           .arg("6379");

  QMessageBox::information(this, "配置信息", configInfo);
}

void ServerWindow::onAboutClicked() {
  QString aboutInfo = QString("聊天与文件传输系统 - MetaServer\n"
                              "版本: 1.0.0\n"
                              "基于Qt 6 + C++17开发\n"
                              "\n"
                              "Copyright © 2026");

  QMessageBox::about(this, "关于", aboutInfo);
}

void ServerWindow::initializeUI() {
  setWindowTitle("聊天与文件传输系统 - MetaServer");
  resize(1024, 768);

  ui->statusLabel->setText("状态: 已停止");
  ui->statusLabel->setStyleSheet("color: red;");

  ui->connectionCountLabel->setText("0");
  ui->onlineUsersLabel->setText("0");

  ui->ipLineEdit->setText("0.0.0.0");
  ui->portLineEdit->setText("8888");

  ui->logTextEdit->setReadOnly(true);
  ui->logTextEdit->setMaximumBlockCount(1000);

  addLog("系统初始化完成");
}

void ServerWindow::initializeConnections() {
  connect(ui->startServerButton, &QPushButton::clicked, this,
          &ServerWindow::onStartServerClicked);
  connect(ui->stopServerButton, &QPushButton::clicked, this,
          &ServerWindow::onStopServerClicked);
  connect(ui->configButton, &QPushButton::clicked, this,
          &ServerWindow::onConfigClicked);
  connect(ui->aboutButton, &QPushButton::clicked, this,
          &ServerWindow::onAboutClicked);

  connect(ui->clearLogButton, &QPushButton::clicked, ui->logTextEdit,
          &QPlainTextEdit::clear);
}

} // namespace UI
} // namespace MetaServer
} // namespace ChatSystem