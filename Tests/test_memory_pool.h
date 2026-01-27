#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

/**
 * @brief 内存池测试类
 * @details 测试内存池的分配、释放、线程安全等功能
 */
class MemoryPoolTest : public QObject {
    Q_OBJECT
public:
    explicit MemoryPoolTest(QObject* parent = nullptr);
    ~MemoryPoolTest() override;

private slots:
    void testMemoryAllocation();
    void testMemoryDeallocation();
    void testMultipleAllocations();
    void testThreadSafety();
    void testMemoryPoolStatistics();
    void testMemoryPoolEdgeCases();
    void testMemoryPoolPerformance();
};

} // namespace Tests
