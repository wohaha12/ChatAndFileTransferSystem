#include "mytcpsocket.h"
#include "memorypool.h"
#include <QDebug>
#include <QTcpServer>
#include <QCoreApplication>
#include <QDir>
#include "operatedb.h"
#include "mytcpserver.h"
#include "msghandler.h"
#include <unistd.h> // For usleep
#include <cstdio>   // For FILE operations

MyTcpSocket::MyTcpSocket()
{
    mh = new MsgHandler;
}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    if(pdu == NULL){
        return NULL;
    }
    qDebug() << "handleMsg pdu->uiPDULen" <<pdu->uiPDULen
             << "pdu->uiMsgLen" <<pdu->uiMsgLen
             << "pdu->uiType" <<pdu->uiType
             << "pdu->caData" <<pdu->caData
             << "pdu->caData + 32" <<pdu->caData + 32
             << "pdu->caMsg" <<pdu->caMsg;
    mh->pdu = pdu;
    switch(pdu->uiType){
    //注册
    case ENUM_MSG_TYPE_REGIST_REQUEST:
        return mh->regist();
    //登录
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
        return mh->login(m_strLoginName);
    //查找用户
    case ENUM_MSG_TYPE_FIND_USER_REQUEST:
        return mh->findUser();
    //在线用户
    case ENUM_MSG_TYPE_ONLINE_USER_REQUEST:
        return mh->onlineUser();
    //添加好友
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        return mh->addFriend();
    //是否添加好友
    case ENUM_MSG_TYPE_AGREE_ADD_FRIEND_REQUEST:
        return mh->agreeAddFriend();
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        return mh->flushFriend();
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        return mh->delFriend();
    case ENUM_MSG_TYPE_CHAT_REQUEST:
        return mh->chat();
    case ENUM_MSG_TYPE_MKDIR_REQUEST:
        return mh->mkDir();
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        return mh->flushFile();
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
        return mh->moveFile();
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        return mh->uploadFile();
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST:
        return mh->uploadFileData();
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        return mh->shareFile();
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST:
        return mh->shareFileAgree();
    case ENUM_MSG_TYPE_DELETE_DIR_REQUEST:
        return mh->deleteDir();
    case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:
        return mh->deleteFile();
    case ENUM_MSG_TYPE_RENAME_REQUEST:
        return mh->rename();
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
        {
            // 处理下载请求
            PDU* respdu = mh->downloadFile();
            
            // 保存文件路径，用于后续发送文件数据
            if(respdu != NULL && strcmp(respdu->caData, "success") == 0){
                // 保存需要发送的文件路径
                m_strDownloadFilePath = QString(pdu->caMsg);
            }
            
            return respdu;
        }
    default:
        return NULL;
    }
}

void MyTcpSocket::sendMsg(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    
    this->write((char*)pdu,pdu->uiPDULen);
    qDebug() << "sendMsg pdu->uiPDULen" <<pdu->uiPDULen
             << "pdu->uiMsgLen" <<pdu->uiMsgLen
             << "pdu->uiType" <<pdu->uiType
             << "pdu->caData" <<pdu->caData
             << "pdu->caData + 32" <<pdu->caData + 32
             << "pdu->caMsg" <<pdu->caMsg;
    
    // 检查是否是下载响应，如果是且成功，则发送文件数据
    bool needSendFileData = false;
    QString filePath;
    
    if(pdu->uiType == ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND && 
       strcmp(pdu->caData, "success") == 0){
        needSendFileData = !m_strDownloadFilePath.isEmpty();
        filePath = m_strDownloadFilePath;
        m_strDownloadFilePath.clear(); // 清空路径，避免重复发送
    }
    
    MemoryPool::getInstance().deallocate(pdu);
        pdu = NULL;
    
    // 发送文件数据
    if(needSendFileData){
        // 清理文件路径，移除开头的"./"前缀（如果存在）
        QString strCleanPath = filePath;
        if(filePath.startsWith("./filesys/")) {
            strCleanPath = filePath.mid(2); // 跳过开头的"./"
        }
        
        // 构建绝对路径，确保准确性
        QString appDir = QCoreApplication::applicationDirPath(); // 获取应用程序所在目录（debug目录）
        QString serverDir = QDir(appDir).absoluteFilePath(".."); // 向上移动一级到Server目录
        QString strRealFilePath;
        
        if(!strCleanPath.startsWith("filesys/")) {
            // 添加filesys前缀
            strRealFilePath = serverDir + QDir::separator() + "filesys" + QDir::separator() + strCleanPath;
        } else {
            // 直接使用清理后的路径
            strRealFilePath = serverDir + QDir::separator() + strCleanPath;
        }
        
        qDebug() << "[DEBUG] Sending file data, path:" << strRealFilePath;
        
        // 打开文件并发送数据 - 使用绝对路径确保准确性
        QFileInfo fileInfo(strRealFilePath);
        QString absFilePath = fileInfo.absoluteFilePath();
        qDebug() << "[DEBUG] Sending file absolute path:" << absFilePath;
        
        QFile file(strRealFilePath);
        if(file.open(QIODevice::ReadOnly)){
            const int BUFFER_SIZE = 4096;
            QByteArray buffer;
            
            // 发送文件数据块
            while((buffer = file.read(BUFFER_SIZE)).size() > 0){
                PDU* dataPdu = mkPDU(buffer.size());
                dataPdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_RESPOND;
                memcpy(dataPdu->caMsg, buffer.data(), buffer.size());
                
                // 写入数据
                this->write((char*)dataPdu, dataPdu->uiPDULen);
                
                // 短暂休眠，避免网络拥塞
                usleep(1000);
                
                MemoryPool::getInstance().deallocate(dataPdu);
            }
            
            file.close();
        } else {
            qDebug() << "[ERROR] Failed to open file for sending:" << strRealFilePath;
        }
    }
}

MyTcpSocket::~MyTcpSocket()
{
    delete mh;
}

void MyTcpSocket::recvMsg()
{
    qDebug()<<"recvMsg接收消息长度"<<this->bytesAvailable();
    //定义buffer成员变量，全部未处理的数据
    QByteArray data = this->readAll();
    buffer.append(data);
    while(buffer.size()>=int(sizeof(PDU))){
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiPDULen)){
            break;
        }
        PDU* respdu = handleMsg(pdu);
        sendMsg(respdu);
        buffer.remove(0,pdu->uiPDULen);
    }
}

void MyTcpSocket::clientOffline()
{
    OperateDB::getInstance().handleOfflint(m_strLoginName.toStdString().c_str());
    MyTcpServer::getInstance().deleteSocket(this);
}
