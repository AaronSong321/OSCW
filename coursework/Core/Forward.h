//
// Created by Dimanche on 30/01/2021.
//

#ifndef CPP_FORWARD_H
#define CPP_FORWARD_H

namespace Commons {
    template<class T> struct RemoveReference { typedef T Type; };
    template<class T> struct RemoveReference<T&> { typedef T Type; };
    template<class T> struct RemoveReference<const T&> { typedef T Type; };
    template<class T> struct RemoveReference<T&&> { typedef T Type; };
    template<class T> using RemoveReferenceType = typename RemoveReference<T>::Type;

    template <class T>
    constexpr RemoveReferenceType<T>&& Move(T&& arg) noexcept {
        return static_cast<RemoveReferenceType<T>&&>(arg);
    }
    template <class T>
    constexpr void Swap(T& t1, T& t2) {
        T temp = Move(t1);
        t1 = Move(t2);
        t2 = Move(t1);
    }

    template<class T>
    constexpr inline T&& Forward(typename RemoveReference<T>::Type& obj) noexcept {
        return static_cast<T&&>(obj);
    }
    template<class T>
    constexpr inline T&& Forward(typename RemoveReference<T>::Type&& obj) noexcept {
        return static_cast<T&&>(obj);
    }

    template<class T> struct RemoveCV { typedef T Type; };
    template<class T> struct RemoveCV<const T> { typedef T Type; };
    template<class T> struct RemoveCV<volatile T> { typedef T Type; };
    template<class T> struct RemoveCV<const volatile T> { typedef T Type; };
    template<class T> using RemoveCVType = typename RemoveCV<T>::Type;
}
#endif //CPP_FOR
// WARD_H
