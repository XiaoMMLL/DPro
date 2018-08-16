#include <iostream>
#include "allocator_test.h"

using namespace std;

int main()
{
    TestMemAlloc test;
    test.run();
    test.report();

    return 0;
}

