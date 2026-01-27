#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

/**
 * @brief 数据库测试类
 * @details 测试数据库连接池、用户DAO、文件DAO等功能
 */
class DatabaseTest : public QObject {
    Q_OBJECT
public:
    explicit DatabaseTest(QObject* parent = nullptr);
    ~DatabaseTest() override;

private slots:
    void testDatabaseConnection();
    void testUserDAOInsert();
    void testUserDAOQuery();
    void testUserDAOUpdate();
    void testFileDAOInsert();
    void testFileDAOQuery();
    void testFileDAODelete();
    void testTransactionCommit();
    void testTransactionRollback();
};

} // namespace Tests
