#include "FileWidget.h"
#include "ui_FileWidget.h"
#include <QDebug>

FileWidget::FileWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::FileWidget)
    , m_currentPath("/")
{
    ui->setupUi(this);

    connect(ui->uploadButton, &QPushButton::clicked, this, &FileWidget::onUploadButtonClicked);
    connect(ui->downloadButton, &QPushButton::clicked, this, &FileWidget::onDownloadButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &FileWidget::onDeleteButtonClicked);
    connect(ui->renameButton, &QPushButton::clicked, this, &FileWidget::onRenameButtonClicked);
    connect(ui->newFolderButton, &QPushButton::clicked, this, &FileWidget::onNewFolderButtonClicked);
    connect(ui->fileTreeWidget, &QTreeWidget::itemDoubleClicked, this, &FileWidget::onTreeItemDoubleClicked);
    connect(ui->fileTreeWidget, &QTreeWidget::itemSelectionChanged, this, &FileWidget::onTreeSelectionChanged);

    updateButtonStates();
}

FileWidget::~FileWidget()
{
    delete ui;
}

void FileWidget::refreshFileList()
{
    qDebug() << "Refresh file list requested";
}

void FileWidget::setCurrentPath(const QString& path)
{
    m_currentPath = path;
    qDebug() << "Current path set to:" << path;
}

void FileWidget::onUploadButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件", "", "所有文件(*.*)");

    if (!filePath.isEmpty()) {
        qDebug() << "Selected file:" << filePath;
    }
}

void FileWidget::onDownloadButtonClicked()
{
    QTreeWidgetItem* item = ui->fileTreeWidget->currentItem();
    if (!item) {
        return;
    }

    QString fileName = item->text(0);
    QString savePath = QFileDialog::getSaveFileName(this, "保存文件", fileName, "所有文件(*.*)");

    if (!savePath.isEmpty()) {
        qDebug() << "Download file to:" << savePath;
    }
}

void FileWidget::onDeleteButtonClicked()
{
    QTreeWidgetItem* item = ui->fileTreeWidget->currentItem();
    if (!item) {
        return;
    }

    int ret = QMessageBox::question(this, "删除确认",
                                      QString("确定要删除 '%1' 吗？").arg(item->text(0)),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        delete item;
        qDebug() << "File deleted:" << item->text(0);
    }
}

void FileWidget::onRenameButtonClicked()
{
    QTreeWidgetItem* item = ui->fileTreeWidget->currentItem();
    if (!item) {
        return;
    }

    bool ok;
    QString newName = QInputDialog::getText(this, "重命名",
                                           "请输入新名称",
                                           QLineEdit::Normal,
                                           item->text(0),
                                           &ok);

    if (ok && !newName.isEmpty()) {
        item->setText(0, newName);
        qDebug() << "File renamed to:" << newName;
    }
}

void FileWidget::onNewFolderButtonClicked()
{
    bool ok;
    QString folderName = QInputDialog::getText(this, "新建文件夹",
                                           "请输入文件夹名称",
                                           QLineEdit::Normal,
                                           "新建文件夹",
                                           &ok);

    if (ok && !folderName.isEmpty()) {
        QTreeWidgetItem* newItem = new QTreeWidgetItem(ui->fileTreeWidget);
        newItem->setText(0, folderName);
        newItem->setIcon(0, QIcon(":/icons/folder.png"));
        ui->fileTreeWidget->addTopLevelItem(newItem);
        qDebug() << "New folder created:" << folderName;
    }
}

void FileWidget::onTreeItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (!item) {
        return;
    }

    QString itemName = item->text(0);
    QString newPath = m_currentPath + "/" + itemName;
    setCurrentPath(newPath);

    qDebug() << "Double clicked on:" << itemName << "New path:" << newPath;
}

void FileWidget::onTreeSelectionChanged()
{
    updateButtonStates();
}

void FileWidget::updateButtonStates()
{
    QTreeWidgetItem* item = ui->fileTreeWidget->currentItem();
    bool hasSelection = (item != nullptr);

    ui->downloadButton->setEnabled(hasSelection);
    ui->deleteButton->setEnabled(hasSelection);
    ui->renameButton->setEnabled(hasSelection);
}
