#include "test_memory_pool.h"
#include "../../Common/Utils/MemoryPool.h"
#include <QtTest>
#include <thread>
#include <vector>

namespace Tests {

MemoryPoolTest::MemoryPoolTest(QObject* parent)
    : QObject(parent)
{
}

MemoryPoolTest::~MemoryPoolTest()
{
}

void MemoryPoolTest::testMemoryAllocation()
{
    ChatSystem::Utils::MemoryPool pool(1024 * 1024);
    
    void* ptr1 = pool.allocate(100);
    void* ptr2 = pool.allocate(200);
    void* ptr3 = pool.allocate(300);
    
    QVERIFY(ptr1 != nullptr);
    QVERIFY(ptr2 != nullptr);
    QVERIFY(ptr3 != nullptr);
    
    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
    pool.deallocate(ptr3);
}

void MemoryPoolTest::testMemoryDeallocation()
{
    ChatSystem::Utils::MemoryPool pool(1024 * 1024);
    
    void* ptr1 = pool.allocate(100);
    void* ptr2 = pool.allocate(200);
    
    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
    
    QVERIFY(pool.getAllocatedCount() == 0);
}

void MemoryPoolTest::testMultipleAllocations()
{
    ChatSystem::Utils::MemoryPool pool(1024 * 1024);
    
    std::vector<void*> pointers;
    for (int i = 0; i < 100; ++i) {
        void* ptr = pool.allocate(100);
        QVERIFY(ptr != nullptr);
        pointers.push_back(ptr);
    }
    
    for (void* ptr : pointers) {
        pool.deallocate(ptr);
    }
    
    QVERIFY(pool.getAllocatedCount() == 0);
}

void MemoryPoolTest::testThreadSafety()
{
    ChatSystem::Utils::MemoryPool pool(1024 * 1024);
    
    auto allocateFunc = [&pool]() {
        for (int i = 0; i < 50; ++i) {
            void* ptr = pool.allocate(100);
            if (ptr) {
                pool.deallocate(ptr);
            }
        }
    };
    
    std::thread t1(allocateFunc);
    std::thread t2(allocateFunc);
    std::thread t3(allocateFunc);
    
    t1.join();
    t2.join();
    t3.join();
    
    QVERIFY(pool.getAllocatedCount() == 0);
}

void MemoryPoolTest::testMemoryPoolStatistics()
{
    ChatSystem::Utils::MemoryPool pool(1024 * 1024);
    
    void* ptr1 = pool.allocate(100);
    void* ptr2 = pool.allocate(200);
    
    QVERIFY(pool.getAllocatedCount() == 2);
    QVERIFY(pool.getFreeCount() > 0);
    
    pool.deallocate(ptr1);
    
    QVERIFY(pool.getAllocatedCount() == 1);
}

} // namespace Tests