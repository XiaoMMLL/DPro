#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <mutex>

//#define PRINT_INFO
//#define FUNC_ENTER
//#define MEM_FREE_TRACE

#define INFO_STREAM stdout

#ifdef FUNC_ENTER
    #define FUNC_ENTER()  fprintf(INFO_STREAM, "Enter function %s\n", __func__)
#else
    #define FUNC_ENTER() void(0)
#endif

#ifdef PRINT_INFO
    #define PRINFO(...) fprintf(INFO_STREAM, __VA_ARGS__)
#else
    #define PRINFO(...) void(0)
#endif

#ifdef MEM_FREE_TRACE
    #define MEM_FREE(...) fprintf(INFO_STREAM, __VA_ARGS__)
#else
    #define MEM_FREE(...) void(0)
#endif

class Allocator
{
    unsigned int num;
    size_t size;
    char* pMask;
    char* pMem;
    unsigned int* prefn;
    unsigned int nuse;
    Allocator();
    Allocator(Allocator&);
public:
    Allocator(unsigned int n, size_t size);
    Allocator& operator=(Allocator&);
    ~Allocator();
    void* malloc(size_t size);
    bool mfree(void* p);
    size_t getSize() const { return size;}
    unsigned int getNum() const { return num; }
    unsigned int getUsedNum() const { return nuse; }
    void print() const;
};

class AllocatorsManager
{
    static AllocatorsManager allocators;
    struct AllocNode
    {
        Allocator* palloc;
        AllocNode* next;
    } *head;
    AllocatorsManager() {}
    AllocatorsManager(AllocatorsManager&);
    AllocatorsManager& operator=(AllocatorsManager&);
public:
    ~AllocatorsManager();
    static AllocatorsManager& instance()
    {
        return allocators;
    }
    void* malloc(size_t size);
    void mfree(void* p);
    bool addAllocator(Allocator* pAllocator);
    long totalSize() const;
    long totalUsed() const;
    long totalUsedPercent() const { return totalUsed() * 100 / totalSize(); }
    long totalUsedNum() const;
    void print() const;
};


void* operator new(size_t size);
void operator delete(void* p) noexcept;
void* operator new[](size_t size);
void operator delete[](void* p) noexcept;


#endif // ALLOCATOR_H
