//
//  Test.cpp
//  Ringbell
//
//  Created by Dimanche on 30/01/2021.
//

#include "../Core/Test.h"
#include "../Core/PriorityQueue.h"
#include "../Core/IntRange.h"
#include "../Core/List.h"

using namespace Commons;
using namespace Commons::Collections;

// void* __Unwind_Resume = 0;
// void* _Unwind_Resume = 0;
// void* ___Unwind_Resume = 0;
// void* __gxx_personality_v0 = 0;

void Test1(){
    auto heap = MakeShared<FibonacciHeap<int>>(GetDefaultComparator<int>());
    auto _lam_add = [heap](SharedPointer<int> a)->void{
        heap->Add(*a);
    };
    Until<int>(1, 500).ForEach(LambdaToFunctor<decltype(_lam_add), void, SharedPointer<int>>(_lam_add));
    auto _lam_em = [heap](SharedPointer<int>)->void{
        heap->ExtractMin();
    };
        Until<int>(1, 500).ForEach(LambdaToFunctor<decltype(_lam_em), void, SharedPointer<int>>(_lam_em));
    auto t = MakeShared<int>(3);
    auto list = MakeShared<List<int>>();
    auto _lam_add2 = [list](SharedPointer<int> elem) {
        list->Add(*elem);
    };
    Until(1,500).ForEach(LambdaToFunctor<decltype(_lam_add2), void, SharedPointer<int>>(_lam_add2));
}
