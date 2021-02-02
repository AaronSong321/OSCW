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
    using IComparator = Functor<int(const T&, const T&)>;

    template <class T>
    using IEqualityComparator = Functor<bool(const T&, const T&)>;
    template <class T>
    using IValueEqualityComparator = Functor<bool(const T, const T)>;

    template <class T>
    SharedPointer<IComparator<T>> GetDefaultComparator()
    #if ENABLECONCEPT
    requires (CharTraits<T>::Value||IsIntegral<T>::Value)
    #endif
    {
        auto lam = [](const T& lhs, const T& rhs) -> int {
            return lhs - rhs;
        };
        // auto p = LambdaToFunctor2<decltype(lam), const T&, const T&>(lam);
        auto p = LambdaToFunctor<decltype(lam),int,const T&,const T&>(lam);
        return p;
    }

    namespace __impl {
        template <class T>
        static bool _Equals(const T& lhs, const T& rhs) {
            return lhs == rhs;
        }        
        template <class T>
        static bool _ValueEquals(const T lhs, const T rhs){
            return lhs == rhs;
        }
    }
    template <class T>
    SharedPointer<IEqualityComparator<T>> GetDefaultEqualityComparator(){
        return MakeShared<Fun<bool, const T&, const T&>>(&__impl::_Equals<T>).template StaticCast<IEqualityComparator<T>>();
    }
    template <class T>
    SharedPointer<IValueEqualityComparator<T>> GetDefaultValueEqualityComparator(){
        return MakeShared<Fun<bool, const T, const T>>(&__impl::_ValueEquals<T>).template StaticCast<IValueEqualityComparator<T>>();
    }
}
#endif //CPP_COMPARATOR_H
