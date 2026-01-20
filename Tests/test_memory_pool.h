#pragma once

#include <QtTest/QtTest>
#include <memory>

namespace Tests {

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
};

} // namespace Tests