#include "test_memory_pool.h"
#include "../../Common/Utils/MemoryPool.h"
#include <QtTest>
#include <thread>
#include <vector>
#include <chrono>

namespace Tests {

/**
 * @brief 构造函数
 */
MemoryPoolTest::MemoryPoolTest(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief 析构函数
 */
MemoryPoolTest::~MemoryPoolTest()
{
}

/**
 * @brief 测试内存分配
 * @details 验证内存池能够正确分配内存
 */
void MemoryPoolTest::testMemoryAllocation()
{
    Common::Utils::MemoryPool pool(1024 * 1024);
    
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

/**
 * @brief 测试内存释放
 * @details 验证内存释放后分配计数正确归零
 */
void MemoryPoolTest::testMemoryDeallocation()
{
    Common::Utils::MemoryPool pool(1024 * 1024);
    
    void* ptr1 = pool.allocate(100);
    void* ptr2 = pool.allocate(200);
    
    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
    
    QCOMPARE(pool.getAllocatedCount(), 0);
}

/**
 * @brief 测试多次分配
 * @details 验证内存池能够处理多次分配和释放
 */
void MemoryPoolTest::testMultipleAllocations()
{
    Common::Utils::MemoryPool pool(1024 * 1024);
    
    std::vector<void*> pointers;
    for (int i = 0; i < 100; ++i) {
        void* ptr = pool.allocate(100);
        QVERIFY(ptr != nullptr);
        pointers.push_back(ptr);
    }
    
    for (void* ptr : pointers) {
        pool.deallocate(ptr);
    }
    
    QCOMPARE(pool.getAllocatedCount(), 0);
}

/**
 * @brief 测试线程安全
 * @details 验证内存池在多线程环境下的安全性
 */
void MemoryPoolTest::testThreadSafety()
{
    Common::Utils::MemoryPool pool(1024 * 1024);
    
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
    
    QCOMPARE(pool.getAllocatedCount(), 0);
}

/**
 * @brief 测试内存池统计
 * @details 验证内存池统计信息的准确性
 */
void MemoryPoolTest::testMemoryPoolStatistics()
{
    Common::Utils::MemoryPool pool(1024 * 1024);
    
    void* ptr1 = pool.allocate(100);
    void* ptr2 = pool.allocate(200);
    
    QCOMPARE(pool.getAllocatedCount(), 2);
    QVERIFY(pool.getFreeCount() > 0);
    
    pool.deallocate(ptr1);
    
    QCOMPARE(pool.getAllocatedCount(), 1);
}

/**
 * @brief 测试边界情况
 * @details 测试内存池在边界条件下的行为
 */
void MemoryPoolTest::testMemoryPoolEdgeCases()
{
    Common::Utils::MemoryPool pool(1024);
    
    void* ptr1 = pool.allocate(1024);
    QVERIFY(ptr1 != nullptr);
    
    void* ptr2 = pool.allocate(1);
    QVERIFY(ptr2 == nullptr);
    
    pool.deallocate(ptr1);
    
    void* ptr3 = pool.allocate(1024);
    QVERIFY(ptr3 != nullptr);
}

/**
 * @brief 测试内存池性能
 * @details 测试内存池的性能表现
 */
void MemoryPoolTest::testMemoryPoolPerformance()
{
    Common::Utils::MemoryPool pool(1024 * 1024);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<void*> pointers;
    for (int i = 0; i < 1000; ++i) {
        void* ptr = pool.allocate(100);
        if (ptr) {
            pointers.push_back(ptr);
        }
    }
    
    for (void* ptr : pointers) {
        pool.deallocate(ptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    QVERIFY(duration.count() < 100);
}

} // namespace Tests
