#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <QMutex>
#include <QMap>
#include <QList>

// 内存块结构体
struct MemoryBlock {
    bool used;          // 是否被使用
    int size;           // 内存块大小
    MemoryBlock* next;  // 指向下一个内存块
    char data[0];       // 内存块数据区域
};

// 内存池类
class MemoryPool
{
public:
    // 获取单例实例
    static MemoryPool& getInstance();
    
    // 初始化内存池
    void init(int blockSize = 1024, int blockCount = 100);
    
    // 分配内存
    void* allocate(int size);
    
    // 释放内存
    void deallocate(void* ptr);
    
    // 销毁内存池
    void destroy();
    
    // 获取内存池状态信息
    void getStatus(int& totalBlocks, int& usedBlocks, int& freeBlocks);
    
private:
    MemoryPool();
    ~MemoryPool();
    
    // 禁止拷贝和赋值
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // 分配新的内存页
    MemoryBlock* allocatePage(int blockSize, int blockCount);
    
    QMutex m_mutex;                          // 线程锁
    QMap<int, QList<MemoryBlock*>> m_blocks; // 不同大小的内存块列表
    int m_defaultBlockSize;                  // 默认内存块大小
    int m_defaultBlockCount;                 // 默认内存块数量
};

#endif // MEMORYPOOL_H