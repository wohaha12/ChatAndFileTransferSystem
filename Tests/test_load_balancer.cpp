#include "test_load_balancer.h"
#include "../../MetaServer/Core/LoadBalancer.h"
#include <QtTest>

namespace Tests {

/**
 * @brief 构造函数
 */
LoadBalancerTest::LoadBalancerTest(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief 析构函数
 */
LoadBalancerTest::~LoadBalancerTest()
{
}

/**
 * @brief 测试添加存储服务器
 * @details 验证能够正确添加存储服务器
 */
void LoadBalancerTest::testAddStorageServer()
{
    MetaServer::Core::LoadBalancer lb;
    
    QVERIFY(lb.addStorageServer(1, "127.0.0.1", 8001, 1));
    QVERIFY(lb.addStorageServer(2, "127.0.0.2", 8002, 2));
    QVERIFY(lb.addStorageServer(3, "127.0.0.3", 8003, 1));
    
    std::string ip;
    uint16_t port;
    QVERIFY(lb.getServerInfo(1, ip, port));
    QCOMPARE(QString::fromStdString(ip), QString("127.0.0.1"));
    QCOMPARE(port, static_cast<uint16_t>(8001));
}

/**
 * @brief 测试移除存储服务器
 * @details 验证能够正确移除存储服务器
 */
void LoadBalancerTest::testRemoveStorageServer()
{
    MetaServer::Core::LoadBalancer lb;
    
    lb.addStorageServer(1, "127.0.0.1", 8001, 1);
    QVERIFY(lb.removeStorageServer(1));
    
    std::string ip;
    uint16_t port;
    QVERIFY(!lb.getServerInfo(1, ip, port));
}

/**
 * @brief 测试选择最佳服务器
 * @details 验证能够根据负载选择最佳服务器
 */
void LoadBalancerTest::testSelectBestServer()
{
    MetaServer::Core::LoadBalancer lb;
    
    lb.addStorageServer(1, "127.0.0.1", 8001, 1);
    lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50);
    
    uint32_t selected = lb.selectBestServer(1024 * 1024);
    QCOMPARE(selected, static_cast<uint32_t>(1));
}

/**
 * @brief 测试加权轮询算法
 * @details 验证加权轮询算法的正确性
 */
void LoadBalancerTest::testWeightedRoundRobin()
{
    MetaServer::Core::LoadBalancer lb;
    
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

/**
 * @brief 测试服务器状态更新
 * @details 验证能够正确更新服务器状态
 */
void LoadBalancerTest::testServerStatusUpdate()
{
    MetaServer::Core::LoadBalancer lb;
    
    lb.addStorageServer(1, "127.0.0.1", 8001, 1);
    
    QVERIFY(lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50));
    
    std::vector<uint32_t> onlineServers = lb.getOnlineServers();
    QCOMPARE(onlineServers.size(), static_cast<size_t>(1));
    QCOMPARE(onlineServers[0], static_cast<uint32_t>(1));
}

/**
 * @brief 测试获取在线服务器列表
 * @details 验证能够正确获取在线服务器列表
 */
void LoadBalancerTest::testGetOnlineServers()
{
    MetaServer::Core::LoadBalancer lb;
    
    lb.addStorageServer(1, "127.0.0.1", 8001, 1);
    lb.addStorageServer(2, "127.0.0.2", 8002, 2);
    
    lb.updateServerStatus(1, 10.0f, 20.0f, 30.0f, 50);
    
    std::vector<uint32_t> onlineServers = lb.getOnlineServers();
    QCOMPARE(onlineServers.size(), static_cast<size_t>(1));
    
    lb.updateServerStatus(2, 20.0f, 30.0f, 40.0f, 100);
    
    onlineServers = lb.getOnlineServers();
    QCOMPARE(onlineServers.size(), static_cast<size_t>(2));
}

/**
 * @brief 测试负载分数计算
 * @details 验证负载分数计算的准确性
 */
void LoadBalancerTest::testLoadScoreCalculation()
{
    MetaServer::Core::LoadBalancer lb;
    
    lb.addStorageServer(1, "127.0.0.1", 8001, 1);
    lb.addStorageServer(2, "127.0.0.2", 8002, 2);
    
    lb.updateServerStatus(1, 50.0f, 60.0f, 70.0f, 100);
    lb.updateServerStatus(2, 10.0f, 20.0f, 30.0f, 50);
    
    uint32_t selected = lb.selectBestServer(1024 * 1024);
    QCOMPARE(selected, static_cast<uint32_t>(2));
}

/**
 * @brief 测试服务器离线处理
 * @details 验证能够正确处理服务器离线情况
 */
void LoadBalancerTest::testServerOfflineHandling()
{
    MetaServer::Core::LoadBalancer lb;
    
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
