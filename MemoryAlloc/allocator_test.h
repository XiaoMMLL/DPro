#ifndef ALLOCTOR_TEST_H
#define ALLOCTOR_TEST_H
#include "allocator.h"
#include "suite.h"

using namespace TestSuite;

class TestMemAlloc : public TestSuite::Test
{
public:
    TestMemAlloc(){}
    virtual void run()
    {
        init();
        test1();
        test2();
        test3();
    }

    void init()
    {
        // AllocatorsManager initial
        AllocatorsManager& allocManager = AllocatorsManager::instance();
        Allocator alloctor1(16, 1);
        Allocator alloctor2(8, 2);
        Allocator alloctor3(4, 4);
        Allocator alloctor4(2, 8);
        Allocator alloctor5(2, 16);
        Allocator alloctor6(1, 32);
        Allocator alloctor7(1, 64);
        Allocator alloctor8(1, 128);

        allocManager.addAllocator(&alloctor1);
        allocManager.addAllocator(&alloctor2);
        allocManager.addAllocator(&alloctor3);
        allocManager.addAllocator(&alloctor4);
        allocManager.addAllocator(&alloctor5);
        allocManager.addAllocator(&alloctor6);
        allocManager.addAllocator(&alloctor7);
        allocManager.addAllocator(&alloctor8);

        test_(allocManager.totalSize() == 320);
        test_(allocManager.totalUsed() == 0);
        test_(allocManager.totalUsedNum() == 0);
    }

    void test1()
    {
        // Allocator test
        Allocator alloctor(2, 4);
        test_(alloctor.getUsedNum() == 0);
        int* pi = (int*)alloctor.malloc(sizeof(int));
        test_(alloctor.getUsedNum() == 1);
        char* pc = (char*)alloctor.malloc(sizeof(char));
        test_(alloctor.getUsedNum() == 2);
        alloctor.mfree(pc);
        test_(alloctor.getUsedNum() == 1);
        double *pd = (double*)alloctor.malloc(sizeof(double));
        test_(alloctor.getUsedNum() == 1);
        alloctor.mfree(pd);
        test_(alloctor.getUsedNum() == 1);
        alloctor.mfree(pi);
        test_(alloctor.getUsedNum() == 0);
    }

    void test2()
    {
        // AllocatorsManager test 1
        AllocatorsManager& allocManager = AllocatorsManager::instance();
        char* p1 = new char[33];
        test_(allocManager.totalUsed() == 64);
        char* p2 = new char[35];
        test_(allocManager.totalUsed() == 192);
        test_(allocManager.totalUsedNum() == 2);
        delete[] p1;
        test_(allocManager.totalUsed() == 128);
        delete[] p2;
        test_(allocManager.totalUsed() == 0);
        test_(allocManager.totalUsedNum() == 0);
        char* p[20];
        for (int i = 0; i < 20; i++)
        {
            p[i] = new char;
        }
        test_(allocManager.totalUsed() == 24);
        test_(allocManager.totalUsedNum() == 20);

        p1 = new char[8];
        p2 = new char[8];
        test_(allocManager.totalUsed() == 40);
        test_(allocManager.totalUsedNum() == 22);

        char* q[10];
        q[0] = new char[14];
        test_(allocManager.totalUsed() == 56);
        test_(allocManager.totalUsedNum() == 23);
        q[1] = new char[14];
        test_(allocManager.totalUsed() == 72);
        test_(allocManager.totalUsedNum() == 24);
        q[2] = new char[14];
        test_(allocManager.totalUsed() == 104);
        test_(allocManager.totalUsedNum() == 25);
        q[3] = new char[14];
        test_(allocManager.totalUsed() == 168);
        test_(allocManager.totalUsedNum() == 26);
        q[4] = new char[14];
        test_(allocManager.totalUsed() == 296);
        test_(allocManager.totalUsedNum() == 27);
        q[6] = new char[14];        // allocate fail
        test_(allocManager.totalUsed() == 296);
        test_(allocManager.totalUsedNum() == 27);
        delete[] q[6];
        test_(allocManager.totalUsed() == 296);
        test_(allocManager.totalUsedNum() == 27);
        q[6] = new char[2];
        q[7] = new char[2];
        q[8] = new char[2];
        q[9] = new char[2];
        test_(allocManager.totalUsed() == 304);
        test_(allocManager.totalUsedNum() == 31);


        char* s[4];
        s[0] = new char[4];
        s[1] = new char[4];
        s[2] = new char[4];
        s[3] = new char[4];
        test_(allocManager.totalUsed() == 320);
        test_(allocManager.totalUsedNum() == 35);

        char* r;
        r = new char;
        test_(allocManager.totalUsed() == 320);
        test_(allocManager.totalUsedNum() == 35);
        delete r;
        r = new char[2];
        test_(allocManager.totalUsed() == 320);
        test_(allocManager.totalUsedNum() == 35);
        delete[] r;
        r = new char[4];
        test_(allocManager.totalUsed() == 320);
        test_(allocManager.totalUsedNum() == 35);
        delete[] r;
        r = new char[16];
        test_(allocManager.totalUsed() == 320);
        test_(allocManager.totalUsedNum() == 35);
        delete[] r;
        r = new char[33];
        test_(allocManager.totalUsed() == 320);
        test_(allocManager.totalUsedNum() == 35);
        delete[] r;

        for (int i = 0; i < 20; i++)
        {
            delete p[i];
        }
        test_(allocManager.totalUsed() == 296);
        test_(allocManager.totalUsedNum() == 15);

        delete[] q[0];
        delete[] q[1];
        test_(allocManager.totalUsed() == 264);
        test_(allocManager.totalUsedNum() == 13);
        delete[] q[4];
        test_(allocManager.totalUsed() == 136);
        test_(allocManager.totalUsedNum() == 12);
        delete[] q[2];
        delete[] q[3];
        test_(allocManager.totalUsed() == 40);
        test_(allocManager.totalUsedNum() == 10);
        delete[] q[6];
        delete[] q[7];
        delete[] q[8];
        delete[] q[9];
        test_(allocManager.totalUsed() == 32);
        test_(allocManager.totalUsedNum() == 6);

        delete[] s[0];
        delete[] s[1];
        delete[] s[2];
        delete[] s[3];
        test_(allocManager.totalUsed() == 16);
        test_(allocManager.totalUsedNum() == 2);

        delete[] p1;
        test_(allocManager.totalUsed() == 8);
        test_(allocManager.totalUsedNum() == 1);
        delete[] p2;
        test_(allocManager.totalUsed() == 0);
        test_(allocManager.totalUsedNum() == 0);
    }

    void test3()
    {
        // AllocatorsManager test 2
        AllocatorsManager& allocManager = AllocatorsManager::instance();

        double* p[10];
        for (int i = 0; i < 10; i++)
        {
            p[i] = new double;
        }
        test_(allocManager.totalUsed() == 272);
        test_(allocManager.totalUsedNum() == 7);
        *p[0] = 1000.345;
        *p[1] = *p[0];
        test_(*p[0] == *p[1]);
        delete p[0];
        delete p[1];
        test_(allocManager.totalUsed() == 256);
        test_(allocManager.totalUsedNum() == 5);

        delete p[2];
        delete p[3];
        test_(allocManager.totalUsed() == 224);
        test_(allocManager.totalUsedNum() == 3);

        delete p[4];
        test_(allocManager.totalUsed() == 192);
        test_(allocManager.totalUsedNum() == 2);

        delete p[5];
        test_(allocManager.totalUsed() == 128);
        test_(allocManager.totalUsedNum() == 1);

        delete p[6];
        test_(allocManager.totalUsed() == 0);
        test_(allocManager.totalUsedNum() == 0);

    }
};

#endif // ALLOCTOR_TEST_H
