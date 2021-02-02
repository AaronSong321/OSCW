/*
 * FIFO Scheduling Algorithm
 * SKELETON IMPLEMENTATION -- TO BE FILLED IN FOR TASK (2)
 */

/*
 * STUDENT NUMBER: s
 */
#include <infos/kernel/sched.h>
#include <infos/kernel/thread.h>
#include <infos/kernel/log.h>
#include <infos/util/list.h>
#include <infos/util/lock.h>

using namespace infos::kernel;
using namespace infos::util;


namespace Commons {
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

    /**
     * Used to mark a type as ValueType
     */
    template <class T> struct IsValueType: public FalseType{};
#define DeclValueType(T) template <> struct IsValueType<T>: public TrueType {};

    template <class T>
    struct IsUnion: public FalseType{};

    namespace __impl{
        template <class T>
        IntegralConstant<bool, !IsUnion<T>::Value> _Test_IsClass_Helper(int T::*);
        template <class>
        FalseType _Test_IsClass_Helper(...);
    }
    template <class T>
    struct IsClass:decltype(__impl::_Test_IsClass_Helper<T>((void*)0)) {};

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
    struct IsBaseOf: IntegralConstant<bool, IsClass<Base>::Value && IsClass<Derived>::Value && decltype(__impl::_TestPreIsBaseOf<Base, Derived>(0))::Value>{};
    
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
            static auto Get(T&& t) -> decltype(t.Get());
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

}
namespace Commons{
    template <class T> struct SharedPointer;
    template <class T> struct WeakPointer;

    template<class T,class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs);

    template <class T>
    struct SharedPointer{
    private:
        T* _data;
        mutable int* _count;
        SharedPointer(T* rawPointer, int* counterPointer): _data(rawPointer), _count(counterPointer){
            ++*_count;
        }
        template <class U>
        friend struct WeakPointer;
        template <class U>
        friend struct SharedPointer;

    private:
        void Hold() const {
            ++*_count;
        }
        void Release() const noexcept{
            if (!_count)
                return;
            if (!--*_count){
                delete _data;
                delete _count;
            }
        }

    public:
        using ElementType = T;
        using WeakType = WeakPointer<T>;
        explicit SharedPointer(T* rawPointer): _data(rawPointer), _count(rawPointer?new int(1):0){
        }
        SharedPointer(const SharedPointer<T>& p): _data(p._data), _count(p._count) {
            ++*_count;
        }
        SharedPointer(SharedPointer<T>&& p) noexcept: _data(p._data), _count(p._count) {
            ++*_count;
        }
        SharedPointer(): _data(0), _count(0){
        }
        SharedPointer(decltype(nullptr)): _data(0), _count(0){
        }

        SharedPointer<T>& operator=(const SharedPointer<T>& other){
            other.Hold();
            Release();
            _data = other._data;
            _count = other._count;
            return *this;
        }
        SharedPointer<T>& operator=(decltype(nullptr)){
            Release();
            _data = 0;
            _count = 0;
            return *this;
        }
        SharedPointer<T>& operator=(SharedPointer<T>&& other) noexcept {
            other.Hold();
            Release();
            _data = other._data;
            _count = other._count;
            return *this;
        }

        bool operator==(const SharedPointer<T> other) const {
            return _data == other._data;
        }
        bool operator!=(const SharedPointer<T> other) const {
            return !(_data == other._data);
        }

        ~SharedPointer(){
            Release();
        }
        T* operator->() const {
            return _data;
        }
        T& operator*() const{
            return *_data;
        }
        operator bool() const {
            return _data != 0;
        }
        T* Get() const { return _data; }

    private:
        template <class TargetType>
        SharedPointer<TargetType> ConstructPointerAtType(TargetType* ptr) {
            if (ptr){
                Hold();
                return SharedPointer<TargetType>(ptr, _count);
            }
            else
                return SharedPointer<TargetType>(nullptr);
        }

    public:
        template <class TBase>
        SharedPointer<TBase> StaticCast();
        template <class TDerived>
        SharedPointer<TDerived> DynamicCast();
        template <class TBase>
        SharedPointer<TBase> ConstCast();
        template <class TDerived>
        SharedPointer<TDerived> ReinterpretCast();
    };

    template <class T>
    template <class TBase>
    SharedPointer<TBase> SharedPointer<T>::StaticCast() {
        auto data = static_cast<TBase*>(_data);
        return ConstructPointerAtType(data);
    }
    template <class T>
    template <class TBase>
    SharedPointer<TBase> SharedPointer<T>::DynamicCast() {
        auto data = dynamic_cast<TBase*>(_data);
        return ConstructPointerAtType(data);
    }
     template <class T>
     template <class TBase>
     SharedPointer<TBase> SharedPointer<T>::ConstCast() {
         auto data = const_cast<TBase*>(_data);
         return ConstructPointerAtType(data);
     }
     template <class T>
     template <class TBase>
     SharedPointer<TBase> SharedPointer<T>::ReinterpretCast() {
         auto data = reinterpret_cast<TBase*>(_data);
         return ConstructPointerAtType(data);
     }

    template<class T, class... ArgTypes>
    inline SharedPointer<T> MakeShared(ArgTypes&&... objects){
        auto p = new T(Forward<ArgTypes>(objects)...);
        return SharedPointer<T>(p);
    }

    template<class T, class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return lhs.Get() == rhs.Get();
    }
    template<class T, class U>
    bool operator!=(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return !(lhs == rhs);
    }

    template <class T>
    struct WeakPointer{
    private:
        T* _data;
        int* _count;

    public:
        using ElementType = T;
        explicit WeakPointer(const SharedPointer<T>& p): _data(p._data), _count(p._count){
        }
        WeakPointer(void*): _data(0), _count(0){ }
        SharedPointer<T> Pin() const {
            return SharedPointer<T>(_data, _count);
        }

        WeakPointer& operator=(const SharedPointer<T>& p){
            _data = p._data;
            _count = p._count;
            return *this;
        }
        
        WeakPointer& operator=(void*) {
            _data = 0;
            _count = 0;
            return *this;
        }
    };
}

namespace Commons {
	namespace __impl {
		template <class RetType, class... ArgTypes>
		RetType __SomeUglyFunction(ArgTypes...);
	}
}
#define Function(RetType, ...) decltype(&Commons::__impl::__SomeUglyFunction<RetType, __VA_ARGS__>)
#define FunctionVariable(RetType, VarName, ...) RetType (*VarName)(__VA_ARGS__)

namespace Commons{
    template <class Ret, class... Args>
    class Functor{
    };
    template <class Ret, class... Args>
    class Functor<Ret(Args...)>{
    public:
        virtual Ret Invoke(Args...) const = 0;
        virtual ~Functor() { }
    };

    /**
     * Generic functor type
     * The constructor is intentionally implicit
     * @tparam RetType
     * @tparam ArgTypes
     */
    template <class RetType, class... ArgTypes>
    struct Fun : public Functor<RetType(ArgTypes...)> {
    private:
        Function(RetType, ArgTypes...) _fpointer;

    public:
        Fun(Function(RetType, ArgTypes...) f): _fpointer(f){}
        RetType operator()(ArgTypes&&... args) const {
            return _fpointer(Forward<ArgTypes>(args)...);
        }
        // bool operator==(const Fun<RetType, ArgTypes...>&) const = default;
        bool operator==(const Fun<RetType, ArgTypes...>& other) const{
            return _fpointer == other._fpointer;
        }

        operator Function(RetType, ArgTypes...)(){
            return _fpointer;
        }
        virtual RetType Invoke(ArgTypes... args) const override {
            return _fpointer(Forward<ArgTypes>(args)...);
        }

        template <class RetType2, class... ArgTypes2>
        friend struct Fun;
        template <class RetType2, class... ArgTypes2>
        Fun<RetType2, ArgTypes2...> ReinterpretCast(){
            return Fun(reinterpret_cast<RetType2(*)(ArgTypes2...)>(_fpointer));
        }
    };

    template <class Y, class Ret, class... Args>
    class Capture: public Functor<Ret(Args...)> {
    private:
        const Y _capture;
    public:
        Capture(Y lambdaExpression): _capture(lambdaExpression){ }
        Ret operator()(Args... args){
            return _capture(args...);
        }
        virtual Ret Invoke(Args... args) const override {
            // return _capture(Forward<Args>(args)...);
            return _capture(args...);
        }
    };

    template <class Y, class Ret, class...Args>
    SharedPointer<Functor<Ret(Args...)>> LambdaToFunctor(Y lambdaExpression){
        return MakeShared<Capture<Y,Ret,Args...>>(lambdaExpression).template StaticCast<Functor<Ret(Args...)>>();
    }
    /** Convert a lambda to a Functor with the return type of the lambda deduced.
     * Argument types are still required.
     */
    template <class Y, class... Args>
    using Ret = InvokeResultType<Y, Args...>;
    template <class Y, class... Args>
    SharedPointer<Functor<Ret<Y, Args...>(Args...)>> LambdaToFunctor2(Y lambdaExpression) {
        return MakeShared<Capture<Y, typename InvokeResult<Y, Args...>::Type, Args...>>(lambdaExpression).template StaticCast<Functor<Ret<Y, Args...>(Args...)>>();
    }
}

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
namespace Commons::Collections{
    template <class T>
    class IEnumerator{
    public:
        typedef SharedPointer<T> EnumeratorType;
        virtual EnumeratorType Get() const = 0;
        virtual bool MoveNext() = 0;
        virtual ~IEnumerator() {}

        template <class... ArgTypes>
        static EnumeratorType MakeEnumerator(ArgTypes&&... args){
            return MakeShared<T>(Forward<ArgTypes>(args)...);
        }
        SharedPointer<T> operator->() const { return Get(); }
        T& operator*() const { return *Get(); }
    };

    #if ENABLECONCEPT
    /**
     * If define this, every template IEnumerable generator needs to provide a version for ValueType
     * @tparam T
     */
    template <ValueType T>
    class IEnumerator<T>{
    public:
        typedef T EnumeratorType;
        virtual EnumeratorType Get() const = 0;
        virtual bool MoveNext() = 0;
        virtual ~IEnumerator() {}
        
        template <class... ArgTypes>
        static inline EnumeratorType MakeEnumerator(ArgTypes&&... args){
            return T(Forward<ArgTypes>(args)...);
        }
    };
    #define IValueEnumerator IEnumerator
    // #else
    // template <class T>
    // class IValueEnumerator{
    // public:
    //     typedef T EnumeratorType;
    //     virtual EnumeratorType Get() const = 0;
    //     virtual bool MoveNext() = 0;
    //     template <class... ArgTypes>
    //     static inline EnumeratorType MakeEnumerator(ArgTypes&&... args){
    //         return T(Forward<ArgTypes>(args)...);
    //     }
    // };
    #endif

    template <class T>
    class IEnumerable;

    namespace __impl{
        template <class T>
        class AnonymousEnumerable: public IEnumerable<T>{
        private:
            const SharedPointer<IEnumerator<T>> _source;
        public:
            AnonymousEnumerable(SharedPointer<IEnumerator<T>> source): _source(source){}
            virtual SharedPointer<IEnumerator<T>> GetEnumerator() const override {
                return _source;
            }
        };
    }

    template <class T>
    SharedPointer<IEnumerable<T>> EnumeratorToEnumerable(SharedPointer<IEnumerator<T>> enumerator){
        auto t = MakeShared<__impl::AnonymousEnumerable<T>>(enumerator).template StaticCast<IEnumerable<T>>();
        return t;
    }
    template <class T, class ActualEnumeratorType>
    SharedPointer<IEnumerable<T>> EnumeratorToEnumerable2(SharedPointer<ActualEnumeratorType> enumerator) {
        auto t = MakeShared<__impl::AnonymousEnumerable<T>>(enumerator.template StaticCast<IEnumerator<T>>()).template StaticCast<IEnumerable<T>>();
    }

    namespace __impl{
        template <class T, class U>
        class _IEnumerable_Transform_IEnumerator;
        template <class T>
        class _IEnumerable_Filter_IEnumerator;
//        template <class T>
//        class
    }

    template <class T>
    class IEnumerable{
    private:

    public:
        virtual SharedPointer<IEnumerator<T>> GetEnumerator() const = 0;
        virtual ~IEnumerable() { }

        void ForEach(SharedPointer<Functor<void(SharedPointer<T>)>> func){
            auto iterator = GetEnumerator();
            while (iterator->MoveNext()){
                func->Invoke(iterator->Get());
            }
        }
        void ModifyEach(SharedPointer<Functor<void(SharedPointer<T>)>> func){
            auto iterator = GetEnumerator();
            while (iterator->MoveNext()){
                func->Invoke(iterator->Get());
            }
        }

        template <class U>
        SharedPointer<IEnumerable<U>> Map(U (*trans)(const T&)){
            auto ptr = new __impl::_IEnumerable_Transform_IEnumerator(*this, trans);
            auto next = SharedPointer(ptr);
            return next.template StaticCast<IEnumerable<U>>();
        }
        SharedPointer<IEnumerable<T>> Filter(FunctionVariable(bool, filter, const T&)){
            auto ptr = new __impl::_IEnumerable_Filter_IEnumerator(*this, filter);
            auto next = SharedPointer(ptr);
            return next.template StaticCast<IEnumerable<T>>();
        }
    };

    namespace __impl{
        template <class T, class U>
        class _IEnumerable_Transform_IEnumerator: public IEnumerator<U>{
        private:
            IEnumerator<T> _source;
            Function(U, T) _trans;
        public:
            _IEnumerable_Transform_IEnumerator(IEnumerable<T> source, Function(U, T) trans): _source(source.GetEnumerator()), _trans(trans){
            }

            virtual typename IEnumerator<U>::EnumeratorType Get() const override {
                return MakeEnumerator(trans(*_source));
            }
            virtual bool MoveNext() override {
                return _source.MoveNext();
            }
        };
        
        template <class T>
        class _IEnumerable_Filter_IEnumerator: public IEnumerator<T>{
        private:
            IEnumerator<T> _source;
            Function(bool, T) _filter;
        public:
            _IEnumerable_Filter_IEnumerator(IEnumerable<T> source, Function(bool, T) filter): _source(source.GetEnumerator()), _filter(filter){}
            
            virtual bool MoveNext() override {
                while (_source.MoveNext()) {
                    if (_filter(_source.Get()))
                        return true;
                }
                return false;
            }
            virtual SharedPointer<T> Get() override
            {
                return _source.Get();
            }
        };
    }

    template <class T>
    class IInputCollection{
    public:
        virtual int GetCount() const = 0;
        virtual void Add(T elem) = 0;
        virtual void Clear() = 0;
        virtual ~IInputCollection() { }
    };

    template <class T>
    class IOutputCollection: virtual public IEnumerable<T>{
    public:
        // using IEnumerable<T>::GetEnumerator;
        virtual bool Contains(T elem) const = 0;
        virtual void Remove(T elem) = 0;
    };

    template <class T>
    class ICollection: public IInputCollection<T>, public IOutputCollection<T> {};
}

namespace Commons{
    using namespace Collections;
    #if ENABLECONCEPT
    template <Integral T>
    #else
    template <class T>
    #endif
    struct Range;

    namespace __impl{
        #if ENABLECONCEPT
        template <Integral T>
        #else
        template <class T>
        #endif
        class Range_Enumerator: public IEnumerator<T>{
        private:
            const T _start;
            const T _end;
            T _current;

        public:
            explicit Range_Enumerator(Range<T> r):_start(r.GetStart()), _end(r.GetEnd()), _current(r.GetStart() - (T)1){

            }
            virtual bool MoveNext() override {
                return ++_current != _end;
            }
            virtual SharedPointer<T> Get() const override {
                return MakeShared<T>(_current);
            }
        };
    }

    #if ENABLECONCEPT
    template <Integral T>
    #else
    template <class T>
    #endif
    struct Range: public IEnumerable<T> {
    private:
        const T _start;
        const T _end;
    public:
        constexpr Range(T start, T end): _start(start), _end(end){
        }
        T GetStart() const { return _start; }
        T GetEnd() const { return _end; }

        virtual SharedPointer<IEnumerator<T>> GetEnumerator() const override {
            auto e = new ::Commons::__impl::Range_Enumerator(*this);
            auto ptr = SharedPointer(e);
            return ptr.template StaticCast<IEnumerator<T>>();
        }

    };
    
    #if ENABLECONCEPT
    template <Integral T>
    #else
    template <class T>
    #endif
    Range<T> To(T start, T end){
        return Range(start, end);
    }
    #if ENABLECONCEPT
    template <Integral T>
    #else
    template <class T>
    #endif
    Range<T> Until(T start, T end){
        return Range(start, ++end);
    }
}

namespace Commons::Collections {
    template <class T>
    class List;

    template <class T>
    class ListNode {
    private:
        T _data;
        SharedPointer<ListNode<T>> _next;
        WeakPointer<ListNode<T>> _prev;
        friend class List<T>;

    public:
        T Data() const { return _data; }
        SharedPointer<ListNode<T>> Next() const { return _next; }
        ListNode(T data): _data(data), _next(nullptr), _prev(nullptr) {
        }
    };

    namespace __impl{
        template <class T>
        class ListIterator: public IEnumerator<T> {
            SharedPointer<ListNode<T>> _cur;
            const SharedPointer<ListNode<T>> _root;
            bool _startState;
        public:
            ListIterator(const List<T>* list): _cur(list->Root()), _root(list->Root()), _startState(true) {
            }
            virtual bool MoveNext() override {
                if (!_cur)
                    return false;
                if (_startState) {
                    _startState = false;
                    return true;
                }
                _cur = _cur->Next();
                return _cur != _root;
            }
            virtual SharedPointer<T> Get() const override {
                return MakeShared<T>(_cur->Data());
            }
        };
    }


    template <class T>
    class List: public ICollection<T> {
    private:
        SharedPointer<ListNode<T>> _root;
        int _count;

    public:
        List(): _root(nullptr), _count(0) {
        }

    private:
        void AddAfter(SharedPointer<ListNode<T>> newNode, SharedPointer<ListNode<T>> oldNode) {
            newNode->_next = oldNode->_next;
            newNode->_prev = oldNode;
            oldNode->_next->_prev = newNode;
            oldNode->_next = newNode;
            ++_count;
        }

        void RemoveNode(SharedPointer<ListNode<T>> node) {
            node->_next->_prev = node->_prev.Pin();
            node->_prev.Pin()->_next = node->_next;
            if (node == _root) {
                _root = _root->_next;
                if (_count == 1) {
                    _root = nullptr;
                }
            }
            --_count;
        }

    public:
        SharedPointer<ListNode<T>> Root() const { return _root; }

        SharedPointer<ListNode<T>> Insert(T elem, SharedPointer<ListNode<T>> node){
            if (!node)
                return SharedPointer<ListNode<T>>();
            auto newNode = MakeShared<ListNode<T>>(elem);
            AddAfter(newNode, node);
            return newNode;
        }
        SharedPointer<ListNode<T>> AddToHead(T elem) {
            auto newNode = MakeShared<ListNode<T>>(elem);
            if (_root) {
                AddAfter(newNode, _root->_prev.Pin());
            } else {
                newNode->_next = newNode;
                newNode->_prev = newNode;
            }
            _root = newNode;
            return newNode;
        }
        SharedPointer<ListNode<T>> AddToTail(T elem) {
            auto newNode = MakeShared<ListNode<T>>(elem);
            if (_root) {
                AddAfter(newNode, _root);
            } else {
                newNode->_next = newNode;
                newNode->_prev = newNode;
                _root = newNode;
            }
            return newNode;
        }

        virtual void Add(T elem) override {
            AddToTail(elem);
        }

        SharedPointer<ListNode<T>> Find(T elem, SharedPointer<IValueEqualityComparator<T>> equalityComparator) const {
            if (!equalityComparator || !_root)
                return nullptr;
            const auto end = _root->_next;
            auto node = _root;
            do {
                if (equalityComparator->Invoke(node->Data(), elem)) {
                    return node;
                }
                node = node->_next;
            } while (node != end);
            return nullptr;
        }

        virtual bool Contains(T elem) const override {
            const auto compare = GetDefaultValueEqualityComparator<T>();
            return Find(elem, compare);
        }

        void Remove(T elem, SharedPointer<IValueEqualityComparator<T>> equalityComparator) {
            if (!equalityComparator || !_root)
                return;
            const auto end = _root;
            auto node = _root;
            SharedPointer<ListNode<T>> deleteList[_count];
            int deleteIndex = 0;
            do {
                if (equalityComparator->Invoke(node->Data(), elem)) {
                    deleteList[deleteIndex++] = node;
                }
                node = node->_next;
            } while (node != end);
            deleteIndex = 0;
            while (deleteList[deleteIndex]) {
                RemoveNode(deleteList[deleteIndex++]);
            }
        }

        virtual void Remove(T elem) override {
            Remove(elem, GetDefaultValueEqualityComparator<T>());
        }

        virtual ~List() override {
            if (_root)
                _root->_next = nullptr;
        }

        virtual SharedPointer<IEnumerator<T>> GetEnumerator() const override {
            auto enumerator = MakeShared<__impl::ListIterator<T>>(this);
            return enumerator.template StaticCast<IEnumerator<T>>();
        }

        virtual int GetCount() const override {
            return _count;
        }

        virtual void Clear() override {
            if (_root) 
                _root->_next = nullptr;
            _root = nullptr;
            _count = 0;
        }

        SharedPointer<ListNode<T>> GetHead() const {
            return _root;
        }
        SharedPointer<ListNode<T>> GetTail() const {
            return _root ? _root->_prev.Pin() : nullptr;
        }
    };
}

static void ListTest(){
    Commons::Collections::List<int> a;
    for (int i=0;i<18;++i){
        syslog.messagef(LogLevel::INFO, "list %i", i);
        a.Add(i);
        syslog.messagef(LogLevel::INFO, "list %i", i);
    }
    auto enumerator = a.GetEnumerator();
    for (int i=15;i<20;++i) {
        a.Remove(i);
        syslog.messagef(LogLevel::INFO, "list %i", i);
    }
    while (enumerator->MoveNext()) {
        syslog.messagef(LogLevel::INFO, "list %i", *enumerator->Get());
    }
}
using namespace Commons;



/**
 * A FIFO scheduling algorithm
 */
class FIFOScheduler : public SchedulingAlgorithm
{
public:
    FIFOScheduler() {
        ListTest();
    }

	/**
	 * Returns the friendly name of the algorithm, for debugging and selection purposes.
	 */
	const char* name() const override { 
        return "fifo"; 
    }

	/**
	 * Called when a scheduling entity becomes eligible for running.
	 * @param entity
	 */
	void add_to_runqueue(SchedulingEntity& entity) override
	{
        UniqueIRQLock _l;
        runqueue.AddToTail(&entity);
	}

	/**
	 * Called when a scheduling entity is no longer eligible for running.
	 * @param entity
	 */
	void remove_from_runqueue(SchedulingEntity& entity) override
	{
        UniqueIRQLock _l;
        runqueue.Remove(&entity);
	}

	/**
	 * Called every time a scheduling event occurs, to cause the next eligible entity
	 * to be chosen.  The next eligible entity might actually be the same entity, if
	 * e.g. its timeslice has not expired, or the algorithm determines it's not time to change.
	 */
	SchedulingEntity *pick_next_entity() override
	{
        if (!runqueue.GetCount())
            return nullptr;
        return runqueue.GetHead()->Data();
	}

private:
	// A list containing the current runqueue.
	Commons::Collections::List<SchedulingEntity *> runqueue;

};

/* --- DO NOT CHANGE ANYTHING BELOW THIS LINE --- */

RegisterScheduler(FIFOScheduler);
