#include "allocator.h"

using namespace std;

AllocatorsManager AllocatorsManager::allocators;
// memory allocate global mutex
std::mutex mtx;

Allocator::Allocator(unsigned int n, size_t size)
{
    FUNC_ENTER();

    this->num = n;
    this->size = size;
    this->nuse = 0;

    if (n == 0 || size == 0)
    {
        PRINFO("Invalid initialized parameter(n = %d, size = %s) for class Allocator.\n", n, size);
        return;
    }

    pMask = pMem = NULL;
    prefn = NULL;
    pMask = (char*)std::malloc((n > 8) ? (n>>3) : 1);  
    pMem = (char*)std::malloc(n * size);
    prefn = (unsigned int*)std::malloc(sizeof(unsigned int));
    if (pMem == NULL || pMask == NULL || prefn == NULL)
    {
        free(pMask);
        free(pMem);
        PRINFO("Allocator initializing... std::malloc fail.\n");
        return;
    }

    memset((void*)pMask, 0, sizeof(pMask));
    memset((void*)pMem, 0, sizeof(pMem));
    *prefn = 1;

    return;
}

Allocator& Allocator::operator=(Allocator& allocator)
{
    memcpy(this, &allocator, sizeof(Allocator));
    *prefn += 1;    // refrence num for alloctor used memory. pMask, pMem, prefn
    return *this;
}

Allocator::~Allocator()
{
    FUNC_ENTER();

    if (num != 0 && size != 0 && *prefn == 1)
    {
        MEM_FREE("Memory free for block num = %u, size = %uB.\n", num, size);
        free(pMem);
        free(pMask);
        free(prefn);
        return;
    }

    (*prefn)--;
}

void* Allocator::malloc(size_t size)
{
    FUNC_ENTER();

    if (size > this->size)
    {
        return NULL;
    }

    for (unsigned int i = 0; i < num; i++)
    {
        if (*(pMask + (i >> 3)) & (1 << (i % 8)))
        {
            continue;
        }
        *(pMask + (i >> 3)) |= (1 << (i % 8));
        nuse++;
        return pMem + i * this->size;
    }

    PRINFO("Allocator::malloc(%u) fail in block(num = %u, size = %u, nuse = %u). All memory is used.\n", size, num, this->size, nuse);
    return NULL;
}

bool Allocator::mfree(void* p)
{
    FUNC_ENTER();
    if (p < pMem || p > pMem + size * num)
    {
        return false;
    }

    for (unsigned int i = 0 ; i < num; i++)
    {
        if ((p == pMem + size * i) && (*(pMask + (i >> 3)) & (1 << (i % 8))))
        {
            *(pMask + (i >> 3)) &= ~(1 << (i % 8));
            nuse--;
            return true;
        }
    }

    PRINFO("Invalid pointer(%p) for Allocator::mfree.\n", p);
    return false;
}

void Allocator::print() const
{
    PRINFO("Alloctor (num = %u, size = %u, used num = %u, refn = %u)\n", num, size, nuse, *prefn);
    PRINFO("bitmask:");
    for (unsigned int i = 0; i < num ; i++)
    {
        if (*(pMask + (i >> 3)) & (1 << (i % 8)))
        {
            PRINFO("%u", 1);
        }
        else
        {
            PRINFO("%u", 0);
        }

        if ((i + 1) % 8 == 0)
        {
            PRINFO(" ");
        }
    }
    PRINFO("\n");
    return;
}

AllocatorsManager::~AllocatorsManager()
{
    FUNC_ENTER();

    MEM_FREE("AllocatorsManager::~AllocatorsManager() Called...\n");
    AllocNode *pNode;
    while(head != NULL)
    {
        pNode = head->next;
        head->palloc->~Allocator();
        free(head);
        head = pNode;
    }
}

void* AllocatorsManager::malloc(size_t size)
{
    FUNC_ENTER();

    if (head == NULL)
    {
        PRINFO("AllocatorsManager initial failed, no memory pool for allocating.\n");
        return NULL;
    }

    std::unique_lock<std::mutex> lck(mtx);
    AllocNode *pNode = head;
    void* palloc = NULL;
    while(pNode != NULL)
    {
        palloc = pNode->palloc->malloc(size);
        if (palloc)
            return palloc;
        pNode = pNode->next;
    }

    PRINFO("Block size(%u) allocate failed in AllocatorsManager::malloc.\n", size);
    return NULL;
}

void AllocatorsManager::mfree(void* p)
{
    FUNC_ENTER();

    if (p == NULL)
    {
        return;
    }

    if (head == NULL)
    {
        PRINFO("AllocatorsManager fatal error...\n");
        return;
    }
    std::unique_lock<std::mutex> lck(mtx);
    AllocNode *pNode = head;
    while(pNode != NULL)
    {
        if (pNode->palloc->mfree(p))
            return;
        pNode = pNode->next;
    }

    PRINFO("Invalid pointer(%p) for Allocators::mfree. Memory free failed.\n", p);
    return;
}

bool AllocatorsManager::addAllocator(Allocator* pAllocator)
{
    FUNC_ENTER();

    if (pAllocator == NULL)
    {
        PRINFO("NULL pointer for AllocatorsManager::addAllocator.\n");
        return false;
    }

    std::unique_lock<std::mutex> lck(mtx);
    AllocNode *pNew = (AllocNode*)std::malloc(sizeof(AllocNode));
    Allocator* pAlloc = (Allocator*)std::malloc(sizeof(Allocator));
    *pAlloc = *pAllocator;  // Get allocator memory from pAllocator
    if (!pNew || !pAlloc)
    {
        PRINFO("malloc error for AllocatorsManager::addAllocator.\n");
        return false;
    }
    pNew->palloc = pAlloc;

    if (head == NULL)
    {
        head = pNew;
        pNew->next = NULL;
    }
    else
    {
        AllocNode *pNode = head;
        AllocNode *pTmp = NULL;

        while(pNode != NULL)
        {
            if (pNode->palloc->getSize() > pAllocator->getSize())
            {
                if (pTmp == NULL)
                {
                    head = pNew;
                    pNew->next = pNode;
                }
                pTmp->next = pNew;
                pNew->next = pNode;
                return true;
            }
            pTmp = pNode;
            pNode = pNode->next;
        }

        pTmp->next = pNew;
        pNew->next = NULL;
    }
    return true;
}

long AllocatorsManager::totalSize() const
{
    FUNC_ENTER();

    long total = 0;
    AllocNode *pNode = head;
    while(pNode != NULL)
    {
        total += pNode->palloc->getNum() * pNode->palloc->getSize();
        pNode = pNode->next;
    }

    return total;
}

long AllocatorsManager::totalUsed() const
{
    FUNC_ENTER();
    long totaluse = 0;
    AllocNode *pNode = head;
    while(pNode != NULL)
    {
        totaluse += pNode->palloc->getUsedNum() * pNode->palloc->getSize();
        pNode = pNode->next;
    }

    return totaluse;
}


long AllocatorsManager::totalUsedNum() const
{
    FUNC_ENTER();
    long totalusenum = 0;
    AllocNode *pNode = head;
    while(pNode != NULL)
    {
        totalusenum += pNode->palloc->getUsedNum();
        pNode = pNode->next;
    }

    return totalusenum;
}

void AllocatorsManager::print() const
{
    FUNC_ENTER();
    int i = 0;
    AllocNode *pNode = head;
    while(pNode != NULL)
    {
        PRINFO("Allocators[%u] ========================================\n", i++);
        pNode->palloc->print();
        pNode = pNode->next;
    }
    long tsize = totalSize();
    long tuse = totalUsed();
    PRINFO("Total:%luB, already used:%luB (%lu\%).\n\n", tsize, tuse, (tuse*100)/tsize);
}



void* operator new(size_t size)
{
    if (size == 0)
        return NULL;
     AllocatorsManager& allocator = AllocatorsManager::instance();
     return allocator.malloc(size);
}
void operator delete(void* p) noexcept
{
    if (!p)
        return;
    AllocatorsManager& allocator = AllocatorsManager::instance();
    allocator.mfree(p);
    return;
}

void* operator new[](size_t size)
{
    if (size == 0)
        return NULL;
     AllocatorsManager& allocator = AllocatorsManager::instance();
     return allocator.malloc(size);
}
void operator delete[](void* p) noexcept
{
    if (!p)
        return;
    AllocatorsManager& allocator = AllocatorsManager::instance();
    allocator.mfree(p);
    return;
}

