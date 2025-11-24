#include "memorypool.h"
#include <QDebug>
#include <cstdlib>

MemoryPool::MemoryPool()
    : m_defaultBlockSize(1024)
    , m_defaultBlockCount(100)
{}

MemoryPool::~MemoryPool()
{
    destroy();
}

MemoryPool& MemoryPool::getInstance()
{
    static MemoryPool instance;
    return instance;
}

void MemoryPool::init(int blockSize, int blockCount)
{
    QMutexLocker locker(&m_mutex);
    
    m_defaultBlockSize = blockSize;
    m_defaultBlockCount = blockCount;
    
    // 预先分配默认大小的内存块
    if (m_blocks[m_defaultBlockSize].isEmpty()) {
        allocatePage(m_defaultBlockSize, m_defaultBlockCount);
    }
    
    qDebug() << "内存池初始化成功，默认块大小:" << m_defaultBlockSize << "，默认块数量:" << m_defaultBlockCount;
}

MemoryBlock* MemoryPool::allocatePage(int blockSize, int blockCount)
{
    // 计算一页内存的总大小
    int pageSize = sizeof(MemoryBlock) + blockSize;
    int totalSize = pageSize * blockCount;
    
    // 分配一页内存
    char* page = (char*)malloc(totalSize);
    if (page == nullptr) {
        qDebug() << "内存分配失败，大小:" << totalSize;
        return nullptr;
    }
    
    // 将页内存分割成多个内存块
    MemoryBlock* firstBlock = nullptr;
    MemoryBlock* prevBlock = nullptr;
    
    for (int i = 0; i < blockCount; ++i) {
        MemoryBlock* block = (MemoryBlock*)(page + i * pageSize);
        block->used = false;
        block->size = blockSize;
        block->next = nullptr;
        
        if (prevBlock) {
            prevBlock->next = block;
        } else {
            firstBlock = block;
        }
        
        prevBlock = block;
        
        // 将新内存块添加到内存池
        m_blocks[blockSize].append(block);
    }
    
    qDebug() << "分配新内存页成功，块大小:" << blockSize << "，块数量:" << blockCount;
    
    return firstBlock;
}

void* MemoryPool::allocate(int size)
{
    if (size <= 0) {
        return nullptr;
    }
    
    QMutexLocker locker(&m_mutex);
    
    // 找到最合适的内存块大小
    int blockSize = m_defaultBlockSize;
    if (size > m_defaultBlockSize) {
        // 如果请求的内存大于默认块大小，使用实际大小
        blockSize = ((size + 7) / 8) * 8; // 对齐到8字节
    }
    
    // 查找可用的内存块
    QList<MemoryBlock*>& blocks = m_blocks[blockSize];
    
    for (MemoryBlock* block : blocks) {
        if (!block->used) {
            block->used = true;
            qDebug() << "分配内存块成功，大小:" << blockSize;
            return block->data;
        }
    }
    
    // 如果没有可用内存块，分配新的内存页
    MemoryBlock* newBlock = allocatePage(blockSize, m_defaultBlockCount);
    if (newBlock != nullptr) {
        newBlock->used = true;
        qDebug() << "分配新内存块成功，大小:" << blockSize;
        return newBlock->data;
    }
    
    // 如果内存分配失败，返回nullptr
    qDebug() << "内存分配失败，请求大小:" << size;
    return nullptr;
}

void MemoryPool::deallocate(void* ptr)
{
    if (ptr == nullptr) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    // 找到对应的内存块
    MemoryBlock* block = (MemoryBlock*)((char*)ptr - offsetof(MemoryBlock, data));
    
    // 标记内存块为未使用
    block->used = false;
    
    qDebug() << "释放内存块成功，大小:" << block->size;
}

void MemoryPool::destroy()
{
    QMutexLocker locker(&m_mutex);
    
    // 释放所有内存块
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it) {
        QList<MemoryBlock*>& blocks = it.value();
        
        if (!blocks.isEmpty()) {
            // 释放整个内存页
            free(blocks.first());
            blocks.clear();
        }
    }
    
    m_blocks.clear();
    
    qDebug() << "内存池销毁成功";
}

void MemoryPool::getStatus(int& totalBlocks, int& usedBlocks, int& freeBlocks)
{
    QMutexLocker locker(&m_mutex);
    
    totalBlocks = 0;
    usedBlocks = 0;
    
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it) {
        QList<MemoryBlock*>& blocks = it.value();
        totalBlocks += blocks.size();
        
        for (MemoryBlock* block : blocks) {
            if (block->used) {
                usedBlocks++;
            }
        }
    }
    
    freeBlocks = totalBlocks - usedBlocks;
}
