#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <qtcpsocket.h>
#include "protocol.h"


class MsgHandler;
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString m_strLoginName;
    QString m_strDownloadFilePath; // 用于存储要下载的文件路径
    PDU* handleMsg(PDU* pdu);
    void sendMsg(PDU* respdu);
    MsgHandler* mh;
    ~MyTcpSocket();
    QByteArray buffer;

public slots:
    void recvMsg();
    void clientOffline();

};

#endif // MYTCPSOCKET_H
