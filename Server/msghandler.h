#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"
#include <stdlib.h>
#include <QDebug>
#include <qfile.h>

class MsgHandler
{
public:
    MsgHandler();
    PDU* pdu;
    QFile m_fUploadFile;
    qint64 m_iUploadFileSize;
    qint64 m_iReceiveSzie;
    PDU* regist();
    PDU* login(QString& strLogiName);
    PDU* findUser();
    PDU* onlineUser();
    PDU* addFriend();
    PDU* agreeAddFriend();
    PDU* flushFriend();
    PDU* delFriend();
    PDU* chat();
    PDU* mkDir();
    PDU* flushFile();
    PDU* moveFile();
    PDU* uploadFile();
    PDU* uploadFileData();
    PDU* shareFile();
    PDU* shareFileAgree();
    bool copyDir(QString strSrcDir,QString strDestDir);
    PDU* deleteDir();
    PDU* deleteFile();
    PDU* rename();
    PDU* downloadFile();
    
};

#endif // MSGHANDLER_H
