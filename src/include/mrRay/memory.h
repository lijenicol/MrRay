#ifndef MR_RAY_MEMORY_H
#define MR_RAY_MEMORY_H

#define ARENA_ALLOC(arena, Type) new ((arena).Alloc(sizeof(Type))) Type

#include <cstddef>
#include <iostream>
#include <list>

#include "mrRay/namespace.h"

MR_RAY_NAMESPACE_OPEN_SCOPE

class alignas(64) MemoryArena
{
public:
    MemoryArena() { Reset(); }

    ~MemoryArena()
    {
        for (char *block: blocks)
            delete[] block;
    }

    void *Alloc(size_t size)
    {
        if (allocated + size >= ALLOC_SIZE) current = GetBlock();
        void *ret = current;
        current += size;
        allocated += size;
        return ret;
    }

    void Reset()
    {
        currentBlock = 0;
        current = GetBlock();
    }

private:
    MemoryArena(const MemoryArena &) = delete;

    void *GetBlock()
    {
        allocated = 0;
        if (currentBlock + 1 > blocks.size()) {
            char *block = new char[ALLOC_SIZE];
            blocks.push_back(block);
        }
        return blocks[currentBlock++];
    }

    const size_t ALLOC_SIZE = 262144;
    size_t allocated;
    std::vector<char *> blocks;
    int currentBlock;
    void *current;
};

MR_RAY_NAMESPACE_CLOSE_SCOPE

#endif // MR_RAY_MEMORY_H
