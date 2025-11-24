#include "msghandler.h"
#include "operatedb.h"
#include "mytcpsocket.h"
#include "mytcpserver.h"
#include "server.h"

#include <QDir>
#include <QCoreApplication>
#include <sys/stat.h>
#include <direct.h>
#include <stdio.h>
MsgHandler::MsgHandler()
{

}

PDU *MsgHandler::regist()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    char caPwd[32] = {'\0'};
    memcpy(caPwd,pdu->caData+32,32);
    bool ret = OperateDB::getInstance().handleRegist(caName,caPwd);
    qDebug()<<"ret"<<ret;
    if(ret){
        QDir dir;
        bool mkRet = dir.mkdir(QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(caName));
        qDebug()<<"mkdir ret"<<mkRet;
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_REGIST_RESPOND;
    return respdu;
}

PDU *MsgHandler::login(QString& strLogiName)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    memcpy(caPwd,pdu->caData+32,32);
    bool ret = OperateDB::getInstance().handleLogin(caName,caPwd);
    qDebug()<<"ret"<<ret;
    if(ret){
        strLogiName = caName;
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    return respdu;
}

PDU *MsgHandler::findUser()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    int ret = OperateDB::getInstance().handleFindUser(caName);
    qDebug() << "查找用户" << caName << "结果：" << ret;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,caName,32);
    memcpy(respdu->caData+32,&ret,sizeof(int));
    respdu->uiType = ENUM_MSG_TYPE_FIND_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    QStringList res = OperateDB::getInstance().handleOnlineUser();
    qDebug() << "res"<<res.size();
    PDU* respdu = mkPDU(res.size()*32);
    for(int i = 0;i<res.size();i++){
        memcpy(respdu->caMsg+i*32,res[i].toStdString().c_str(),32);
    }
    respdu->uiType = ENUM_MSG_TYPE_ONLINE_USER_RESPOND;
    return respdu;
}

PDU* MsgHandler::addFriend()
{
    char curName[32] = {'\0'};
    memcpy(curName,pdu->caData,32);
    char tarName[32] = {'\0'};
    memcpy(tarName,pdu->caData+32,32);
    int ret = OperateDB::getInstance().handleAddFriend(curName,tarName);
    qDebug()<<"addFriend ret"<<ret;
    if(ret == 1){
        MyTcpServer::getInstance().resend(tarName,pdu);
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::agreeAddFriend()
{
    char curName[32] = {'\0'};
    memcpy(curName,pdu->caData,32);
    char tarName[32] = {'\0'};
    memcpy(tarName,pdu->caData+32,32);
    bool ret = OperateDB::getInstance().handleAgreeAddFriend(curName,tarName);
    qDebug()<<"agreeAddFriend ret"<<ret;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_AGREE_ADD_FRIEND_RESPOND;
    MyTcpServer::getInstance().resend(curName,respdu);
    return respdu;
}

PDU *MsgHandler::flushFriend()
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    QStringList res = OperateDB::getInstance().handleFlushOnlineUser(caName);
    qDebug() << "res"<<res.size();
    PDU* respdu = mkPDU(res.size()*32);
    for(int i = 0;i<res.size();i++){
        memcpy(respdu->caMsg+i*32,res[i].toStdString().c_str(),32);
    }
    respdu->uiType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::delFriend()
{
    char curName[32] = {'\0'};
    memcpy(curName,pdu->caData,32);
    char tarName[32] = {'\0'};
    memcpy(tarName,pdu->caData+32,32);
    bool ret = OperateDB::getInstance().handleDelFriend(curName,tarName);
    qDebug()<<"delFriend ret"<<ret;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::chat()
{
    char tarName[32] = {'\0'};
    memcpy(tarName,pdu->caData+32,32);
    pdu->uiType = ENUM_MSG_TYPE_CHAT_RESPOND;
    MyTcpServer::getInstance().resend(tarName,pdu);
    return NULL;
}

PDU *MsgHandler::mkDir()
{
    QDir dir;
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    qDebug()<<"mkdir path"<<strPath;
    bool ret;
    if(dir.exists(strPath)){
        ret = false;
        qDebug()<<"文件夹名已存在";
    }
    ret = dir.mkdir(QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData));
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_MKDIR_RESPOND;
    return respdu;
}

PDU *MsgHandler::flushFile()
{
    qDebug()<<"flushFile path"<<pdu->caMsg;
    QDir dir(pdu->caMsg);
    QFileInfoList fileInfoList = dir.entryInfoList();
    QList<QFileInfo> validFiles;
    foreach(QFileInfo info, fileInfoList){
        QString fileName = info.fileName();
        if(fileName != "." && fileName != ".."){
            validFiles.append(info);
        }
    }
    int iFileCount = validFiles.size();
    PDU* respdu = mkPDU(iFileCount*sizeof(FileInfo));
    respdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
    for(int i = 0;i<iFileCount;i++){
        FileInfo* pFileInfo = (FileInfo*)respdu->caMsg+i;
        memcpy(pFileInfo->caName,validFiles[i].fileName().toStdString().c_str(),32);
        if(validFiles[i].isDir()){
            pFileInfo->iType = 0;
        }
        else{
            pFileInfo->iType = 1;
        }
        qDebug()<<"pFileInfo->caName"<<pFileInfo->caName;
    }
    return respdu;
}

PDU *MsgHandler::moveFile()
{
    int iSrcPathLen = 0;
    int iTarPathLen = 0;

    memcpy(&iSrcPathLen,pdu->caData,sizeof(int));
    memcpy(&iTarPathLen,pdu->caData+sizeof(int),sizeof(int));

    char* strSrcPath = new char[iSrcPathLen + 1];
    memset(strSrcPath, '\0', iSrcPathLen + 1);

    char* strTarPath = new char[iTarPathLen + 1];
    memset(strTarPath, '\0', iTarPathLen + 1);

    memcpy(strSrcPath,pdu->caMsg,iSrcPathLen);
    memcpy(strTarPath,pdu->caMsg+iSrcPathLen,iTarPathLen);

    qDebug()<<"strSrcPath"<< strSrcPath <<"strTarPath"<< strTarPath;

    bool ret = QFile::rename(strSrcPath,strTarPath);
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
    delete [] strSrcPath;
    delete [] strTarPath;
    strSrcPath = NULL;
    strTarPath = NULL;
    return respdu;
}

PDU *MsgHandler::uploadFile()
{
    m_iReceiveSzie = 0;
    char caFileName[32] = {'\0'};
    memcpy(caFileName,pdu->caData,32);
    memcpy(&m_iUploadFileSize,pdu->caData+32,sizeof(qint64));
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    bool ret = m_fUploadFile.open(QIODevice::WriteOnly);
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(ret));
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
    return respdu;
}

PDU *MsgHandler::uploadFileData()
{
    m_fUploadFile.write(pdu->caMsg,pdu->uiMsgLen);
    m_iReceiveSzie += pdu->uiMsgLen;
    if(m_iReceiveSzie < m_iUploadFileSize){
        return NULL;
    }
    m_fUploadFile.close();
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND;
    bool ret = m_iReceiveSzie == m_iUploadFileSize;
    memcpy(respdu->caData,&ret,sizeof (ret));
    return respdu;
}

PDU *MsgHandler::shareFile()
{
    qDebug() << "shareFile start";
    char strCurName[32] = {'\0'};
    int friend_num = 0;
    memcpy(strCurName,pdu->caData,32);
    memcpy(&friend_num,pdu->caData+32,sizeof(int));
    qDebug()<<"strCurName"<<strCurName;
    qDebug()<<"friend_num"<<friend_num;
    int size = friend_num*32;
    PDU* resendpdu = mkPDU(pdu->uiMsgLen - size);
    resendpdu->uiType = pdu->uiType;
    memcpy(resendpdu->caData,strCurName,32);
    memcpy(resendpdu->caMsg,pdu->caMsg+size,pdu->uiMsgLen-size);
    char caRecvName[32] = {'\0'};
    for(int i=0;i<friend_num;i++){
        memcpy(caRecvName,pdu->caMsg+i*32,32);
        qDebug()<<"caRecvName"<<caRecvName;
        MyTcpServer::getInstance().resend(caRecvName,resendpdu);
    }
    free(resendpdu);
    resendpdu = NULL;
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
    return respdu;
}

PDU *MsgHandler::shareFileAgree()
{
    qDebug() << "[DEBUG] shareFileAgree started";
    
    // 获取应用程序目录 (debug目录)
    QString appDir = QCoreApplication::applicationDirPath();
    qDebug() << "[DEBUG] Application directory:" << appDir;
    
    // 向上移动一级到Server目录
    QDir dir(appDir);
    dir.cdUp();
    QString serverDir = dir.absolutePath();
    qDebug() << "[DEBUG] Server directory:" << serverDir;
    
    // 获取根路径配置
    QString strRootPath = Server::getInstance().m_strRootPath;
    qDebug() << "[DEBUG] Configured root path:" << strRootPath;
    
    // 构建接收方目录的绝对路径
    QString strRecvPath;
    if (strRootPath.startsWith("./")) {
        QString relRootPath = strRootPath.mid(2); // 移除./前缀
        strRecvPath = QString("%1/%2/%3").arg(serverDir).arg(relRootPath).arg(pdu->caData);
    } else {
        strRecvPath = QString("%1/%2/%3").arg(serverDir).arg(strRootPath).arg(pdu->caData);
    }
    qDebug() << "[DEBUG] Receiver path:" << strRecvPath;
    
    // 构建分享文件的绝对路径
    QString strShareFilePath = pdu->caMsg;
    qDebug() << "[DEBUG] Original share file path from client:" << strShareFilePath;
    
    QString strAbsShareFilePath;
    
    // 处理分享文件路径
    if (strShareFilePath.startsWith("./")) {
        // 客户端发送的是相对路径，直接基于serverDir构建绝对路径
        QString relPath = strShareFilePath.mid(2); // 移除./前缀
        strAbsShareFilePath = QString("%1/%2").arg(serverDir).arg(relPath);
    } else if (strShareFilePath.startsWith("filesys/")) {
        // 客户端发送的是filesys开头的路径
        strAbsShareFilePath = QString("%1/%2").arg(serverDir).arg(strShareFilePath);
    } else {
        // 其他情况，直接拼接
        strAbsShareFilePath = QString("%1/%2/%3").arg(serverDir).arg(strRootPath).arg(strShareFilePath);
    }
    
    // 规范化路径分隔符
    strAbsShareFilePath = strAbsShareFilePath.replace('\\', '/');
    qDebug() << "[DEBUG] Final absolute share file path:" << strAbsShareFilePath;
    
    // 确保源文件存在
    QFileInfo srcFileInfo(strAbsShareFilePath);
    if(!srcFileInfo.exists()) {
        qDebug() << "[ERROR] Share file failed: Source file does not exist" << strAbsShareFilePath;
        
        // 尝试查找可能的文件（处理缺少扩展名的情况）
        QDir srcDir = srcFileInfo.dir();
        QString fileName = srcFileInfo.fileName();
        QFileInfoList fileList = srcDir.entryInfoList();
        qDebug() << "[DEBUG] Looking for files in directory:" << srcDir.absolutePath();
        bool foundSimilar = false;
        for (const QFileInfo& info : fileList) {
            if (info.isFile()) {
                QString candidateName = info.fileName();
                qDebug() << "[DEBUG] Found file:" << candidateName;
                if (candidateName.startsWith(fileName)) {
                    qDebug() << "[DEBUG] Found similar file:" << candidateName;
                    foundSimilar = true;
                    // 更新绝对路径
                    strAbsShareFilePath = info.absoluteFilePath();
                    break;
                }
            }
        }
        
        if (foundSimilar) {
            qDebug() << "[DEBUG] Using corrected file path:" << strAbsShareFilePath;
            // 更新srcFileInfo对象以指向修正后的文件路径
            srcFileInfo = QFileInfo(strAbsShareFilePath);
        } else {
            PDU* respdu = mkPDU();
            bool ret = false;
            memcpy(respdu->caData, &ret, sizeof(bool));
            respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND;
            return respdu;
        }
    }
    
    // 获取文件名和目标目录
    int index = strAbsShareFilePath.lastIndexOf('/');
    QString strFileName = strAbsShareFilePath.right(strAbsShareFilePath.size() - index - 1);
    QString strDestDir = strRecvPath;
    QString strDestFilePath = strDestDir + '/' + strFileName;
    
    // 确保目标目录存在
    QDir destDir(strDestDir);
    if(!destDir.exists()) {
        if(!destDir.mkpath(strDestDir)) {
            qDebug() << "分享文件失败：无法创建目标目录" << strDestDir;
            PDU* respdu = mkPDU();
            bool ret = false;
            memcpy(respdu->caData, &ret, sizeof(bool));
            respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND;
            return respdu;
        }
    }
    
    qDebug() << "源文件路径:" << strAbsShareFilePath
             << "目标文件路径:" << strDestFilePath;
    
    bool ret = true;
    if(srcFileInfo.isFile()){
        ret = QFile::copy(strAbsShareFilePath, strDestFilePath);
        qDebug() << "文件复制结果:" << ret;
    } else if(srcFileInfo.isDir()){
        ret = copyDir(strAbsShareFilePath, strDestDir + '/' + strFileName);
        qDebug() << "目录复制结果:" << ret;
    } else {
        ret = false;
        qDebug() << "分享文件失败：源路径既不是文件也不是目录";
    }
    
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND;
    return respdu;
}

bool MsgHandler::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);
    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();
    bool ret = true;
    QString srcTmp;
    QString destTmp;
    for(int i=0;i<fileInfoList.size();i++){
        QFileInfo fileInfo = fileInfoList[i];
        QString fileName = fileInfo.fileName();
        
        // 跳过.和..目录
        if(fileName == "." || fileName == ".."){
            continue;
        }
        
        srcTmp = strSrcDir + '/' + fileName;
        destTmp = strDestDir + '/' + fileName;
        
        if(fileInfo.isFile()){
            // 复制文件
            if(!QFile::copy(srcTmp, destTmp)){
                qDebug() << "[ERROR] Failed to copy file:" << srcTmp << "to" << destTmp;
                ret = false;
            } else {
                qDebug() << "[DEBUG] Successfully copied file:" << srcTmp << "to" << destTmp;
            }
        } else if(fileInfo.isDir()){
            // 递归复制目录
            if(!copyDir(srcTmp, destTmp)){
                qDebug() << "[ERROR] Failed to copy directory:" << srcTmp << "to" << destTmp;
                ret = false;
            } else {
                qDebug() << "[DEBUG] Successfully copied directory:" << srcTmp << "to" << destTmp;
            }
        }
    }
    
    return ret;
}

PDU* MsgHandler::deleteDir()
{
    QString strDirPath = QString(pdu->caMsg);
    QDir dir(strDirPath);
    QString strMsg = "DEL_DIR_SUCCESS";
    
    if(!dir.exists()){
        strMsg = "文件夹不存在";
    } else {
        // 删除文件夹及其所有内容
        if(!dir.removeRecursively()){
            strMsg = "删除失败";
        }
    }
    
    PDU* respdu = mkPDU(strMsg.toStdString().size() + 1);
    respdu->uiType = ENUM_MSG_TYPE_DELETE_DIR_RESPOND;
    memcpy(respdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
    return respdu;
}

PDU* MsgHandler::deleteFile()
{
    QString strFilePath = QString(pdu->caMsg);
    QFile file(strFilePath);
    QString strMsg = "DEL_FILE_SUCCESS";
    
    if(!file.exists()){
        strMsg = "文件不存在";
    } else {
        // 删除文件
        if(!file.remove()){
            strMsg = "删除失败";
        }
    }
    
    PDU* respdu = mkPDU(strMsg.toStdString().size() + 1);
    respdu->uiType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
    memcpy(respdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
    return respdu;
}

PDU* MsgHandler::rename()
{
    // 解析客户端传来的路径信息
    int iOldPathLen = 0;
    int iNewPathLen = 0;
    
    memcpy(&iOldPathLen, pdu->caData, sizeof(int));
    memcpy(&iNewPathLen, pdu->caData + sizeof(int), sizeof(int));
    
    char* strOldPath = new char[iOldPathLen + 1];
    memset(strOldPath, '\0', iOldPathLen + 1);
    
    char* strNewPath = new char[iNewPathLen + 1];
    memset(strNewPath, '\0', iNewPathLen + 1);
    
    memcpy(strOldPath, pdu->caMsg, iOldPathLen);
    memcpy(strNewPath, pdu->caMsg + iOldPathLen, iNewPathLen);
    
    qDebug() << "重命名：从" << strOldPath << "到" << strNewPath;
    
    QString strOldPathQ = QString(strOldPath);
    QString strNewPathQ = QString(strNewPath);
    
    QString strMsg = "RENAME_SUCCESS";
    bool ret = false;
    
    // 检查旧路径是否存在
    QFileInfo fileInfo(strOldPathQ);
    if(!fileInfo.exists()){
        strMsg = "原文件/文件夹不存在";
    } 
    // 检查新路径是否已存在
    else if(QFileInfo(strNewPathQ).exists()){
        strMsg = "新名称已存在";
    } 
    else {
        // 执行重命名操作
        if(fileInfo.isDir()){
            QDir dir;
            ret = dir.rename(strOldPathQ, strNewPathQ);
        } else {
            ret = QFile::rename(strOldPathQ, strNewPathQ);
        }
        
        if(!ret){
            strMsg = "重命名失败";
        }
    }
    
    // 清理内存
    delete[] strOldPath;
    delete[] strNewPath;
    strOldPath = NULL;
    strNewPath = NULL;
    
    // 构建响应PDU
    PDU* respdu = mkPDU(strMsg.toStdString().size() + 1);
    respdu->uiType = ENUM_MSG_TYPE_RENAME_RESPOND;
    memcpy(respdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
    
    return respdu;
}

PDU* MsgHandler::downloadFile()
{
    if(pdu == NULL){
        return NULL;
    }
    
    // 解析客户端发送的文件路径
    QString strFilePath = QString::fromUtf8(pdu->caMsg);
    
    // 移除路径开头的"./"前缀（如果存在）
    QString strCleanPath = strFilePath;
    if(strFilePath.startsWith("./filesys/")) {
        strCleanPath = strFilePath.mid(2); // 跳过开头的"./"
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
    
    // 调试输出：打印所有相关路径信息
    qDebug() << "[DEBUG] Client requested file path:" << strFilePath;
    qDebug() << "[DEBUG] Cleaned path:" << strCleanPath;
    qDebug() << "[DEBUG] Real file path:" << strRealFilePath;
    
    // 检查文件是否存在 - 使用绝对路径确保准确性
    QFileInfo fileInfo(strRealFilePath);
    QString absFilePath = fileInfo.absoluteFilePath();
    qDebug() << "[DEBUG] Absolute file path:" << absFilePath;
    
    if(!fileInfo.exists()){
        // 文件不存在
        PDU* respdu = mkPDU(128);
        respdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        strcpy(respdu->caData, "error");
        strcpy(respdu->caMsg, "File does not exist");
        return respdu;
    }
    
    // 检查是否是文件（非文件夹）
    if(!fileInfo.isFile()){
        // 不是文件
        PDU* respdu = mkPDU(128);
        respdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        strcpy(respdu->caData, "error");
        strcpy(respdu->caMsg, "Not a file");
        return respdu;
    }
    
    // 获取文件大小
    qint64 fileSize = fileInfo.size();
    
    // 构建成功响应，包含文件大小信息
    PDU* respdu = mkPDU(sizeof(qint64));
    respdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
    strcpy(respdu->caData, "success");
    memcpy(respdu->caMsg, &fileSize, sizeof(qint64));
    
    return respdu;
}

