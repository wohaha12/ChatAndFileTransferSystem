#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

/**
 * @brief 存储引擎测试类
 * @details 测试存储引擎的文件读写、分片管理等功能
 */
class StorageEngineTest : public QObject {
  Q_OBJECT
public:
  explicit StorageEngineTest(QObject *parent = nullptr);
  ~StorageEngineTest() override;

private slots:
  void testInitialize();
  void testFileCreation();
  void testFileDeletion();
  void testChunkWrite();
  void testChunkRead();
  void testFileHashCalculation();
  void testDirectoryCreation();
  void testFileExistence();
  void testFileSize();
  void testMultipleChunks();
};

} // namespace Tests
