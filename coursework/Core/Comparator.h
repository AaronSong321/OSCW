//
// Created by Dimanche on 30/01/2021.
//

#ifndef CPP_COMPARATOR_H
#define CPP_COMPARATOR_H

#include "Func.h"
#include "Concepts.h"
#include "TypeTraits.h"

namespace Commons{
    /**
     * This interface indicates that this type can be compared to type T
     * @tparam T
     */
    template <class T>
    class IComparable{
    public:
        virtual int CompareTo(T other) const = 0;
    };

    /**
     * This interface indicates that this is a function that compare two objects (denoted as a, b) of type T
     * return value rt > 0 indicates a > b
     * rt == 0 indicates a == b
     * rt < 0 indicates a < b
     * @tparam T
     */
    template <class T>
    class IComparator: public Fun<int, const T&, const T&> {
    public:
        IComparator(Function(T, const T&, const T&) f): Fun<int, const T&, const T&>(f){}
    };

    template <class T> class A{};
    template <class T> class B: public A<T>{};

    template <class T>
    IComparator<T> GetDefaultComparator()
    #if ENABLECONCEPT
    requires (CharTraits<T>::Value||IsIntegral<T>::Value)
    #endif
    {
        auto lam = [](const T& lhs, const T& rhs) -> int {
            return lhs - rhs;
        };
        int (*conv2)(const T&, const T&) = lam;
        return IComparator<T>(conv2);
    }

}
#endif //CPP_COMPARATOR_H
