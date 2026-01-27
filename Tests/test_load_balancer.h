#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

/**
 * @brief 负载均衡器测试类
 * @details 测试负载均衡器的服务器管理、选择算法等功能
 */
class LoadBalancerTest : public QObject {
    Q_OBJECT
public:
    explicit LoadBalancerTest(QObject* parent = nullptr);
    ~LoadBalancerTest() override;

private slots:
    void testAddStorageServer();
    void testRemoveStorageServer();
    void testSelectBestServer();
    void testWeightedRoundRobin();
    void testServerStatusUpdate();
    void testGetOnlineServers();
    void testLoadScoreCalculation();
    void testServerOfflineHandling();
};

} // namespace Tests
