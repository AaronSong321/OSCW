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
    auto _lam_add = [heap](int a)->void{
        heap->Add(a);
    };
    Until<int>(1, 500).ForEach(LambdaToFunctor<decltype(_lam_add), void, SharedPointer<int>>(_lam_add));
    auto _lam_em = [heap](int)->void{
        heap->ExtractMin();
    };
        Until<int>(1, 500).ForEach(LambdaToFunctor<decltype(_lam_em), void, SharedPointer<int>>(_lam_em));
    auto t = MakeShared<int>(3);
}
