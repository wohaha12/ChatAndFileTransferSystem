#include "FileWidget.h"
#include "ui_FileWidget.h"
#include "StyleManager.h"
#include "AnimationManager.h"
#include <QDebug>
#include <QTreeWidgetItem>

FileWidget::FileWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::FileWidget)
    , m_currentPath("/")
{
    ui->setupUi(this);
    setupUi();
    applyStyles();
    connectSignals();

    addSampleFiles();
}

FileWidget::~FileWidget()
{
    delete ui;
}

void FileWidget::setupUi()
{
    ui->fileTreeWidget->setHeaderLabels(QStringList() << "名称" << "大小" << "修改时间");
    ui->fileTreeWidget->setColumnWidth(0, 400);
    ui->fileTreeWidget->setColumnWidth(1, 120);
    ui->fileTreeWidget->setColumnWidth(2, 180);
}

void FileWidget::applyStyles()
{
    StyleManager* styleManager = StyleManager::instance();

    QString headerStyle = QString(
        "QWidget#headerWidget {"
        "   background-color: %1;"
        "   border-bottom: 1px solid %2;"
        "}"
        "QLabel#homeLabel {"
        "   color: %3;"
        "   padding: 5px;"
        "   border-radius: 5px;"
        "}"
        "QLabel#homeLabel:hover {"
        "   background-color: %4;"
        "}"
        "QLabel#separatorLabel {"
        "   color: %5;"
        "   padding: 5px;"
        "}"
        "QLabel#currentPathLabel {"
        "   color: %6;"
        "   padding: 5px;"
        "   font-weight: bold;"
        "}"
    ).arg(styleManager->cardBackgroundColor().name(),
           styleManager->borderColor().name(),
           styleManager->primaryColor().name(),
           styleManager->hoverColor().name(),
           styleManager->secondaryTextColor().name(),
           styleManager->textColor().name());
    ui->headerWidget->setStyleSheet(headerStyle);

    QString toolbarButtonStyle = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: %2;"
        "   border: 1px solid %3;"
        "   border-radius: 6px;"
        "   padding: 5px 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: %4;"
        "   border-color: %5;"
        "}"
        "QPushButton:pressed {"
        "   background-color: %6;"
        "}"
        "QPushButton:disabled {"
        "   background-color: %7;"
        "   color: %8;"
        "   border-color: %7;"
        "}"
    ).arg(styleManager->cardBackgroundColor().name(),
           styleManager->textColor().name(),
           styleManager->borderColor().name(),
           styleManager->hoverColor().name(),
           styleManager->primaryColor().name(),
           styleManager->primaryColor().name(),
           styleManager->borderColor().name(),
           styleManager->secondaryTextColor().name());
    ui->uploadButton->setStyleSheet(toolbarButtonStyle);
    ui->downloadButton->setStyleSheet(toolbarButtonStyle);
    ui->newFolderButton->setStyleSheet(toolbarButtonStyle);
    ui->renameButton->setStyleSheet(toolbarButtonStyle);
    ui->deleteButton->setStyleSheet(toolbarButtonStyle);

    QString refreshButtonStyle = QString(
        "QPushButton {"
        "   background-color: transparent;"
        "   color: %1;"
        "   border: 1px solid %2;"
        "   border-radius: 18px;"
        "}"
        "QPushButton:hover {"
        "   background-color: %3;"
        "   border-color: %4;"
        "}"
    ).arg(styleManager->textColor().name(),
           styleManager->borderColor().name(),
           styleManager->hoverColor().name(),
           styleManager->primaryColor().name());
    ui->refreshButton->setStyleSheet(refreshButtonStyle);

    styleManager->applyLineEditStyle(ui->searchLineEdit);
    styleManager->applyTreeWidgetStyle(ui->fileTreeWidget);

    QString lineStyle = QString(
        "QFrame[frameShape=\"4\"], QFrame[frameShape=\"5\"] {"
        "   color: %1;"
        "   background-color: %1;"
        "}"
    ).arg(styleManager->borderColor().name());
    ui->headerLine->setStyleSheet(lineStyle);
}

void FileWidget::connectSignals()
{
    connect(ui->uploadButton, &QPushButton::clicked, this, &FileWidget::onUploadButtonClicked);
    connect(ui->downloadButton, &QPushButton::clicked, this, &FileWidget::onDownloadButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &FileWidget::onDeleteButtonClicked);
    connect(ui->renameButton, &QPushButton::clicked, this, &FileWidget::onRenameButtonClicked);
    connect(ui->newFolderButton, &QPushButton::clicked, this, &FileWidget::onNewFolderButtonClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &FileWidget::onRefreshButtonClicked);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &FileWidget::onSearchTextChanged);
    connect(ui->homeLabel, &QLabel::linkActivated, this, &FileWidget::onHomeClicked);
    connect(ui->fileTreeWidget, &QTreeWidget::itemDoubleClicked, this, &FileWidget::onTreeItemDoubleClicked);
    connect(ui->fileTreeWidget, &QTreeWidget::itemSelectionChanged, this, &FileWidget::onTreeSelectionChanged);

    updateButtonStates();
}

void FileWidget::addSampleFiles()
{
    QTreeWidgetItem* folder1 = new QTreeWidgetItem(ui->fileTreeWidget);
    folder1->setText(0, "📁 文档");
    folder1->setText(1, "-");
    folder1->setText(2, "2024-01-15 10:30");
    folder1->setData(0, Qt::UserRole, "folder");

    QTreeWidgetItem* folder2 = new QTreeWidgetItem(ui->fileTreeWidget);
    folder2->setText(0, "📁 图片");
    folder2->setText(1, "-");
    folder2->setText(2, "2024-01-14 15:20");
    folder2->setData(0, Qt::UserRole, "folder");

    QTreeWidgetItem* file1 = new QTreeWidgetItem(ui->fileTreeWidget);
    file1->setText(0, "📄 report.pdf");
    file1->setText(1, "2.5 MB");
    file1->setText(2, "2024-01-15 09:45");
    file1->setData(0, Qt::UserRole, "file");

    QTreeWidgetItem* file2 = new QTreeWidgetItem(ui->fileTreeWidget);
    file2->setText(0, "📄 notes.txt");
    file2->setText(1, "12 KB");
    file2->setText(2, "2024-01-14 16:30");
    file2->setData(0, Qt::UserRole, "file");

    QTreeWidgetItem* file3 = new QTreeWidgetItem(ui->fileTreeWidget);
    file3->setText(0, "🖼️ photo.jpg");
    file3->setText(1, "1.8 MB");
    file3->setText(2, "2024-01-13 11:20");
    file3->setData(0, Qt::UserRole, "file");
}

void FileWidget::refreshFileList()
{
    qDebug() << "Refresh file list requested";
}

void FileWidget::setCurrentPath(const QString& path)
{
    m_currentPath = path;
    ui->currentPathLabel->setText(path);
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
        newItem->setText(0, "📁 " + folderName);
        newItem->setText(1, "-");
        newItem->setText(2, QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm"));
        newItem->setData(0, Qt::UserRole, "folder");
        ui->fileTreeWidget->addTopLevelItem(newItem);
        
        AnimationManager::instance()->bounceIn(ui->fileTreeWidget, 600);
        
        qDebug() << "New folder created:" << folderName;
    }
}

void FileWidget::onRefreshButtonClicked()
{
    refreshFileList();
    qDebug() << "Refresh button clicked";
}

void FileWidget::onSearchTextChanged(const QString& text)
{
    qDebug() << "Search text changed:" << text;
}

void FileWidget::onHomeClicked()
{
    setCurrentPath("/");
    qDebug() << "Home clicked";
}

void FileWidget::adjustForWindowSize(int width, int height)
{
    int headerHeight = 120;
    
    if (height < 600) {
        headerHeight = 100;
    } else if (height < 800) {
        headerHeight = 110;
    }
    
    ui->headerWidget->setMaximumHeight(headerHeight);
    ui->headerWidget->setMinimumHeight(headerHeight);
    
    int searchWidth = 250;
    if (width < 1000) {
        searchWidth = 200;
    } else if (width < 1200) {
        searchWidth = 225;
    }
    
    ui->searchLineEdit->setMaximumWidth(searchWidth);
    ui->searchLineEdit->setMinimumWidth(searchWidth);
}

void FileWidget::onTreeItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (!item) {
        return;
    }

    QString itemName = item->text(0);
    QString itemType = item->data(0, Qt::UserRole).toString();

    if (itemType == "folder") {
        QString newPath = m_currentPath + "/" + itemName;
        setCurrentPath(newPath);
        qDebug() << "Double clicked on folder:" << itemName << "New path:" << newPath;
    } else {
        qDebug() << "Double clicked on file:" << itemName;
    }
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
