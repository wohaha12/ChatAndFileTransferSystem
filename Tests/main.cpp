#include "test_database.h"
#include "test_load_balancer.h"
#include "test_memory_pool.h"
#include "test_protocol.h"
#include "test_storage_engine.h"
#include "test_token_validator.h"
#include <QCoreApplication>
#include <QDebug>
#include <QVector>
#include <QtTest/QtTest>

/**
 * @brief 主测试入口
 * @details 运行所有测试用例并生成测试报告
 */
int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  int result = 0;

  QVector<QObject *> testObjects;

  testObjects.append(new Tests::MemoryPoolTest());
  testObjects.append(new Tests::LoadBalancerTest());
  testObjects.append(new Tests::ProtocolTest());
  testObjects.append(new Tests::DatabaseTest());
  testObjects.append(new Tests::TokenValidatorTest());
  testObjects.append(new Tests::StorageEngineTest());

  qDebug() << "========================================";
  qDebug() << "开始执行测试...";
  qDebug() << "========================================";

  for (QObject *testObj : testObjects) {
    // qExec returns the number of failures
    result |= QTest::qExec(testObj, argc, argv);
  }

  qDeleteAll(testObjects);

  qDebug() << "========================================";
  qDebug() << "测试执行完成, 最终状态码:" << result;
  qDebug() << "========================================";

  return result;
}
