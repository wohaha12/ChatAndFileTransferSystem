#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "protocol.h"

#include <QObject>
#include <QFile>
#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader(QString strSaveFilePath, QString strFileName);
    ~Downloader();
    void start();
    void processDownloadFileRespond(PDU* pdu);
    void processDownloadFileDataRespond(PDU* pdu);
    
    QString m_strSaveFilePath; // 保存文件的路径
    QString m_strFileName;    // 文件名
    QFile* m_pFile;           // 文件指针
    qint64 m_iFileSize;       // 文件总大小
    qint64 m_iReceivedSize;   // 已接收大小

public slots:
    void downloadFile();

private:
    void updateProgress();

public:
    // 单例模式
    static Downloader* getInstance();
    static void destroyInstance();
    
private:
    static Downloader* m_pInstance;

    // 进度条窗口相关
    QWidget* m_pProgressWidget;
    QProgressBar* m_pProgressBar;
    QLabel* m_pProgressLabel;
    QPushButton* m_pCancelButton;

private slots:
    void onCancelClicked();

}; // end of class

#endif // DOWNLOADER_H