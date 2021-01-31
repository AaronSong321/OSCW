//
//  Test.cpp
//  Ringbell
//
//  Created by Dimanche on 30/01/2021.
//

#include "../Core/Test.h"
#include "../Core/PriorityQueue.h"
#include "../Core/IntRange.h"

using namespace Commons;
using namespace Commons::Collections;

// void* __Unwind_Resume = 0;
// void* _Unwind_Resume = 0;
// void* ___Unwind_Resume = 0;
// void* __gxx_personality_v0 = 0;

void Test1(){
    auto heap = MakeShared<FibonacciHeap<int>>(GetDefaultComparator<int>());
    Until<int>(1, 500).ForEach([heap](int a)->void{
        heap->Add(a);
    });
    Until<int>(1, 500).ForEach([heap](int)->void{
        heap->ExtractMin();
    })
    auto t = MakeShared<int>(3);
}
