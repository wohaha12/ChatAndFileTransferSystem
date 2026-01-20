#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

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
};

} // namespace Tests