#include "downloader.h"
#include "client.h"

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

// 静态成员初始化
Downloader* Downloader::m_pInstance = NULL;

Downloader::Downloader(QString strSaveFilePath, QString strFileName) 
    : m_strSaveFilePath(strSaveFilePath), 
      m_strFileName(strFileName), 
      m_pFile(NULL), 
      m_iFileSize(0), 
      m_iReceivedSize(0)
{
    // 设置单例实例
    m_pInstance = this;
    // 初始化进度条窗口
    m_pProgressWidget = new QWidget();
    m_pProgressWidget->setWindowTitle("下载进度 - " + m_strFileName);
    m_pProgressWidget->setFixedSize(400, 150);
    
    // 设置进度条
    m_pProgressBar = new QProgressBar();
    m_pProgressBar->setRange(0, 100);
    m_pProgressBar->setValue(0);
    
    // 设置进度标签
    m_pProgressLabel = new QLabel("0% - 准备下载...");
    m_pProgressLabel->setAlignment(Qt::AlignCenter);
    
    // 设置取消按钮
    m_pCancelButton = new QPushButton("取消");
    connect(m_pCancelButton, &QPushButton::clicked, this, &Downloader::onCancelClicked);
    
    // 设置布局
    QVBoxLayout* pMainLayout = new QVBoxLayout();
    QHBoxLayout* pButtonLayout = new QHBoxLayout();
    
    pButtonLayout->addStretch();
    pButtonLayout->addWidget(m_pCancelButton);
    pButtonLayout->addStretch();
    
    pMainLayout->addWidget(m_pProgressLabel);
    pMainLayout->addWidget(m_pProgressBar);
    pMainLayout->addLayout(pButtonLayout);
    
    m_pProgressWidget->setLayout(pMainLayout);
    
    // 打开文件准备写入
    m_pFile = new QFile(m_strSaveFilePath);
    if(!m_pFile->open(QIODevice::WriteOnly)){
        QMessageBox::warning(NULL, "警告", "无法打开文件进行写入: " + m_strSaveFilePath);
        delete m_pFile;
        m_pFile = NULL;
        return;
    }
    
    m_pProgressWidget->show();
}

Downloader::~Downloader()
{
    if(m_pFile != NULL){
        if(m_pFile->isOpen()){
            m_pFile->close();
        }
        delete m_pFile;
        m_pFile = NULL;
    }
    
    if(m_pProgressWidget != NULL){
        m_pProgressWidget->close();
        delete m_pProgressWidget;
        m_pProgressWidget = NULL;
    }
    
    m_pInstance = NULL;
}

void Downloader::start()
{
    downloadFile();
}

void Downloader::downloadFile()
{
    // 准备接收数据
    m_pProgressLabel->setText("0% - 等待服务器响应...");
    
    // 发送下载请求
    PDU* pdu = mkPDU(m_strFileName.toStdString().size() + 1);
    pdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    memcpy(pdu->caData, m_strFileName.toStdString().c_str(), m_strFileName.toStdString().size());
    
    // 这里假设m_strSaveFilePath包含了服务器上的路径信息
    // 在实际应用中，可能需要从路径中提取服务器上的相对路径
    QString serverFilePath = m_strSaveFilePath;
    memcpy(pdu->caMsg, serverFilePath.toStdString().c_str(), serverFilePath.toStdString().size());
    
    Client::getInstance().sendMsg(pdu);
}

void Downloader::processDownloadFileRespond(PDU* pdu)
{
    if(pdu == NULL) return;
    
    // 解析响应
    char caResult[64] = {0};
    strncpy(caResult, pdu->caData, 63);
    
    if(strcmp(caResult, "success") == 0){
        // 成功，开始接收文件数据
        m_iFileSize = atoll(pdu->caMsg);
        m_iReceivedSize = 0;
        m_pProgressLabel->setText("0% - 开始接收数据...");
    } else {
        // 失败，显示错误信息
        QString strError = QString("下载失败: %1").arg(pdu->caMsg);
        QMessageBox::warning(NULL, "警告", strError);
        
        // 关闭下载器
        delete this;
    }
}

void Downloader::processDownloadFileDataRespond(PDU* pdu)
{
    if(pdu == NULL || m_pFile == NULL) return;
    
    // 写入文件数据
    qint64 bytesWritten = m_pFile->write(pdu->caMsg, pdu->uiMsgLen);
    if(bytesWritten > 0){
        m_iReceivedSize += bytesWritten;
        
        // 更新进度
        updateProgress();
        
        // 检查是否下载完成
        if(m_iReceivedSize >= m_iFileSize){
            // 下载完成
            m_pFile->close();
            delete m_pFile;
            m_pFile = NULL;
            
            m_pProgressLabel->setText("100% - 下载完成");
            m_pProgressBar->setValue(100);
            
            QMessageBox::information(NULL, "提示", "文件下载成功！");
            
            // 关闭进度窗口
            m_pProgressWidget->close();
            
            // 清理实例
            delete this;
        }
    } else {
        // 写入失败
        QMessageBox::warning(NULL, "警告", "写入文件失败！");
        
        // 关闭下载器
        delete this;
    }
}

void Downloader::updateProgress()
{
    if(m_iFileSize > 0){
        int progress = static_cast<int>((m_iReceivedSize * 100.0) / m_iFileSize);
        m_pProgressBar->setValue(progress);
        m_pProgressLabel->setText(QString("%1% - %2/%3").arg(progress)
                                 .arg(m_iReceivedSize).arg(m_iFileSize));
    }
}

Downloader* Downloader::getInstance()
{
    return m_pInstance;
}

void Downloader::destroyInstance()
{
    if(m_pInstance != NULL){
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

void Downloader::onCancelClicked()
{
    // 用户取消下载
    QMessageBox::information(m_pProgressWidget, "提示", "下载已取消");
    delete this;
}