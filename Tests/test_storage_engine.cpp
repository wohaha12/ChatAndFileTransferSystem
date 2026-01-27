#include "test_storage_engine.h"
#include "../../StorageServer/Core/StorageEngine.h"
#include <QtTest>
#include <QFile>
#include <QDir>
#include <QTemporaryDir>

namespace Tests {

/**
 * @brief 构造函数
 */
StorageEngineTest::StorageEngineTest(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief 析构函数
 */
StorageEngineTest::~StorageEngineTest()
{
}

/**
 * @brief 测试文件创建
 * @details 验证能够正确创建文件
 */
void StorageEngineTest::testFileCreation()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QVERIFY(QFile::exists(filePath));
}

/**
 * @brief 测试文件删除
 * @details 验证能够正确删除文件
 */
void StorageEngineTest::testFileDeletion()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QVERIFY(engine.deleteFile(filePath));
    QVERIFY(!QFile::exists(filePath));
}

/**
 * @brief 测试分片写入
 * @details 验证能够正确写入文件分片
 */
void StorageEngineTest::testChunkWrite()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QByteArray chunkData("Hello, World!");
    QVERIFY(engine.writeChunk(filePath, 0, chunkData));
    
    QFile file(filePath);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray data = file.readAll();
    QCOMPARE(data, chunkData);
}

/**
 * @brief 测试分片读取
 * @details 验证能够正确读取文件分片
 */
void StorageEngineTest::testChunkRead()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QByteArray chunkData("Hello, World!");
    QVERIFY(engine.writeChunk(filePath, 0, chunkData));
    
    QByteArray readData = engine.readChunk(filePath, 0, chunkData.size());
    QCOMPARE(readData, chunkData);
}

/**
 * @brief 测试文件哈希计算
 * @details 验证能够正确计算文件哈希
 */
void StorageEngineTest::testFileHashCalculation()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QByteArray chunkData("Hello, World!");
    QVERIFY(engine.writeChunk(filePath, 0, chunkData));
    
    QString fileHash = engine.calculateFileHash(filePath);
    QVERIFY(!fileHash.isEmpty());
    QCOMPARE(fileHash.length(), 64);
}

/**
 * @brief 测试目录创建
 * @details 验证能够正确创建目录
 */
void StorageEngineTest::testDirectoryCreation()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString dirPath = tempDir.filePath("testdir");
    QVERIFY(QDir().mkpath(dirPath));
    
    QVERIFY(QDir(dirPath).exists());
}

/**
 * @brief 测试文件存在性
 * @details 验证能够正确检查文件是否存在
 */
void StorageEngineTest::testFileExistence()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QVERIFY(engine.fileExists(filePath));
    
    QString nonExistentPath = tempDir.filePath("nonexistent.txt");
    QVERIFY(!engine.fileExists(nonExistentPath));
}

/**
 * @brief 测试文件大小
 * @details 验证能够正确获取文件大小
 */
void StorageEngineTest::testFileSize()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QByteArray chunkData("Hello, World!");
    QVERIFY(engine.writeChunk(filePath, 0, chunkData));
    
    uint64_t fileSize = engine.getFileSize(filePath);
    QCOMPARE(fileSize, static_cast<uint64_t>(chunkData.size()));
}

/**
 * @brief 测试多个分片
 * @details 验证能够正确处理多个分片
 */
void StorageEngineTest::testMultipleChunks()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    StorageServer::Core::StorageEngine engine;
    QVERIFY(engine.initialize(tempDir.path()));
    
    QString filePath = tempDir.filePath("testfile.txt");
    QVERIFY(engine.createFile(filePath, 1024));
    
    QByteArray chunk1("Hello, ");
    QByteArray chunk2("World!");
    
    QVERIFY(engine.writeChunk(filePath, 0, chunk1));
    QVERIFY(engine.writeChunk(filePath, 1, chunk2));
    
    QByteArray readChunk1 = engine.readChunk(filePath, 0, chunk1.size());
    QByteArray readChunk2 = engine.readChunk(filePath, 1, chunk2.size());
    
    QCOMPARE(readChunk1, chunk1);
    QCOMPARE(readChunk2, chunk2);
}

} // namespace Tests
