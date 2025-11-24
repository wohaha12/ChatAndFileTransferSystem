#include "client.h"
#include "index.h"
#include "reshandler.h"
#include "stdlib.h"
#include "downloader.h"

#include <QDir>
#include <qmessagebox.h>
#include <QDebug>
ResHandler::ResHandler()
{

}

void ResHandler::regist()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(),"提示","注册成功");
    }else{
        QMessageBox::warning(&Client::getInstance(),"提示","注册失败");
    }
}

void ResHandler::login()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().show();
        Index::getInstance().setWindowTitle(QString("首页-%1").arg(Client::getInstance().m_strLoginName));
        Client::getInstance().hide();
    }else{
        QMessageBox::warning(&Client::getInstance(),"提示","登录失败");
    }
}

void ResHandler::findUser()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    int ret;
    // 从PDU数据区拷贝返回值
    memcpy(&ret, pdu->caData+32, sizeof(int));
    qDebug()<<"findUser ret"<<ret;
    // 根据不同的返回值显示相应提示
    if (ret == 2) {
        QMessageBox::information(&Index::getInstance(), "提示", QString("%1不存在").arg(caName));
    } else if (ret == -1) {
        QMessageBox::information(&Index::getInstance(), "提示", "查找失败");
    }  else if (ret == 0) {
        QMessageBox::information(&Index::getInstance(), "提示", QString("%1不在线").arg(caName));
    }else if (ret == 1) {
        int ret = QMessageBox::information(&Index::getInstance(), "提示", QString("%1在线").arg(caName),"添加好友","取消");
        qDebug()<<"ret"<<ret;
        if(ret == 0){
            QString strCurName = Client::getInstance().m_strLoginName;
            PDU* pdu = mkPDU();
            pdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
            memcpy(pdu->caData+32,caName,32);
            Client::getInstance().sendMsg(pdu);
        }
    }
}

void ResHandler::onlineUser(QString& strLoginName)
{
    int iSize = pdu->uiMsgLen/32;
    QStringList res;
    char caName[32] = {'\0'};
    for(int i = 0;i<iSize;i++){
        memcpy(caName,pdu->caMsg+i*32,32);
        if(QString(caName) == strLoginName){
            continue;
        }
        res.append(caName);
    }
    Index::getInstance().getFriend()->m_pOnlineUser->updateUserList(res);
}

void ResHandler::addFriend()
{
    int ret;
    memcpy(&ret,pdu->caData,sizeof(int));
    qDebug()<<"addFriend ret"<<ret;
    if(ret == 2){
        QMessageBox::information(&Index::getInstance(),"提示","该用户已是好友");
    }
    else if(ret == -1){
        QMessageBox::information(&Index::getInstance(),"提示","查询失败");
    }
    else if(ret == 0){
        QMessageBox::information(&Index::getInstance(),"提示","该用户不在线");
    }
}

void ResHandler::addFriendResend()
{
    char curName[32] = {'\0'};
    memcpy(curName,pdu->caData,32);
    int ret = QMessageBox::question(&Index::getInstance(),"提示",QString("%1 请求添加您为好友，是否同意？").arg(curName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_AGREE_ADD_FRIEND_REQUEST;
    memcpy(respdu->caData,pdu->caData,64);
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::agreeAddFriend()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if (ret) {
        Index::getInstance().getFriend()->flushFriend();
    } else {
        QMessageBox::warning(&Client::getInstance(), "提示", "添加好友失败");
    }
}

void ResHandler::flushFriend()
{
    int iSize = pdu->uiMsgLen/32;
    QStringList res;
    char caName[32] = {'\0'};
    for(int i = 0;i<iSize;i++){
        memcpy(caName,pdu->caMsg+i*32,32);
        res.append(caName);
    }
    Index::getInstance().getFriend()->updateFriendrList(res);
}

void ResHandler::delFriend()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    qDebug()<<"delFriend ret"<<ret;
    if(ret){
        QMessageBox::information(&Index::getInstance(),"提示","删除成功");
        flushFriend();
    }
    else {
        QMessageBox::information(&Index::getInstance(),"提示","删除失败");
    }
}

void ResHandler::chat()
{
    Chat* c = Index::getInstance().getFriend()->m_pChat;
    if(c->isHidden()){
        c->show();
    }
    char caChatName[32] = {'\0'};
    memcpy(caChatName,pdu->caData,32);
    c->m_strChatName = caChatName;
    c->addMsg(QString("%1 : %2").arg(caChatName).arg(pdu->caMsg));

}

void ResHandler::mkDir()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::warning(&Index::getInstance(),"提示","创建文件夹失败");
    }
}

void ResHandler::flushFile()
{
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    qDebug()<<"iCount"<<iCount;
    QList<FileInfo*> pFileList;
    for(int i = 0;i<iCount;i++){
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo,pdu->caMsg+i*sizeof(FileInfo),sizeof(FileInfo));
        pFileList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFileList(pFileList);
}

void ResHandler::moveFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::warning(&Index::getInstance(),"提示","移动文件夹失败");
    }
}

void ResHandler::uploadFile()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->uploadFile();
    }else{
        QMessageBox::warning(&Index::getInstance(),"提示","服务器初始化失败");
    }
}

void ResHandler::uploadFileData()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::warning(&Index::getInstance(),"提示","上传文件失败");
    }
}

void ResHandler::shareFile()
{
    QMessageBox::information(Index::getInstance().getFile(),"分享文件","文件已分享");
}

void ResHandler::shareFileRequest()
{
    QString strSharePath = QString(pdu->caMsg);
    int index = strSharePath.lastIndexOf('/');
    QString strFileName = strSharePath.right(strSharePath.size()-index-1);
    // 安全地将caData转换为QString（限制为32个字符）
    char senderName[33] = {0};
    memcpy(senderName, pdu->caData, 32);
    QString strMsg = QString("%1 分享文件: %2\n是否接收？").arg(QString(senderName)).arg(strFileName);
    int ret = QMessageBox::question(&Index::getInstance(),"分享文件",strMsg);
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* respdu = mkPDU(pdu->uiMsgLen);
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST;
    QString strName = Client::getInstance().m_strLoginName;
    memcpy(respdu->caData,strName.toStdString().c_str(),32);
    memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::shareFileResult()
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::warning(&Index::getInstance(),"提示","分享文件失败");
    }
}

void ResHandler::deleteDir()
{
    // 从PDU中获取服务器返回的结果信息
    QString strMsg = QString(pdu->caMsg);
    if(strMsg == "DEL_DIR_SUCCESS"){
        QMessageBox::information(NULL, "删除文件夹", "文件夹删除成功");
        // 刷新文件列表以更新界面
        Index::getInstance().getFile()->flushFile();
    } else {
        QMessageBox::warning(NULL, "删除文件夹", QString("文件夹删除失败: %1").arg(strMsg));
    }
}

void ResHandler::deleteFile()
{
    // 从PDU中获取服务器返回的结果信息
    QString strMsg = QString(pdu->caMsg);
    if(strMsg == "DEL_FILE_SUCCESS"){
        QMessageBox::information(NULL, "删除文件", "文件删除成功");
        // 刷新文件列表以更新界面
        Index::getInstance().getFile()->flushFile();
    } else {
        QMessageBox::warning(NULL, "删除文件", QString("文件删除失败: %1").arg(strMsg));
    }
}

void ResHandler::rename()
{
    // 从PDU中获取服务器返回的结果信息
    QString strMsg = QString(pdu->caMsg);
    if(strMsg == "RENAME_SUCCESS"){
        QMessageBox::information(NULL, "重命名", "重命名成功");
        // 刷新文件列表以更新界面
        Index::getInstance().getFile()->flushFile();
    } else {
        QMessageBox::warning(NULL, "重命名", QString("重命名失败: %1").arg(strMsg));
    }
}

void ResHandler::downloadFileRespond()
{
    if(pdu == NULL) return;
    
    // 从PDU中获取服务器返回的结果信息
    char caResult[64] = {0};
    strncpy(caResult, pdu->caData, 63);
    
    if(strcmp(caResult, "success") == 0){
        // 成功响应，创建下载器实例
        // 获取文件名（从caData中获取）
        char caFileName[32] = {0};
        strncpy(caFileName, pdu->caData, 31);
        QString strFileName = caFileName;
        
        // 使用文件的保存路径
        QString strFilePath = Index::getInstance().getFile()->m_strUploadFilePath;
        
        // 创建下载器实例
        Downloader* pDownloader = new Downloader(strFilePath, strFileName);
        
        // 处理响应数据
        pDownloader->processDownloadFileRespond(pdu);
    } else {
        // 失败响应，显示错误信息
        QMessageBox::warning(NULL, "下载文件", QString("下载失败: %1").arg(pdu->caMsg));
    }
}

void ResHandler::downloadFileDataRespond()
{
    if(pdu == NULL) return;
    
    // 获取下载器实例并处理数据
    Downloader* pDownloader = Downloader::getInstance();
    if(pDownloader != NULL){
        pDownloader->processDownloadFileDataRespond(pdu);
    } else {
        // 如果没有找到下载器实例，可能是因为多个下载同时进行
        // 这里可以添加日志记录或其他处理逻辑
        qDebug() << "Warning: No active downloader instance found";
    }
}



