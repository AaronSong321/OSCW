//
// Created by Dimanche on 07/02/2021.
//

#ifndef CORE_PAIR_H
#define CORE_PAIR_H
#include "TypeTraits.h"


namespace Commons {
    template <class T1, class T2>
    struct Pair {
    public:
        const T1 t1;
        const T2 t2;
        Pair(T1 a1, T2 a2): t1(a1), t2(a2) {}

        friend bool operator==(Pair<T1, T2> a, Pair<T1, T2> b) {
            return a.t1==a.t2 && b.t1==b.t2;
        }
    };
}

#endif //CORE_PAIR_H
