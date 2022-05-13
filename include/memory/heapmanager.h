#ifndef __HEAPMANAGEMENT_H
#define __HEAPMANAGEMENT_H

#include "common/types.h"

// 双向链表形式组织 heap 内存
struct MemoryChunk {
    MemoryChunk* next;
    MemoryChunk* prev;
    bool         allocated;
    size_t       size;
};

class MemoryManager {
public:
    MemoryManager(size_t first, size_t size);
    ~MemoryManager();

    void* malloc(size_t size);
    void  free(void* ptr);

    static MemoryManager* activeMemoryManager;

private:
    MemoryChunk* first;
};

void* operator new(size_t size);
void* operator new[](size_t size);

void* operator new(size_t size, void* ptr);
void* operator new[](size_t size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif