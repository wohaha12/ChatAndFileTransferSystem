#ifndef CLIENT_NETWORK_STORAGECONNECTION_H
#define CLIENT_NETWORK_STORAGECONNECTION_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QByteArray>
#include "CommonProtocol.h"

class StorageConnection : public QObject
{
    Q_OBJECT

public:
    explicit StorageConnection(QObject* parent = nullptr);
    ~StorageConnection();

    bool connectToServer(const QString& ip, uint16_t port);
    void disconnect();

    bool sendRequest(const TransHeader& header, const QByteArray& data);

    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void messageReceived(const TransHeader& header, const QByteArray& data);
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);

private:
    void processReceivedData();

    QTcpSocket* m_socket;
    QByteArray m_buffer;
    QString m_ip;
    uint16_t m_port;
};

#endif // CLIENT_NETWORK_STORAGECONNECTION_H
