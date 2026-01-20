#ifndef COMMON_UTILS_MEMORYPOOL_H
#define COMMON_UTILS_MEMORYPOOL_H

#include <cstdint>
#include <QMutex>
#include <QMap>
#include <QList>

/**
 * @brief 内存块结构体
 * @details 用于管理内存池中的单个内存块
 */
struct MemoryBlock {
    bool used;          // 是否被使用
    int size;           // 内存块大小
    MemoryBlock* next;  // 指向下一个内存块
    char data[0];       // 内存块数据区域（柔性数组）
};

/**
 * @brief 内存池类
 * @details 提供高效的内存分配和管理功能，减少内存碎片
 */
class MemoryPool {
public:
    /**
     * @brief 获取单例实例
     * @return 内存池单例引用
     */
    static MemoryPool& getInstance();
    
    /**
     * @brief 初始化内存池
     * @param blockSize 默认内存块大小（字节）
     * @param blockCount 默认内存块数量
     */
    void init(int blockSize = 4096, int blockCount = 100);
    
    /**
     * @brief 分配内存
     * @param size 请求的内存大小（字节）
     * @return 分配的内存指针，失败返回nullptr
     */
    void* allocate(int size);
    
    /**
     * @brief 释放内存
     * @param ptr 要释放的内存指针
     */
    void deallocate(void* ptr);
    
    /**
     * @brief 销毁内存池
     */
    void destroy();
    
    /**
     * @brief 获取内存池状态信息
     * @param totalBlocks 总块数
     * @param usedBlocks 已使用块数
     * @param freeBlocks 空闲块数
     */
    void getStatus(int& totalBlocks, int& usedBlocks, int& freeBlocks);
    
private:
    MemoryPool();
    ~MemoryPool();
    
    /**
     * @brief 禁止拷贝和赋值
     */
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    /**
     * @brief 分配新的内存页
     * @param blockSize 内存块大小
     * @param blockCount 内存块数量
     * @return 第一个内存块指针
     */
    MemoryBlock* allocatePage(int blockSize, int blockCount);
    
    QMutex m_mutex;                          // 线程锁
    QMap<int, QList<MemoryBlock*>> m_blocks; // 不同大小的内存块列表
    int m_defaultBlockSize;                  // 默认内存块大小
    int m_defaultBlockCount;                 // 默认内存块数量
};

#endif // COMMON_UTILS_MEMORYPOOL_H
