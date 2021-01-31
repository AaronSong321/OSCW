#pragma once
#include "Forward.h"

namespace Commons {
    template<class T, T v>
    struct IntegralConstant {
        static constexpr T Value = v;
        typedef T ValueType;
        typedef IntegralConstant<T, v> Type;
        constexpr operator T() const noexcept { return v; }
        constexpr T operator()() const noexcept { return v; }
    };
    struct TrueType : public IntegralConstant<bool, true> {};
    struct FalseType : public IntegralConstant<bool, false> {};

    template <class T> struct TypeIdentity { using Type = T; };

    namespace _impl {
        template <class T>
        auto TryAddRValueReference(int)->TypeIdentity<T&&>;
        template <class T>
        auto TryAddRValueReference(...)->TypeIdentity<T>;
    }
    template <class T> struct AddRvalueReference : decltype(_impl::TryAddRValueReference<T>(0)){};
    template <class T> using AddRvalueReferenceType = typename AddRvalueReference<T>::Type;
    template <class T>
    typename AddRvalueReference<T>::Type DeclVal() noexcept;

    template <class T>
    struct CharTraits : public FalseType {};
    template <>
    struct CharTraits<char> : public TrueType {};
    template <>
    struct CharTraits<wchar_t> : public TrueType {};
    //template <> struct CharTraits<char8_t>: public TrueType{};
    template <> struct CharTraits<char16_t>: public TrueType{};
    template <> struct CharTraits<char32_t>: public TrueType{};


    template <class T1, class T2> struct IsSame : FalseType {};
    template <class T> struct IsSame<T, T> :TrueType {};

    template <class T> struct IsVoid : IsSame<void, typename RemoveCV<T>::Type> {};

    namespace _impl
    {
        template<class T>
        auto TestReturnable(int) -> decltype(void(static_cast<T(*)()>(0)), TrueType { });
        template<class>
        auto TestReturnable(...) -> FalseType;
        template<class From, class To>
        auto TestImplicitlyConvertible(int) -> decltype(void(DeclVal<void (&)(To)>()(DeclVal<From>())), TrueType { });
        template<class, class>
        auto TestImplicitlyConvertible(...) -> FalseType;
    }
    template <class From, class To>
    struct IsConvertible : IntegralConstant<bool,
            (decltype(_impl::TestReturnable<To>(0))::Value&&
             decltype(_impl::TestImplicitlyConvertible<From, To>(0))::Value ||
             (IsVoid<From>::Value && IsVoid<To>::Value))
    > {};

    template <class T> struct IsIntegral: public IntegralConstant<bool, CharTraits<T>::Value>{};
    template <> struct IsIntegral<int>: public TrueType{};
    template <> struct IsIntegral<long long>: public TrueType{};
    template <> struct IsIntegral<long>: public TrueType{};
    template <> struct IsIntegral<short>: public TrueType{};

    /**
     * Used to mark a type as ValueType
     */
    template <class T> struct IsValueType: public FalseType{};
#define DeclValueType(T) template <> struct IsValueType<T>: public TrueType {};

    template <class T>
    struct IsUnion: public FalseType{};

    namespace _impl{
        template <class T>
        IntegralConstant<bool, !IsUnion<T>::Value> _Test_IsClass_Helper(int T::*);
        template <class>
        FalseType _Test_IsClass_Helper(...);
    }
    template <class T>
    struct IsClass:decltype(_impl::_Test_IsClass_Helper<T>((void*)0)) {};

    namespace _impl{
        template <class B>
        TrueType _TestPrePtrConvertible(const volatile B*);
        template <class B>
        FalseType _TestPrePtrConvertible(const volatile void*);
        template <class, class>
        auto _TestPreIsBaseOf(...)->TrueType;
        template <class Base, class Derived>
        auto _TestPreIsBaseOf(int)-> decltype(_TestPrePtrConvertible<Base>(static_cast<Derived*>(0)));
    }
    template <class Base, class Derived>
    struct IsBaseOf: IntegralConstant<bool, IsClass<Base>::Value && IsClass<Derived>::Value && decltype(_impl::_TestPreIsBaseOf<Base, Derived>(0))::Value>{};
}
