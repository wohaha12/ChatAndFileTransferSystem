#include "test_storage_engine.h"
#include "../../StorageServer/Core/StorageEngine.h"
#include <QDir>
#include <QtTest>

namespace Tests {

StorageEngineTest::StorageEngineTest(QObject *parent) : QObject(parent) {}

StorageEngineTest::~StorageEngineTest() {}

void StorageEngineTest::testInitialize() {
  ::ChatSystem::StorageServer::Core::StorageEngine engine;
  QString testPath = QDir::currentPath() + "/test_data";
  QVERIFY(engine.initialize(testPath.toStdString()));
}

} // namespace Tests
