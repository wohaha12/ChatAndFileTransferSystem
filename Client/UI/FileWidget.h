#ifndef CLIENT_UI_FILEWIDGET_H
#define CLIENT_UI_FILEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

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
    void adjustForWindowSize(int width, int height);

private slots:
    void onUploadButtonClicked();
    void onDownloadButtonClicked();
    void onDeleteButtonClicked();
    void onRenameButtonClicked();
    void onNewFolderButtonClicked();
    void onRefreshButtonClicked();
    void onSearchTextChanged(const QString& text);
    void onHomeClicked();
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onTreeSelectionChanged();

private:
    void setupUi();
    void applyStyles();
    void connectSignals();
    void addSampleFiles();
    void updateButtonStates();

    Ui::FileWidget* ui;
    QString m_currentPath;
};

#endif // CLIENT_UI_FILEWIDGET_H
