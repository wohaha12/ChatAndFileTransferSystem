#ifndef CLIENT_CORE_TRANSFERMANAGER_H
#define CLIENT_CORE_TRANSFERMANAGER_H

#include <QObject>
#include <QString>
#include <QAtomicInt>
#include <QAtomicBool>
#include <QTimer>
#include "../Network/NetworkManager.h"
#include "FileWorker.h"

class TransferManager : public QObject
{
    Q_OBJECT

public:
    explicit TransferManager(QObject* parent = nullptr);
    ~TransferManager();

    void setNetworkManager(NetworkManager* networkManager);

    void requestUpload(const QString& filePath, const QString& parentPath);
    void requestDownload(uint64_t fileId, const QString& savePath);

    bool isTransferring() const;
    qint64 getTransferSpeed() const;

    void pauseTransfer();
    void resumeTransfer();
    void stopTransfer();

signals:
    void uploadProgress(const QString& filePath, qint64 bytesSent, qint64 bytesTotal);
    void uploadFinished(const QString& filePath, bool success, const QString& errorMsg);
    void downloadProgress(uint64_t fileId, qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(uint64_t fileId, bool success, const QString& errorMsg);

public slots:
    void handleUploadResponse(const TransHeader& header, const QByteArray& data);
    void handleDownloadResponse(const TransHeader& header, const QByteArray& data);

private:
    NetworkManager* m_networkManager;
    FileWorker* m_fileWorker;
    QThread* m_workerThread;
    QAtomicBool m_isTransferring;
    QAtomicInt m_activeTransfers;
    QTimer* m_speedTimer;
    qint64 m_lastBytesTransferred;
    qint64 m_transferSpeed;
};

#endif // CLIENT_CORE_TRANSFERMANAGER_H
