#ifndef CLIENT_NETWORK_NETWORKMANAGER_H
#define CLIENT_NETWORK_NETWORKMANAGER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QAtomicInt>
#include <QMutex>
#include <QTcpSocket>
#include "CommonProtocol.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    bool connectToMetaServer(const QString& ip, uint16_t port);
    bool connectToStorageServer(const QString& ip, uint16_t port);
    void disconnectFromServer();

    bool sendToMetaServer(const TransHeader& header, const QByteArray& data);
    bool sendToStorageServer(const TransHeader& header, const QByteArray& data);

    bool isMetaServerConnected() const;
    bool isStorageServerConnected() const;

    uint32_t generateSequenceNumber();

signals:
    void metaServerConnectionChanged(bool connected);
    void storageServerConnectionChanged(bool connected);
    void metaServerMessageReceived(const TransHeader& header, const QByteArray& data);
    void storageServerMessageReceived(const TransHeader& header, const QByteArray& data);

private slots:
    void onMetaSocketReadyRead();
    void onStorageSocketReadyRead();
    void onMetaSocketError(QAbstractSocket::SocketError error);
    void onStorageSocketError(QAbstractSocket::SocketError error);

private:
    void processReceivedData(QTcpSocket* socket, QByteArray& buffer, bool isMeta);

    QTcpSocket* m_metaSocket;
    QTcpSocket* m_storageSocket;
    QByteArray m_metaBuffer;
    QByteArray m_storageBuffer;
    QAtomicInt m_sequenceNumber;
    QMutex m_mutex;
};

#endif // CLIENT_NETWORK_NETWORKMANAGER_H
