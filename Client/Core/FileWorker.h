#ifndef CLIENT_CORE_FILEWORKER_H
#define CLIENT_CORE_FILEWORKER_H

#include <QObject>
#include <QString>
#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>
#include <QFile>
#include <QCryptographicHash>
#include <QList>
#include <QByteArray>
#include "../Network/NetworkManager.h"

enum TransferState {
    State_Idle = 0,          // 空闲
    State_Hashing,           // 计算哈希
    State_Handshake,          // 握手中
    State_Connecting,         // 连接中
    State_Transferring,       // 传输中
    State_Paused,             // 已暂停
    State_Finished,           // 已完成
    State_Error               // 错误
};

class FileWorker : public QObject
{
    Q_OBJECT

public:
    explicit FileWorker(QObject* parent = nullptr);
    ~FileWorker();

    void uploadFile(const QString& filePath, const QString& parentPath);
    void downloadFile(uint64_t fileId, const QString& savePath);
    void pauseTransfer();
    void resumeTransfer();
    void stopTransfer();

signals:
    void uploadProgress(const QString& filePath, qint64 bytesSent, qint64 bytesTotal);
    void uploadFinished(const QString& filePath, bool success, const QString& errorMsg);
    void downloadProgress(uint64_t fileId, qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(uint64_t fileId, bool success, const QString& errorMsg);

private slots:
    void onUploadChunkAcked(uint32_t chunkIndex);
    void onDownloadChunkReceived(uint32_t chunkIndex, const QByteArray& data);

private:
    QString calculateFileHash(const QString& filePath);
    QList<QByteArray> splitFileToChunks(const QString& filePath, uint32_t chunkSize);
    void sendChunk(const QByteArray& chunk, uint32_t chunkIndex);
    void uploadLoop();
    void downloadLoop();
    void waitForResume();
    bool isPaused() const;
    bool shouldStop() const;
    void setState(TransferState state);

    QString m_filePath;
    uint64_t m_fileSize;
    QString m_fileHash;
    QString m_uploadToken;
    QString m_storageIp;
    uint16_t m_storagePort;
    QAtomicInt m_nextChunkIndex;
    QAtomicInt m_transferredBytes;
    QAtomicInt m_isUploading;
    QAtomicInt m_shouldStop;
    QAtomicInt m_isPaused;
    TransferState m_state;
    QMutex m_mutex;
    QWaitCondition m_waitCondition;
    QTimer* m_heartbeatTimer;
    uint32_t m_totalChunks;
};

#endif // CLIENT_CORE_FILEWORKER_H
