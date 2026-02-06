#include "test_memory_pool.h"
#include "../../Common/Utils/MemoryPool.h"
#include <QtTest>
#include <chrono>
#include <thread>
#include <vector>

namespace Tests {

MemoryPoolTest::MemoryPoolTest(QObject *parent) : QObject(parent) {}

MemoryPoolTest::~MemoryPoolTest() {}

void MemoryPoolTest::testMemoryAllocation() {
  MemoryPool &pool = MemoryPool::getInstance();
  // pool.init(1024 * 1024); // Init might be called once, relying on default or
  // previous state if singleton For testing purposes, we assume it's
  // initialized or lazy initialized safely.

  void *ptr1 = pool.allocate(100);
  void *ptr2 = pool.allocate(200);
  void *ptr3 = pool.allocate(300);

  QVERIFY(ptr1 != nullptr);
  QVERIFY(ptr2 != nullptr);
  QVERIFY(ptr3 != nullptr);

  pool.deallocate(ptr1);
  pool.deallocate(ptr2);
  pool.deallocate(ptr3);
}

void MemoryPoolTest::testMemoryDeallocation() {
  MemoryPool &pool = MemoryPool::getInstance();

  void *ptr1 = pool.allocate(100);
  void *ptr2 = pool.allocate(200);

  pool.deallocate(ptr1);
  pool.deallocate(ptr2);

  int total, used, free;
  pool.getStatus(total, used, free);
  QCOMPARE(used, 0);
}

void MemoryPoolTest::testMultipleAllocations() {
  MemoryPool &pool = MemoryPool::getInstance();

  std::vector<void *> pointers;
  for (int i = 0; i < 100; ++i) {
    void *ptr = pool.allocate(100);
    QVERIFY(ptr != nullptr);
    pointers.push_back(ptr);
  }

  for (void *ptr : pointers) {
    pool.deallocate(ptr);
  }

  int total, used, free;
  pool.getStatus(total, used, free);
  QCOMPARE(used, 0);
}

void MemoryPoolTest::testThreadSafety() {
  MemoryPool &pool = MemoryPool::getInstance();

  auto allocateFunc = [&pool]() {
    for (int i = 0; i < 50; ++i) {
      void *ptr = pool.allocate(100);
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

  int total, used, free;
  pool.getStatus(total, used, free);
  QCOMPARE(used, 0);
}

void MemoryPoolTest::testMemoryPoolStatistics() {
  MemoryPool &pool = MemoryPool::getInstance();

  void *ptr1 = pool.allocate(100);
  void *ptr2 = pool.allocate(200);

  int total, used, free;
  pool.getStatus(total, used, free);
  QCOMPARE(used, 2);
  QVERIFY(free > 0);

  pool.deallocate(ptr1);

  pool.getStatus(total, used, free);
  QCOMPARE(used, 1);
}

void MemoryPoolTest::testMemoryPoolEdgeCases() {
  MemoryPool &pool = MemoryPool::getInstance();
  // Can't re-init with smaller size easily if singleton already alive,
  // but we can try to allocate.

  void *ptr1 = pool.allocate(1024);
  QVERIFY(ptr1 != nullptr);

  void *ptr2 = pool.allocate(1);
  QVERIFY(ptr2 == nullptr);

  pool.deallocate(ptr1);

  void *ptr3 = pool.allocate(1024);
  QVERIFY(ptr3 != nullptr);
}

void MemoryPoolTest::testMemoryPoolPerformance() {
  MemoryPool &pool = MemoryPool::getInstance();

  auto start = std::chrono::high_resolution_clock::now();

  std::vector<void *> pointers;
  for (int i = 0; i < 1000; ++i) {
    void *ptr = pool.allocate(100);
    if (ptr) {
      pointers.push_back(ptr);
    }
  }

  for (void *ptr : pointers) {
    pool.deallocate(ptr);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  QVERIFY(duration.count() < 1000);
}

} // namespace Tests
