#include "FileModel.h"

FileModel::FileModel(QObject* parent)
    : QObject(parent)
{
}

FileModel::~FileModel()
{
}

void FileModel::addFile(const FileInfo& file)
{
    m_files.append(file);
    emit fileAdded(file);
}

void FileModel::updateFile(const FileInfo& file)
{
    for (int i = 0; i < m_files.size(); ++i) {
        if (m_files[i].fileId == file.fileId) {
            m_files[i] = file;
            emit fileUpdated(file);
            return;
        }
    }
}

void FileModel::removeFile(uint64_t fileId)
{
    for (int i = 0; i < m_files.size(); ++i) {
        if (m_files[i].fileId == fileId) {
            m_files.removeAt(i);
            emit fileRemoved(fileId);
            return;
        }
    }
}

void FileModel::clear()
{
    m_files.clear();
    emit fileCleared();
}

FileInfo FileModel::getFile(uint64_t fileId) const
{
    for (const FileInfo& file : m_files) {
        if (file.fileId == fileId) {
            return file;
        }
    }
    return FileInfo();
}

QList<FileInfo> FileModel::getFilesByParent(uint64_t parentId) const
{
    QList<FileInfo> result;
    for (const FileInfo& file : m_files) {
        if (file.parentId == parentId && !file.isDeleted) {
            result.append(file);
        }
    }
    return result;
}

QList<FileInfo> FileModel::getAllFiles() const
{
    return m_files;
}

int FileModel::getFileCount() const
{
    return m_files.size();
}
