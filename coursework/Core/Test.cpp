//
//  Test.cpp
//  Ringbell
//
//  Created by Dimanche on 30/01/2021.
//

#include <stdio.h>
#include "Test.h"
#include "PriorityQueue.h"

using namespace Commons;

void* __Unwind_Resume = 0;
void* _Unwind_Resume = 0;
void* ___Unwind_Resume = 0;
void* __gxx_personality_v0 = 0;

void Test1(){
    // auto heap = MakeShared<FibonacciHeap<int>>(GetDefaultComparator<int>());
    // heap->Add(1);
    auto t = MakeShared<int>(3);
}
