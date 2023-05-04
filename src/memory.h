#ifndef MEMORY_H
#define MEMORY_H

#include<list>
#include<cstddef>
#include<iostream>

#define ARENA_ALLOC(arena, Type) new ((arena).Alloc(sizeof(Type))) Type

class alignas(64) MemoryArena
{
public:
    MemoryArena() { current = NewBlock(); }

    ~MemoryArena() 
    {
        for (char* block : blocks) 
            delete[] block; 
    }

    void* Alloc(size_t size)
    {
        if (allocated + size >= ALLOC_SIZE)
            current = NewBlock();
        void* ret = current;
        current += size;
        allocated += size;
        return ret;
    }

    static size_t blockCount;

private:
    MemoryArena(const MemoryArena&) = delete;

    void* NewBlock()
    {
        // FIXME: Probs don't need
        blockCount++;
        allocated = 0;
        // Does this need to be aligned properly?? 
        char* block = new char[ALLOC_SIZE];
        blocks.push_back(block);
        return block;
    }

    const size_t ALLOC_SIZE = 262144;
    size_t allocated;
    std::list<char*> blocks;
    void* currentBlock;
    void* current;
};

#endif
