#include "memory/heapmanager.h"


MemoryManager* MemoryManager::activeMemoryManager = 0;

MemoryManager::MemoryManager(size_t start, size_t size) {
    activeMemoryManager = this;

    if (size < sizeof(MemoryChunk)) {
        first = 0;
    }
    else {
        first            = (MemoryChunk*)start;
        first->allocated = false;
        first->prev      = 0;
        first->next      = 0;
        first->size      = size - sizeof(MemoryChunk);
    }
}

MemoryManager::~MemoryManager() {
    if (activeMemoryManager == this) {
        activeMemoryManager = 0;
    }
}

void* MemoryManager::malloc(size_t size) {
    MemoryChunk* result = 0;
    for (MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next) {
        if (chunk->size > size && !chunk->allocated) {
            result = chunk;
        }
    }
    // no memory for using
    if (result == 0) return 0;

    // 空闲内存块，大小大于了 size 加上如果再分配一个chunk所需的最小空间，那么需要进行切分
    if (result->size >= size + sizeof(MemoryChunk) + 1) {
        // temp ： 切分后，剩余可用的新chunk
        MemoryChunk* temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
        temp->allocated = false;
        temp->size      = result->size - size - sizeof(MemoryChunk);
        temp->prev      = result;
        temp->next      = result->next;
        if (temp->next != 0) {
            temp->next->prev = temp;
        }
        result->next = temp;
        result->size = size;
    }

    result->allocated = true;
    return (void*)((size_t)result + sizeof(MemoryChunk));
}

void MemoryManager::free(void* ptr) {
    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));

    chunk->allocated = false;

    // merge the spaces freed
    if (chunk->prev != 0 && !chunk->prev->allocated) {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        if (chunk->next != 0) {
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }

    // merge free neighbors
    if (chunk->next != 0 && !chunk->next->allocated) {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        if (chunk->next != 0) {
            chunk->next->prev = chunk;
        }
    }
}

void* operator new(size_t size) {
    if (MemoryManager::activeMemoryManager == 0) return 0;
    return MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](size_t size) {
    if (MemoryManager::activeMemoryManager == 0) return 0;
    return MemoryManager::activeMemoryManager->malloc(size);
}

// placement new
void* operator new(size_t size, void* ptr) { return ptr; }

void* operator new[](size_t size, void* ptr) { return ptr; }

void operator delete(void* ptr) {
    if (MemoryManager::activeMemoryManager != 0) {
        MemoryManager::activeMemoryManager->free(ptr);
    }
}

void operator delete[](void* ptr) {
    if (MemoryManager::activeMemoryManager != 0) {
        MemoryManager::activeMemoryManager->free(ptr);
    }
}