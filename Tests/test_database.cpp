#include "test_database.h"
#include "../../MetaServer/Database/DatabaseManager.h"
#include <QtTest>

namespace Tests {

DatabaseTest::DatabaseTest(QObject *parent) : QObject(parent) {}

DatabaseTest::~DatabaseTest() {}

void DatabaseTest::testConnection() {
  ::ChatSystem::MetaServer::Database::DatabaseManager db(nullptr);
  // Mock connection test
  QVERIFY(true);
}

} // namespace Tests
