#include "TransferManager.h"
#include <QDebug>
#include <QThread>

TransferManager::TransferManager(QObject *parent)
    : QObject(parent), m_networkManager(nullptr), m_fileWorker(nullptr),
      m_workerThread(nullptr), m_isTransferring(false), m_activeTransfers(0),
      m_speedTimer(nullptr), m_lastBytesTransferred(0), m_transferSpeed(0) {
  m_speedTimer = new QTimer(this);
  connect(m_speedTimer, &QTimer::timeout, this, [this]() {
    qint64 currentBytes = m_lastBytesTransferred;
    m_transferSpeed = currentBytes;
    m_lastBytesTransferred = 0;
  });
  m_speedTimer->start(1000);
}

TransferManager::~TransferManager() {
  stopTransfer();

  if (m_speedTimer) {
    delete m_speedTimer;
  }
}

void TransferManager::setNetworkManager(NetworkManager *networkManager) {
  m_networkManager = networkManager;
}

void TransferManager::requestUpload(const QString &filePath,
                                    const QString &parentPath) {
  if (!m_fileWorker) {
    m_workerThread = new QThread(this);
    m_fileWorker = new FileWorker();
    m_fileWorker->moveToThread(m_workerThread);

    connect(m_fileWorker, &FileWorker::uploadProgress, this,
            &TransferManager::uploadProgress);
    connect(m_fileWorker, &FileWorker::uploadFinished, this,
            &TransferManager::uploadFinished);

    connect(m_workerThread, &QThread::finished, this,
            [this]() { m_workerThread->deleteLater(); });

    m_workerThread->start();
  }

  QMetaObject::invokeMethod(m_fileWorker, "uploadFile", Qt::QueuedConnection,
                            Q_ARG(QString, filePath),
                            Q_ARG(QString, parentPath));

  m_isTransferring.storeRelaxed(true);
  m_activeTransfers.fetchAndAddRelaxed(1);
}

void TransferManager::requestDownload(uint64_t fileId,
                                      const QString &savePath) {
  if (!m_fileWorker) {
    m_workerThread = new QThread(this);
    m_fileWorker = new FileWorker();
    m_fileWorker->moveToThread(m_workerThread);

    connect(m_fileWorker, &FileWorker::downloadProgress, this,
            &TransferManager::downloadProgress);
    connect(m_fileWorker, &FileWorker::downloadFinished, this,
            &TransferManager::downloadFinished);

    connect(m_workerThread, &QThread::finished, this,
            [this]() { m_workerThread->deleteLater(); });

    m_workerThread->start();
  }

  QMetaObject::invokeMethod(m_fileWorker, "downloadFile", Qt::QueuedConnection,
                            Q_ARG(uint64_t, fileId), Q_ARG(QString, savePath));

  m_isTransferring.storeRelaxed(true);
  m_activeTransfers.fetchAndAddRelaxed(1);
}

bool TransferManager::isTransferring() const {
  return m_isTransferring.loadAcquire();
}

qint64 TransferManager::getTransferSpeed() const { return m_transferSpeed; }

void TransferManager::pauseTransfer() {
  if (m_fileWorker) {
    QMetaObject::invokeMethod(m_fileWorker, "pauseTransfer",
                              Qt::QueuedConnection);
  }
}

void TransferManager::resumeTransfer() {
  if (m_fileWorker) {
    QMetaObject::invokeMethod(m_fileWorker, "resumeTransfer",
                              Qt::QueuedConnection);
  }
}

void TransferManager::stopTransfer() {
  if (m_fileWorker) {
    QMetaObject::invokeMethod(m_fileWorker, "stopTransfer",
                              Qt::QueuedConnection);
  }

  m_isTransferring.storeRelaxed(false);
}

void TransferManager::handleUploadResponse(
    const ChatSystem::Protocol::TransHeader &header, const QByteArray &data) {}

void TransferManager::handleDownloadResponse(
    const ChatSystem::Protocol::TransHeader &header, const QByteArray &data) {}
