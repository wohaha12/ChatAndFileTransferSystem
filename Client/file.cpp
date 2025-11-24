#include "client.h"
#include "file.h"
#include "ui_file.h"
#include "uploader.h"

#include <QCryptographicHash>
#include <QFileDialog>
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strUserPath = QString("%1/%2").arg(Client::getInstance().m_strRootPath).arg(Client::getInstance().m_strLoginName);
    m_strCurPath = m_strUserPath;
    flushFile();
    m_pShareFile = new ShareFile;
}

File::~File()
{
    delete ui;
    delete m_pShareFile;
}

void File::flushFile()
{
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::updateFileList(QList<FileInfo*> pFileList)
{
    foreach(FileInfo* pFileInfo,m_pFileList){
        delete pFileInfo;
    }
    m_pFileList = pFileList;
    ui->listWidget->clear();
    foreach(FileInfo* pFileInfo,pFileList){
        QListWidgetItem* pItem = new QListWidgetItem;
        if(pFileInfo->iType == 0){
            pItem->setIcon(QIcon(QPixmap(":/images/folder.png")));
        }else if(pFileInfo->iType == 1){
            pItem->setIcon(QIcon(QPixmap(":/images/file.png")));
        }
        pItem->setText(pFileInfo->caName);
        ui->listWidget->addItem(pItem);
    }
}

void File::on_mkDir_PB_clicked()
{
    QString strNewDir = QInputDialog::getText(this,"提示","新建文件夹名");
    if(strNewDir.toStdString().size() == 0 || strNewDir.toStdString().size()>32){
        QMessageBox::information(this,"提示","文件夹名长度非法");
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_MSG_TYPE_MKDIR_REQUEST;
    memcpy(pdu->caData,strNewDir.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_flushFile_PB_clicked()
{
    flushFile();
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    foreach(FileInfo* pFileInfo,m_pFileList){
        if(item->text() == pFileInfo->caName && pFileInfo->iType != 0){
            return;
        }
    }
    m_strCurPath = m_strCurPath + "/" + item->text();
    flushFile();
}

void File::on_return_PB_clicked()
{
    // 检查当前路径是否已经是用户根目录
//    if (m_strCurPath != m_strUserPath) {
//        // 找到最后一个 '/' 的位置
//        int lastIndex = m_strCurPath.lastIndexOf('/');
//        if (lastIndex != -1) {
//            // 截取到最后一个 '/' 之前的路径，即父目录
//            flushFile();
//        }
//    } else {
//        QMessageBox::information(this, "提示", "已经是根目录");
//    }
    if (m_strCurPath != m_strUserPath) {
        int lastIndex = m_strCurPath.lastIndexOf('/');
        m_strCurPath.remove(lastIndex,m_strCurPath.size() - lastIndex);
        flushFile();
    } else {
        QMessageBox::information(this, "提示", "已经是根目录");
    }
}

void File::on_mvFile_PB_clicked()
{
    //判断按钮上的文字，分为移动文件和确认/取消
    if(ui->mvFile_PB->text() == "移动文件"){
        QListWidgetItem* pItem = ui->listWidget->currentItem();
        if(pItem == NULL){
            return;
        }
        QMessageBox::information(this,"移动文件","请选择要移动到的目录");
        //修改按钮上的文字
        ui->mvFile_PB->setText("确认/取消");
        //记录选择的文件名以及该文件的当前路径
        m_strMvName = pItem->text();
        m_strMvPath = m_strCurPath;
        return;
    }
    //点击确认/取消时的逻辑，修改按钮的文字
    ui->mvFile_PB->setText("移动文件");
    //如果用户选择了目录就移动到该目录下，如果没有选择目录（没有选择或者选择的是文件）就移动到当前路径
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    //获取要移动到的目录
    QString strTarPath;
    if(pItem == NULL){
        strTarPath = m_strCurPath;
    }else{
        strTarPath = m_strCurPath + "/" + pItem->text();
        foreach(FileInfo* pFileInfo,m_pFileList){
            if(pItem->text() == pFileInfo->caName && pFileInfo->iType != 0){
                strTarPath = m_strCurPath;
                break;
            }
        }
    }
    //询问用户是否确认移动
    int ret = QMessageBox::information(this,"移动文件",QString("是否确认移动到\n%1?").arg(strTarPath),"确认","取消");
    if(ret != 0){
        return;
    }
    //拼接文件的原完整路径和要移动到的完整路径
    strTarPath = strTarPath + "/" + m_strMvName;
    QString strSrcPath = m_strMvPath + "/" + m_strMvName;
    //两个路径放入caMsg，将两个路径的长度放入caData中，服务器根据路径的长度获取路径
    int iSrcPathLen = strSrcPath.toStdString().size();
    int iTarPathLen = strTarPath.toStdString().size();
    qDebug()<<"iSrcPathLen"<<iSrcPathLen<<"iTarPathLen"<<iTarPathLen;
    PDU* pdu = mkPDU(iSrcPathLen+iTarPathLen+1);
    pdu->uiType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    memcpy(pdu->caData,&iSrcPathLen,sizeof(int));
    memcpy(pdu->caData+sizeof(int),&iTarPathLen,sizeof(int));
    memcpy(pdu->caMsg,strSrcPath.toStdString().c_str(),iSrcPathLen);
    memcpy(pdu->caMsg+iSrcPathLen,strTarPath.toStdString().c_str(),iTarPathLen);
    Client::getInstance().sendMsg(pdu);
}

void File::on_uploadFile_PB_clicked()
{
    m_strUploadFilePath = QFileDialog::getOpenFileName(this,"选择文件","","所有文件(*.*)");
    qDebug()<<"selectedPath"<<m_strUploadFilePath;
    int index = m_strUploadFilePath.lastIndexOf("/");
    QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index -1);
    qDebug()<<"selectFileName"<<strFileName;
    QFile file(m_strUploadFilePath);
    qint64 iFileSize = file.size();
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    memcpy(pdu->caData,strFileName.toStdString().c_str(),32);
    memcpy(pdu->caData + 32,&iFileSize,sizeof(qint64));
    Client::getInstance().sendMsg(pdu);
}

void File::uploadFile()
{
   Uploader* uploader = new Uploader(m_strUploadFilePath);
   connect(uploader,&Uploader::errorMsgBox,this,&File::uploadError);
   connect(uploader,&Uploader::uploadPDU,&Client::getInstance(),&Client::sendMsg);
   uploader->start();
}

void File::uploadError(QString strError)
{
    QMessageBox::warning(this,"提示",strError);
}

void File::on_shareFile_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        return;
    }
    m_strShareFileName = pItem->text();
    m_pShareFile->updataFriend_LW();
    if(m_pShareFile->isHidden()){
        m_pShareFile->show();
    }
}

void File::on_delDir_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "提示", "请选择要删除的文件夹");
        return;
    }
    
    // 确认要删除的是文件夹
    bool isDir = false;
    foreach(FileInfo* pFileInfo, m_pFileList){
        if(pItem->text() == pFileInfo->caName && pFileInfo->iType == 0){
            isDir = true;
            break;
        }
    }
    
    if(!isDir){
        QMessageBox::warning(this, "提示", "请选择文件夹而不是文件");
        return;
    }
    
    // 弹窗确认是否删除
    int ret = QMessageBox::question(this, "删除文件夹", 
                                   QString("确定要删除文件夹 '%1' 吗？").arg(pItem->text()),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if(ret != QMessageBox::Yes){
        return;
    }
    
    // 构建请求PDU，发送给服务器
    QString strDirPath = QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    PDU* pdu = mkPDU(strDirPath.toStdString().size() + 1);
    pdu->uiType = ENUM_MSG_TYPE_DELETE_DIR_REQUEST;
    memcpy(pdu->caMsg, strDirPath.toStdString().c_str(), strDirPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_delFile_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "提示", "请选择要删除的文件");
        return;
    }
    
    // 确认要删除的是文件
    bool isFile = false;
    foreach(FileInfo* pFileInfo, m_pFileList){
        if(pItem->text() == pFileInfo->caName && pFileInfo->iType == 1){
            isFile = true;
            break;
        }
    }
    
    if(!isFile){
        QMessageBox::warning(this, "提示", "请选择文件而不是文件夹");
        return;
    }
    
    // 弹窗确认是否删除
    int ret = QMessageBox::question(this, "删除文件", 
                                   QString("确定要删除文件 '%1' 吗？").arg(pItem->text()),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if(ret != QMessageBox::Yes){
        return;
    }
    
    // 构建请求PDU，发送给服务器
    QString strFilePath = QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    PDU* pdu = mkPDU(strFilePath.toStdString().size() + 1);
    pdu->uiType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;  // 使用删除文件消息类型
    memcpy(pdu->caMsg, strFilePath.toStdString().c_str(), strFilePath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_rename_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "提示", "请选择要重命名的文件或文件夹");
        return;
    }
    
    // 弹出输入框让用户输入新名称
    QString strNewName = QInputDialog::getText(this, "重命名", 
                                            "请输入新名称", 
                                            QLineEdit::Normal, 
                                            pItem->text());
    
    if(strNewName.isEmpty() || strNewName.size() > 32){
        QMessageBox::warning(this, "提示", "名称不能为空或超过32个字符");
        return;
    }
    
    // 检查新名称是否与当前目录下的其他文件/文件夹重名
    foreach(QListWidgetItem* item, ui->listWidget->findItems(strNewName, Qt::MatchExactly)){
        if(item != pItem){ // 排除自己
            QMessageBox::warning(this, "提示", "该名称已存在，请选择其他名称");
            return;
        }
    }
    
    // 构建请求PDU
    QString strOldPath = QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    QString strNewPath = QString("%1/%2").arg(m_strCurPath).arg(strNewName);
    
    int iOldPathLen = strOldPath.toStdString().size();
    int iNewPathLen = strNewPath.toStdString().size();
    
    PDU* pdu = mkPDU(iOldPathLen + iNewPathLen + 1);
    pdu->uiType = ENUM_MSG_TYPE_RENAME_REQUEST;
    
    // 将路径长度放入caData
    memcpy(pdu->caData, &iOldPathLen, sizeof(int));
    memcpy(pdu->caData + sizeof(int), &iNewPathLen, sizeof(int));
    
    // 将旧路径和新路径放入caMsg
    memcpy(pdu->caMsg, strOldPath.toStdString().c_str(), iOldPathLen);
    memcpy(pdu->caMsg + iOldPathLen, strNewPath.toStdString().c_str(), iNewPathLen);
    
    // 发送给服务器
    Client::getInstance().sendMsg(pdu);
}

void File::on_downloadFile_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "提示", "请选择要下载的文件");
        return;
    }
    
    // 确认要下载的是文件
    bool isFile = false;
    foreach(FileInfo* pFileInfo, m_pFileList){
        if(pItem->text() == pFileInfo->caName && pFileInfo->iType == 1){
            isFile = true;
            break;
        }
    }
    
    if(!isFile){
        QMessageBox::warning(this, "提示", "只能下载文件，不能下载文件夹");
        return;
    }
    
    // 弹出文件保存对话框，让用户选择保存位置
    QString strSaveFilePath = QFileDialog::getSaveFileName(
        this, 
        "保存文件", 
        pItem->text(),
        "所有文件(*.*)"
    );
    
    if(strSaveFilePath.isEmpty()){
        // 用户取消了保存操作
        return;
    }
    
    // 构建请求PDU，发送给服务器
    QString strFilePath = QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    
    // 保存下载信息，供后续下载使用
    m_strUploadFilePath = strSaveFilePath; // 复用已有的成员变量来存储下载保存路径
    
    PDU* pdu = mkPDU(strFilePath.toStdString().size() + 1);
    pdu->uiType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    memcpy(pdu->caMsg, strFilePath.toStdString().c_str(), strFilePath.toStdString().size());
    
    // 将保存路径也发送给服务器，以便在响应中返回（可选，主要用于日志）
    // 实际文件保存由客户端在接收数据时处理

    Client::getInstance().sendMsg(pdu);
}
