#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "LoginDialog.h"
#include "ChatWidget.h"
#include "FileWidget.h"
#include "StyleManager.h"
#include "AnimationManager.h"
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_loginDialog(nullptr)
    , m_chatWidget(nullptr)
    , m_fileWidget(nullptr)
{
    ui->setupUi(this);
    setupUi();
    applyStyles();
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
    ui->usernameLabel->setText(username);
    this->show();
    AnimationManager::instance()->fadeIn(this, 400);
    showChatWidget();
    qDebug() << "Login success:" << username << "UserId:" << userId;
}

void MainWindow::onLogout()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "退出登录",
                                  "确定要退出登录吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        AnimationManager::instance()->fadeOut(this, 300);
        QTimer::singleShot(300, this, [this]() {
            this->hide();
            m_loginDialog->show();
        });
    }
}

void MainWindow::showChatWidget()
{
    if (!m_chatWidget) {
        m_chatWidget = new ChatWidget(this);
        ui->stackedWidget->addWidget(m_chatWidget);
    }
    ui->stackedWidget->setCurrentWidget(m_chatWidget);
    ui->chatNavButton->setChecked(true);
    ui->fileNavButton->setChecked(false);
    AnimationManager::instance()->slideInFromRight(m_chatWidget, 300);
}

void MainWindow::showFileWidget()
{
    if (!m_fileWidget) {
        m_fileWidget = new FileWidget(this);
        ui->stackedWidget->addWidget(m_fileWidget);
    }
    ui->stackedWidget->setCurrentWidget(m_fileWidget);
    ui->chatNavButton->setChecked(false);
    ui->fileNavButton->setChecked(true);
    AnimationManager::instance()->slideInFromRight(m_fileWidget, 300);
}

void MainWindow::setupUi()
{
    setWindowTitle("聊天与文件传输系统");
    resize(1280, 800);
    setMinimumSize(1024, 768);
    
    connect(ui->chatNavButton, &QPushButton::clicked, this, &MainWindow::showChatWidget);
    connect(ui->fileNavButton, &QPushButton::clicked, this, &MainWindow::showFileWidget);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::onLogout);
}

void MainWindow::applyStyles()
{
    StyleManager* styleManager = StyleManager::instance();

    QString sidebarStyle = QString(
        "QWidget#sidebarWidget {"
        "   background-color: %1;"
        "}"
    ).arg(styleManager->cardBackgroundColor().name());
    ui->sidebarWidget->setStyleSheet(sidebarStyle);

    QString contentStyle = QString(
        "QWidget#contentWidget {"
        "   background-color: %1;"
        "}"
    ).arg(styleManager->backgroundColor().name());
    ui->contentWidget->setStyleSheet(contentStyle);

    QString userInfoStyle = QString(
        "QWidget#userInfoWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "   stop:0 %1, stop:1 %2);"
        "}"
        "QLabel#userAvatarLabel {"
        "   background-color: white;"
        "   border-radius: 30px;"
        "   border: 2px solid %1;"
        "}"
    ).arg(styleManager->primaryColor().name(),
           styleManager->secondaryColor().name());
    ui->userInfoWidget->setStyleSheet(userInfoStyle);

    QString lineStyle = QString(
        "QFrame[frameShape=\"4\"], QFrame[frameShape=\"5\"] {"
        "   color: %1;"
        "   background-color: %1;"
        "}"
    ).arg(styleManager->borderColor().name());
    ui->line->setStyleSheet(lineStyle);

    QString navButtonStyle = QString(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: %1;"
        "   border: none;"
        "   border-radius: 8px;"
        "   text-align: left;"
        "   padding-left: 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: %2;"
        "}"
        "QPushButton:checked {"
        "   background-color: %3;"
        "   color: white;"
        "   font-weight: bold;"
        "}"
    ).arg(styleManager->textColor().name(),
           styleManager->hoverColor().name(),
           styleManager->primaryColor().name());
    ui->chatNavButton->setStyleSheet(navButtonStyle);
    ui->fileNavButton->setStyleSheet(navButtonStyle);

    QString logoutButtonStyle = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   text-align: left;"
        "   padding-left: 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: %2;"
        "}"
    ).arg(styleManager->dangerColor().name(),
           "#D32F2F");
    ui->logoutButton->setStyleSheet(logoutButtonStyle);

    styleManager->applyLabelStyle(ui->usernameLabel, false);
}

void MainWindow::connectSignals()
{
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    adjustLayoutForSize();
}

void MainWindow::adjustLayoutForSize()
{
    int windowWidth = this->width();
    
    if (windowWidth < 1200) {
        ui->sidebarWidget->setMaximumWidth(200);
        ui->sidebarWidget->setMinimumWidth(200);
    } else if (windowWidth < 1400) {
        ui->sidebarWidget->setMaximumWidth(220);
        ui->sidebarWidget->setMinimumWidth(220);
    } else {
        ui->sidebarWidget->setMaximumWidth(240);
        ui->sidebarWidget->setMinimumWidth(240);
    }
    
    if (m_chatWidget) {
        m_chatWidget->adjustForWindowSize(windowWidth, this->height());
    }
    
    if (m_fileWidget) {
        m_fileWidget->adjustForWindowSize(windowWidth, this->height());
    }
}
