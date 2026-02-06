#include "test_load_balancer.h"
#include "../../MetaServer/Core/LoadBalancer.h"
#include <QtTest>

namespace Tests {

LoadBalancerTest::LoadBalancerTest(QObject *parent) : QObject(parent) {}

LoadBalancerTest::~LoadBalancerTest() {}

void LoadBalancerTest::testAddStorageServer() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  QVERIFY(lb.addStorageServer(1, "127.0.0.1", 8001, 1));
  QVERIFY(lb.addStorageServer(2, "127.0.0.2", 8002, 2));
  QVERIFY(lb.addStorageServer(3, "127.0.0.3", 8003, 1));

  std::string ip;
  uint16_t port;
  QVERIFY(lb.getServerInfo(1, ip, port));
  QCOMPARE(QString::fromStdString(ip), QString("127.0.0.1"));
  QCOMPARE(port, static_cast<uint16_t>(8001));
}

void LoadBalancerTest::testRemoveStorageServer() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  lb.addStorageServer(1, "127.0.0.1", 8001, 1);
  QVERIFY(lb.removeStorageServer(1));

  std::string ip;
  uint16_t port;
  QVERIFY(!lb.getServerInfo(1, ip, port));
}

void LoadBalancerTest::testSelectBestServer() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  lb.addStorageServer(1, "127.0.0.1", 8001, 1);
  lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50);

  uint32_t selected = lb.selectBestServer(1024 * 1024);
  QCOMPARE(selected, static_cast<uint32_t>(1));
}

void LoadBalancerTest::testWeightedRoundRobin() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  lb.addStorageServer(1, "127.0.0.1", 8001, 1);
  lb.addStorageServer(2, "127.0.0.2", 8002, 2);
  lb.addStorageServer(3, "127.0.0.3", 8003, 1);

  lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50);
  lb.updateServerStatus(2, 20.0f, 30.0f, 40.0f, 100);
  lb.updateServerStatus(3, 30.0f, 40.0f, 50.0f, 150);

  std::vector<uint32_t> selections;
  for (int i = 0; i < 10; ++i) {
    uint32_t selected = lb.selectBestServer(1024 * 1024);
    selections.push_back(selected);
  }

  int count1 = std::count(selections.begin(), selections.end(), 1);
  int count2 = std::count(selections.begin(), selections.end(), 2);
  int count3 = std::count(selections.begin(), selections.end(), 3);

  QCOMPARE(count1, 2);
  QCOMPARE(count2, 6);
  QCOMPARE(count3, 2);
}

void LoadBalancerTest::testServerStatusUpdate() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  lb.addStorageServer(1, "127.0.0.1", 8001, 1);

  QVERIFY(lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50));

  std::vector<uint32_t> onlineServers = lb.getOnlineServers();
  QCOMPARE(onlineServers.size(), static_cast<size_t>(1));
  QCOMPARE(onlineServers[0], static_cast<uint32_t>(1));
}

void LoadBalancerTest::testGetOnlineServers() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  lb.addStorageServer(1, "127.0.0.1", 8001, 1);
  lb.addStorageServer(2, "127.0.0.2", 8002, 2);

  lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50);

  std::vector<uint32_t> onlineServers = lb.getOnlineServers();
  QCOMPARE(onlineServers.size(), static_cast<size_t>(1));

  lb.updateServerStatus(2, 20.0f, 30.0f, 40.0f, 100);

  onlineServers = lb.getOnlineServers();
  QCOMPARE(onlineServers.size(), static_cast<size_t>(2));
}

void LoadBalancerTest::testLoadScoreCalculation() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  lb.addStorageServer(1, "127.0.0.1", 8001, 1);
  lb.addStorageServer(2, "127.0.0.2", 8002, 2);

  lb.updateServerStatus(1, 50.0f, 60.0f, 70.0f, 100);
  lb.updateServerStatus(2, 10.0f, 20.0f, 30.0f, 50);

  uint32_t selected = lb.selectBestServer(1024 * 1024);
  QCOMPARE(selected, static_cast<uint32_t>(2));
}

void LoadBalancerTest::testServerOfflineHandling() {
  ::ChatSystem::MetaServer::Core::LoadBalancer lb;

  lb.addStorageServer(1, "127.0.0.1", 8001, 1);
  lb.addStorageServer(2, "127.0.0.2", 8002, 2);

  lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50);

  uint32_t selected = lb.selectBestServer(1024 * 1024);
  QCOMPARE(selected, static_cast<uint32_t>(1));

  lb.updateServerStatus(1, 0.0f, 0.0f, 0.0f, 0);

  selected = lb.selectBestServer(1024 * 1024);
  QCOMPARE(selected, static_cast<uint32_t>(2));
}

} // namespace Tests
