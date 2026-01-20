#ifndef CLIENT_UI_FILEWIDGET_H
#define CLIENT_UI_FILEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

namespace Ui {
class FileWidget;
}

class FileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileWidget(QWidget* parent = nullptr);
    ~FileWidget();

    void refreshFileList();
    void setCurrentPath(const QString& path);

private slots:
    void onUploadButtonClicked();
    void onDownloadButtonClicked();
    void onDeleteButtonClicked();
    void onRenameButtonClicked();
    void onNewFolderButtonClicked();
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onTreeSelectionChanged();

private:
    void updateButtonStates();

    Ui::FileWidget* ui;
    QString m_currentPath;
};

#endif // CLIENT_UI_FILEWIDGET_H
