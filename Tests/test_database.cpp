#include "test_database.h"
#include "../../MetaServer/Database/DatabaseManager.h"
#include "../../MetaServer/Database/UserDAO.h"
#include "../../MetaServer/Database/FileDAO.h"
#include <QtTest>
#include <QSqlDatabase>
#include <QSqlError>

namespace Tests {

/**
 * @brief 构造函数
 */
DatabaseTest::DatabaseTest(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief 析构函数
 */
DatabaseTest::~DatabaseTest()
{
}

/**
 * @brief 测试数据库连接
 * @details 验证数据库连接池能够正确连接数据库
 */
void DatabaseTest::testDatabaseConnection()
{
    MetaServer::Database::DatabaseManager dbManager;
    
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    QVERIFY(dbManager.isConnected());
    
    dbManager.disconnect();
    QVERIFY(!dbManager.isConnected());
}

/**
 * @brief 测试用户DAO插入
 * @details 验证能够正确插入用户记录
 */
void DatabaseTest::testUserDAOInsert()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    QVERIFY(userId > 0);
    
    dbManager.disconnect();
}

/**
 * @brief 测试用户DAO查询
 * @details 验证能够正确查询用户记录
 */
void DatabaseTest::testUserDAOQuery()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    
    auto user = userDAO.queryUserById(userId);
    QVERIFY(user.has_value());
    QCOMPARE(user->username, QString("testuser"));
    QCOMPARE(user->password_hash, QString("testhash"));
    QCOMPARE(user->nickname, QString("Test User"));
    
    dbManager.disconnect();
}

/**
 * @brief 测试用户DAO更新
 * @details 验证能够正确更新用户记录
 */
void DatabaseTest::testUserDAOUpdate()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    
    QVERIFY(userDAO.updateUserNickname(userId, "Updated User"));
    QVERIFY(userDAO.updateUserStatus(userId, 1));
    
    auto user = userDAO.queryUserById(userId);
    QVERIFY(user.has_value());
    QCOMPARE(user->nickname, QString("Updated User"));
    QCOMPARE(user->status, 1);
    
    dbManager.disconnect();
}

/**
 * @brief 测试文件DAO插入
 * @details 验证能够正确插入文件记录
 */
void DatabaseTest::testFileDAOInsert()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    MetaServer::Database::FileDAO fileDAO(&dbManager);
    
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    
    uint64_t fileId = fileDAO.insertFileRecord(userId, "testfile.txt", "/test/testfile.txt", 1024, "d41d8cd98f00b204e9800998ecf8427e", 0);
    QVERIFY(fileId > 0);
    
    dbManager.disconnect();
}

/**
 * @brief 测试文件DAO查询
 * @details 验证能够正确查询文件记录
 */
void DatabaseTest::testFileDAOQuery()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    MetaServer::Database::FileDAO fileDAO(&dbManager);
    
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    uint64_t fileId = fileDAO.insertFileRecord(userId, "testfile.txt", "/test/testfile.txt", 1024, "d41d8cd98f00b204e9800998ecf8427e", 0);
    
    auto file = fileDAO.queryFileById(fileId);
    QVERIFY(file.has_value());
    QCOMPARE(file->file_name, QString("testfile.txt"));
    QCOMPARE(file->file_size, static_cast<uint64_t>(1024));
    QCOMPARE(file->file_hash, QString("d41d8cd98f00b204e9800998ecf8427e"));
    
    dbManager.disconnect();
}

/**
 * @brief 测试文件DAO删除
 * @details 验证能够正确删除文件记录
 */
void DatabaseTest::testFileDAODelete()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    MetaServer::Database::FileDAO fileDAO(&dbManager);
    
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    uint64_t fileId = fileDAO.insertFileRecord(userId, "testfile.txt", "/test/testfile.txt", 1024, "d41d8cd98f00b204e9800998ecf8427e", 0);
    
    QVERIFY(fileDAO.deleteFileRecord(userId, fileId));
    
    auto file = fileDAO.queryFileById(fileId);
    QVERIFY(!file.has_value());
    
    dbManager.disconnect();
}

/**
 * @brief 测试事务提交
 * @details 验证事务能够正确提交
 */
void DatabaseTest::testTransactionCommit()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    QVERIFY(dbManager.beginTransaction());
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    
    QVERIFY(dbManager.commitTransaction());
    
    auto user = userDAO.queryUserById(userId);
    QVERIFY(user.has_value());
    
    dbManager.disconnect();
}

/**
 * @brief 测试事务回滚
 * @details 验证事务能够正确回滚
 */
void DatabaseTest::testTransactionRollback()
{
    MetaServer::Database::DatabaseManager dbManager;
    QVERIFY(dbManager.connect("localhost", 3306, "chat_file_transfer", "root", ""));
    
    QVERIFY(dbManager.beginTransaction());
    
    MetaServer::Database::UserDAO userDAO(&dbManager);
    uint64_t userId = userDAO.insertUser("testuser", "testhash", "testsalt", "Test User", "");
    
    QVERIFY(dbManager.rollbackTransaction());
    
    auto user = userDAO.queryUserById(userId);
    QVERIFY(!user.has_value());
    
    dbManager.disconnect();
}

} // namespace Tests
