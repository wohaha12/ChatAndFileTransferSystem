#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "LoginDialog.h"
#include "ChatWidget.h"
#include "FileWidget.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_loginDialog(nullptr)
    , m_chatWidget(nullptr)
    , m_fileWidget(nullptr)
{
    ui->setupUi(this);
    setupUi();
    connectSignals();

    m_loginDialog = new LoginDialog(this);
    connect(m_loginDialog, &LoginDialog::loginSuccess, this, &MainWindow::onLoginSuccess);

    m_loginDialog->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_loginDialog;
    delete m_chatWidget;
    delete m_fileWidget;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "退出确认",
                                  "确定要退出程序吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::onLoginSuccess(const QString& username, uint64_t userId)
{
    m_loginDialog->hide();
    this->show();
    qDebug() << "Login success:" << username << "UserId:" << userId;
}

void MainWindow::onLogout()
{
    this->hide();
    m_loginDialog->show();
}

void MainWindow::showChatWidget()
{
    if (!m_chatWidget) {
        m_chatWidget = new ChatWidget(this);
        ui->stackedWidget->addWidget(m_chatWidget);
    }
    ui->stackedWidget->setCurrentWidget(m_chatWidget);
}

void MainWindow::showFileWidget()
{
    if (!m_fileWidget) {
        m_fileWidget = new FileWidget(this);
        ui->stackedWidget->addWidget(m_fileWidget);
    }
    ui->stackedWidget->setCurrentWidget(m_fileWidget);
}

void MainWindow::setupUi()
{
    setWindowTitle("聊天与文件传输系统");
    resize(1024, 768);
}

void MainWindow::connectSignals()
{
}
