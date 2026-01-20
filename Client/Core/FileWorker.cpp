#include "FileWorker.h"
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>

FileWorker::FileWorker(QObject* parent)
    : QObject(parent)
    , m_fileSize(0)
    , m_storagePort(0)
    , m_nextChunkIndex(0)
    , m_transferredBytes(0)
    , m_isUploading(false)
    , m_shouldStop(false)
    , m_isPaused(false)
    , m_state(State_Idle)
    , m_heartbeatTimer(nullptr)
    , m_totalChunks(0)
{
}

FileWorker::~FileWorker()
{
    stopTransfer();
}

void FileWorker::uploadFile(const QString& filePath, const QString& parentPath)
{
    m_filePath = filePath;
    m_shouldStop.storeRelaxed(false);
    m_isPaused.storeRelaxed(false);
    m_transferredBytes.storeRelaxed(0);
    m_nextChunkIndex.storeRelaxed(0);

    setState(State_Hashing);

    m_fileHash = calculateFileHash(filePath);
    if (m_fileHash.isEmpty()) {
        setState(State_Error);
        emit uploadFinished(filePath, false, "Failed to calculate file hash");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        setState(State_Error);
        emit uploadFinished(filePath, false, "Failed to open file");
        return;
    }
    m_fileSize = file.size();
    file.close();

    setState(State_Handshake);

    qDebug() << "File hash calculated:" << m_fileHash << "File size:" << m_fileSize;

    uploadLoop();
}

void FileWorker::downloadFile(uint64_t fileId, const QString& savePath)
{
    m_filePath = savePath;
    m_shouldStop.storeRelaxed(false);
    m_isPaused.storeRelaxed(false);
    m_transferredBytes.storeRelaxed(0);

    setState(State_Handshake);

    downloadLoop();
}

void FileWorker::pauseTransfer()
{
    QMutexLocker locker(&m_mutex);
    if (m_state == State_Transferring) {
        m_isPaused.storeRelaxed(true);
        qDebug() << "Transfer paused";
    }
}

void FileWorker::resumeTransfer()
{
    QMutexLocker locker(&m_mutex);
    if (m_state == State_Paused) {
        m_isPaused.storeRelaxed(false);
        m_waitCondition.wakeAll();
        qDebug() << "Transfer resumed";
    }
}

void FileWorker::stopTransfer()
{
    m_shouldStop.storeRelaxed(true);
    QMutexLocker locker(&m_mutex);
    m_waitCondition.wakeAll();
    qDebug() << "Transfer stopped";
}

void FileWorker::onUploadChunkAcked(uint32_t chunkIndex)
{
}

void FileWorker::onDownloadChunkReceived(uint32_t chunkIndex, const QByteArray& data)
{
}

QString FileWorker::calculateFileHash(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    while (!file.atEnd()) {
        hash.addData(file.read(8192));
    }
    file.close();

    return hash.result().toHex();
}

QList<QByteArray> FileWorker::splitFileToChunks(const QString& filePath, uint32_t chunkSize)
{
    QList<QByteArray> chunks;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return chunks;
    }

    while (!file.atEnd()) {
        chunks.append(file.read(chunkSize));
    }
    file.close();

    return chunks;
}

void FileWorker::sendChunk(const QByteArray& chunk, uint32_t chunkIndex)
{
}

void FileWorker::uploadLoop()
{
    uint32_t chunkSize = 1024 * 1024;
    m_totalChunks = (static_cast<uint32_t>(m_fileSize) + chunkSize - 1) / chunkSize;

    setState(State_Transferring);

    while (!shouldStop() && m_nextChunkIndex.loadAcquire() < static_cast<int>(m_totalChunks)) {
        if (isPaused()) {
            setState(State_Paused);
            waitForResume();
            setState(State_Transferring);
            continue;
        }

        uint32_t chunkIndex = static_cast<uint32_t>(m_nextChunkIndex.fetchAndAddRelaxed(1));
        QByteArray chunk;
        sendChunk(chunk, chunkIndex);

        m_transferredBytes.fetchAndAddRelaxed(chunk.size());
        emit uploadProgress(m_filePath, m_transferredBytes.loadAcquire(), static_cast<qint64>(m_fileSize));
    }

    if (m_nextChunkIndex.loadAcquire() >= static_cast<int>(m_totalChunks) && !shouldStop()) {
        setState(State_Finished);
        emit uploadFinished(m_filePath, true, "");
    } else if (shouldStop()) {
        setState(State_Error);
        emit uploadFinished(m_filePath, false, "Transfer stopped by user");
    }
}

void FileWorker::downloadLoop()
{
    setState(State_Transferring);

    while (!shouldStop()) {
        if (isPaused()) {
            setState(State_Paused);
            waitForResume();
            setState(State_Transferring);
            continue;
        }

    }

    if (!shouldStop()) {
        setState(State_Finished);
        emit downloadFinished(0, true, "");
    } else {
        setState(State_Error);
        emit downloadFinished(0, false, "Download stopped by user");
    }
}

void FileWorker::waitForResume()
{
    QMutexLocker locker(&m_mutex);
    while (m_isPaused.loadAcquire() && !m_shouldStop.loadAcquire()) {
        m_waitCondition.wait(&m_mutex);
    }
}

bool FileWorker::isPaused() const
{
    return m_isPaused.loadAcquire();
}

bool FileWorker::shouldStop() const
{
    return m_shouldStop.loadAcquire();
}

void FileWorker::setState(TransferState state)
{
    QMutexLocker locker(&m_mutex);
    m_state = state;
}
