#ifndef CLIENT_MODELS_FILEMODEL_H
#define CLIENT_MODELS_FILEMODEL_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>

struct FileInfo {
    uint64_t fileId;
    uint64_t userId;
    QString fileName;
    QString filePath;
    uint64_t fileSize;
    QString fileHash;
    int fileType;
    uint64_t parentId;
    bool isDeleted;
    QDateTime createTime;
    QDateTime updateTime;

    FileInfo()
        : fileId(0)
        , userId(0)
        , fileSize(0)
        , fileType(0)
        , parentId(0)
        , isDeleted(false)
    {
    }
};

class FileModel : public QObject
{
    Q_OBJECT

public:
    explicit FileModel(QObject* parent = nullptr);
    ~FileModel();

    void addFile(const FileInfo& file);
    void updateFile(const FileInfo& file);
    void removeFile(uint64_t fileId);
    void clear();

    FileInfo getFile(uint64_t fileId) const;
    QList<FileInfo> getFilesByParent(uint64_t parentId) const;
    QList<FileInfo> getAllFiles() const;

    int getFileCount() const;

signals:
    void fileAdded(const FileInfo& file);
    void fileUpdated(const FileInfo& file);
    void fileRemoved(uint64_t fileId);
    void fileCleared();

private:
    QList<FileInfo> m_files;
};

#endif // CLIENT_MODELS_FILEMODEL_H
