#include <QtTest/QtTest>
#include "test_memory_pool.h"
#include "test_load_balancer.h"
#include "test_protocol.h"
#include "test_database.h"
#include "test_token_validator.h"
#include "test_storage_engine.h"
#include <QCoreApplication>

/**
 * @brief 主测试入口
 * @details 运行所有测试用例并生成测试报告
 */
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    
    int result = 0;
    
    QVector<QObject*> testObjects;
    
    testObjects.append(new Tests::MemoryPoolTest());
    testObjects.append(new Tests::LoadBalancerTest());
    testObjects.append(new Tests::ProtocolTest());
    testObjects.append(new Tests::DatabaseTest());
    testObjects.append(new Tests::TokenValidatorTest());
    testObjects.append(new Tests::StorageEngineTest());
    
    for (QObject* testObj : testObjects) {
        QTest::qExec(testObj, QStringList(), QStringList());
        if (QTest::numFailed() > 0) {
            result = 1;
        }
    }
    
    qDeleteAll(testObjects);
    
    qDebug() << "========================================";
    qDebug() << "测试执行完成";
    qDebug() << "========================================";
    qDebug() << "总测试数:" << QTest::numExecuted();
    qDebug() << "失败测试数:" << QTest::numFailed();
    qDebug() << "跳过测试数:" << QTest::numSkipped();
    qDebug() << "========================================";
    
    return result;
}
