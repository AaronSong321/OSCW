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

    namespace __impl {
        template <class T>
        auto TryAddRValueReference(int)->TypeIdentity<T&&>;
        template <class T>
        auto TryAddRValueReference(...)->TypeIdentity<T>;
    }
    template <class T> struct AddRvalueReference : decltype(__impl::TryAddRValueReference<T>(0)){};
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

    namespace __impl
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
            (decltype(__impl::TestReturnable<To>(0))::Value&&
             decltype(__impl::TestImplicitlyConvertible<From, To>(0))::Value ||
             (IsVoid<From>::Value && IsVoid<To>::Value))
    > {};


    template <class T> struct IsIntegral: public IntegralConstant<bool, CharTraits<T>::Value>{};
    template <> struct IsIntegral<int>: public TrueType{};
    template <> struct IsIntegral<long long>: public TrueType{};
    template <> struct IsIntegral<long>: public TrueType{};
    template <> struct IsIntegral<short>: public TrueType{};
    template <> struct IsIntegral<bool>: public TrueType {};
//    template <> struct IsIntegral<ptrdiff_t>: public TrueType{};
//    template <> struct IsIntegral<size_t>: public TrueType{};
//    template <> struct IsIntegral<max_align_t>: public TrueType{};
//    template <> struct IsIntegral<byte>: public TrueType{};
    template <> struct IsIntegral<unsigned int>: public TrueType{};
    template <> struct IsIntegral<unsigned long long>: public TrueType{};
    template <> struct IsIntegral<unsigned long>: public TrueType{};
    template <> struct IsIntegral<unsigned short>: public TrueType{};

    template <class T> struct IsFloat: public FalseType {};
    template <> struct IsFloat<float>: public TrueType {};
    template <> struct IsFloat<double>: public TrueType {};
    template <> struct IsFloat<long double>: public TrueType {};

    template <class T> struct IsArithmetic: public IntegralConstant<bool, IsIntegral<T>::Value && IsFloat<T>::Value> {};

    template <class T> struct IsFundamental: public IntegralConstant<bool, IsArithmetic<T>::Value> {};
    template <> struct IsFundamental<void>: public TrueType {};
    template <> struct IsFundamental<decltype(nullptr)>: public TrueType {};

    /**
     * Used to mark a type as ValueType
     */
    template <class T> struct IsValueType: public FalseType{};
#define DeclValueType(T) namespace Commons { template <> struct IsValueType<T>: public TrueType {}; }

    template <class T>
    struct IsUnion: public FalseType {};
    template <class T>
    struct IsEnum: public FalseType {};

    namespace __impl{
        template <class T>
        IntegralConstant<bool, !IsUnion<T>::Value> _Test_IsClass_Helper(int T::*);
        template <class>
        FalseType _Test_IsClass_Helper(...);
    }
    template <class T>
    struct IsClass: decltype(__impl::_Test_IsClass_Helper<T>((void*)0)) {};
    template <class T>
    struct IsClass2: IntegralConstant<bool, !IsUnion<T>::Value&&!IsEnum<T>::Value&&!IsFundamental<T>::Value> {};

    namespace __impl{
        template <class B>
        TrueType _TestPrePtrConvertible(const volatile B*);
        template <class B>
        FalseType _TestPrePtrConvertible(const volatile void*);
        template <class, class>
        auto _TestPreIsBaseOf(...)->TrueType;
        template <class Base, class Derived>
        auto _TestPreIsBaseOf(int)-> decltype(_TestPrePtrConvertible<Base>(static_cast<Derived*>(nullptr)));
    }
    template <class Base, class Derived>
    struct IsBaseOf: IntegralConstant<bool, IsClass2<Base>::Value && IsClass2<Derived>::Value && decltype(__impl::_TestPreIsBaseOf<Base, Derived>(0))::Value>{};
    template <class Base, class Derived>
    inline constexpr bool IsBaseOfValue = IsBaseOf<RemoveCVType<Base>, RemoveCVType<Derived>>::Value;
    
    template <bool b, class T = void>
    struct EnableIf {};
    template <class T>
    struct EnableIf<true, T> { typedef T Type; };
    template <bool b, class T = void>
    using EnableIfType = typename EnableIf<b, T>::Type;

    template <bool b, class T, class F>
    struct Conditional { typedef T Type; };
    template <class T, class F>
    struct Conditional<false, T, F> { typedef F Type; };
    template <bool b, class T, class F>
    using ConditionalType = typename Conditional<b, T, F>::Type;

    template <class T> struct IsArray: FalseType {};
    template <class T> struct IsArray<T[]>: TrueType {};
    template <class T, int n> struct IsArray<T[n]>: TrueType {};

    template <class T> struct RemoveExtent { typedef T Type; };
    template <class T> struct RemoveExtent<T[]> { typedef T Type; };
    template <class T, int n> struct RemoveExtent<T[n]> { typedef T Type; };

    template <class T> struct IsReference: FalseType {};
    template <class T> struct IsReference<T&>: TrueType {};
    template <class T> struct IsReference<T&&>: TrueType {};

    template <class T> struct IsConst: FalseType {};
    template <class T> struct IsConst<const T>: TrueType {};
    template <class T> struct IsConst<const volatile T>: TrueType {};

    template <class T> struct IsFunction: IntegralConstant<bool,
#ifdef __clang__
    __is_function(T)
#else
    !(IsReference<T>::Value || IsConst<const T>::Value)
#endif
    > {};

    namespace __impl {
        template <class T>
        auto TryAddPointer(int)->TypeIdentity<RemoveReferenceType<T>*>;
        template <class T>
        auto TryAddPointer(...)->TypeIdentity<T>;
    }
    template <class T>
    struct AddPointer: decltype(__impl::TryAddPointer<T>(0)) {};
    template <class T> using AddPointerType = typename AddPointer<T>::Type;

    template <class T>
    struct Decay {
    private:
        typedef RemoveReferenceType<T> U;
    public:
        typedef typename Conditional<
            IsArray<U>::Value,
            typename RemoveExtent<U>::Type*,
            typename Conditional<
                IsFunction<U>::Value,
                AddPointerType<U>,
                RemoveCVType<U>
            >::Type
        >::Type Type;
    };
    template <class T> using DecayType = typename Decay<T>::Type;


    namespace __impl{
        template <class T> struct IsReferenceWrapper: FalseType {};
        template <class T>
        struct InvokeImpl {
            template <class F, class... Args>
            static auto Call(F&& f, Args&&... args) -> decltype(Forward<F>(f)(Forward<Args>(args)...));
        };

        template <class B, class MT>
        struct InvokeImpl<MT B::*> {
            template <class T, class Td = DecayType<T>, class __A = typename EnableIf<IsBaseOf<B, Td>::Value>::Type>
            static auto Get(T&& t) -> T&&;
            template <class T, class Td = DecayType<T>, class __A = typename EnableIf<IsReferenceWrapper<Td>::Value>::Type>
            static auto Get(T&& t) -> decltype(t.Current());
            template <class T, class Td = DecayType<T>,
                class __A = typename EnableIf<!IsBaseOf<B, Td>::Value>::Type,
                class __B = typename EnableIf<!IsReferenceWrapper<Td>::Value>::Type
            > static auto Get(T&& t) -> decltype(*Forward<T>(t));

            template <class T, class... Args, class MT1, class __A = typename EnableIf<IsFunction<MT1>::Value>::Type>
            static auto Call(MT1 B::*pmf, T&& t, Args&&... args) -> decltype(InvokeImpl::Get(Forward<T>(t)).*pmf(Forward<Args>(args)...));
            template <class T>
            static auto Call(MT B::*pmd, T&& t) -> decltype(InvokeImpl::Get(Forward<T>(t)).*pmd);
        };

        template <class F, class... Args, class Fd = DecayType<F>>
        auto __Invoke(F&& f, Args&&... args) -> decltype(InvokeImpl<Fd>::Call(Forward<F>(f), Forward<Args>(args)...));

        template <class AlwaysVoid, class __A, class... __Args> struct InvokeResult {};
        template <class F, class... Args>
        struct InvokeResult<decltype(void(__Invoke(DeclVal<F>(), DeclVal<Args>()...))), F, Args...> {
            using Type = decltype(__Invoke(DeclVal<F>(), DeclVal<Args>()...));
        };
    }

    template <class > struct ResultOf;
    template <class F, class... ArgTypes> struct ResultOf<F(ArgTypes...)>: __impl::InvokeResult<void, F, ArgTypes...> {};
    template <class F, class... ArgTypes>
    struct InvokeResult: __impl::InvokeResult<void, F, ArgTypes...> {};
    template <class T> using ResultOfType = typename ResultOf<T>::Type;
    template <class F, class... ArgTypes> using InvokeResultType = typename InvokeResult<F, ArgTypes...>::Type;
};
