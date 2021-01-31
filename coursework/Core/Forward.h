//
// Created by Dimanche on 30/01/2021.
//

#ifndef CPP_FORWARD_H
#define CPP_FORWARD_H


template <class T> struct RemoveReference { typedef T Type; };
template <class T> struct RemoveReference<T&> { typedef T Type; };
template <class T> struct RemoveReference<T&&> { typedef T Type; };
template <class T> using RemoveReferenceType = typename RemoveReference<T>::Type;

template <class T>
constexpr inline T&& Forward(typename RemoveReference<T>::Type& obj) noexcept {
    return static_cast<T&&>(obj);
}
template <class T>
constexpr inline T&& Forward(typename RemoveReference<T>::Type&& obj) noexcept {
    return static_cast<T&&>(obj);
}

template <class T> struct RemoveCV { typedef T Type; };
template <class T> struct RemoveCV<const T> { typedef T Type; };
template <class T> struct RemoveCV<volatile T> { typedef T Type; };
template <class T> struct RemoveCV<const volatile T> { typedef T Type; };
template <class T> using RemoveCVType = typename RemoveCV<T>::Type;

#endif //CPP_FORWARD_H
