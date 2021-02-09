


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
};


#define ThrowIf0(argName) if (!argName) throw #argName" is null!"
/**
 * Used when such exit doesn't really exist
 */
#define DummyThrow() //throw 0
#define ShallThrow(expression) DummyThrow()

#define OverloadComparisonOperatorsDeclaration1(typeName, typeArg1) \
template <class typeArg1> \
friend bool operator==(typeName<typeArg1> lhs, typeName<typeArg1> rhs);          \
template <class typeArg1> \
friend bool operator!=(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator>(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator>=(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator<(typeName<typeArg1> lhs, typeName<typeArg1> rhs);     \
template <class typeArg1> \
friend bool operator<=(typeName<typeArg1> lhs, typeName<typeArg1> rhs);

#define OverloadComparisonOperatorDefinition1(typeName, typeArg1) \
template <class typeArg1> \
bool operator==(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return !typeName<typeArg1>::CompareTo(lhs, rhs); } \
template <class typeArg1> \
bool operator!=(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs); } \
template <class typeArg1> \
bool operator>(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)>0; } \
template <class typeArg1> \
bool operator>=(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)>=0; } \
template <class typeArg1> \
bool operator<(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)<0; } \
template <class typeArg1> \
bool operator<=(typeName<typeArg1> lhs, typeName<typeArg1> rhs) { return typeName<typeArg1>::CompareTo(lhs, rhs)<=0; }

#define NotImplementedException() throw "function '" __PRETTY_FUNCTION__ "' not implemented yet."


namespace Commons{


    template <class T> struct SharedPointer;
    template <class T> struct WeakPointer;

    template<class T,class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs);

    template <class T>
    struct SharedPointer{
    private:
        T* _data;
        mutable int* _sharedCount;
        mutable int* _weakCount;
        SharedPointer(T* rawPointer, int* sharedPtr, int* weakPtr): _data(rawPointer), _sharedCount(sharedPtr), _weakCount(weakPtr){
            ++*_sharedCount;
        }
        template <class U>
        friend struct WeakPointer;
        template <class U>
        friend struct SharedPointer;

    private:
        void Hold() const {
            if (_sharedCount)
                ++*_sharedCount;
        }
        void Release() const noexcept{
            if (!_sharedCount)
                return;
            if (!--*_sharedCount){
                delete _data;
                --*_weakCount;
            }
        }

    public:
        using ElementType = T;
        using WeakType = WeakPointer<T>;
        explicit SharedPointer(T* rawPointer): _data(rawPointer), _sharedCount(rawPointer ? new int(1) : nullptr), _weakCount(rawPointer? new int(1): nullptr){
        }
        SharedPointer(const SharedPointer<T>& p): _data(p._data), _sharedCount(p._sharedCount), _weakCount(p._weakCount) {
            Hold();
        }
        SharedPointer(SharedPointer<T>&& p) noexcept: _data(p._data), _sharedCount(p._sharedCount), _weakCount(p._weakCount) {
            p._data = nullptr;
            p._sharedCount = nullptr;
            p._weakCount = nullptr;
        }
        SharedPointer(): _data(0), _sharedCount(nullptr), _weakCount(nullptr) {
        }
        SharedPointer(decltype(nullptr)): _data(0), _sharedCount(nullptr), _weakCount(nullptr){
        }

        SharedPointer<T>& operator=(const SharedPointer<T>& other){
            other.Hold();
            Release();
            _data = other._data;
            _sharedCount = other._sharedCount;
            _weakCount = other._weakCount;
            return *this;
        }
        SharedPointer<T>& operator=(SharedPointer<T>&& other) noexcept {
            other.Hold();
            Release();
            _data = other._data;
            _sharedCount = other._sharedCount;
            _weakCount = other._weakCount;
            return *this;
        }
        SharedPointer<T>& operator=(decltype(nullptr)){
            Release();
            _data = 0;
            _sharedCount = nullptr;
            _weakCount = nullptr;
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
        T* GetRawPointer() const { return _data; }


    private:
        template <class TargetType>
        SharedPointer<TargetType> ConstructPointerAtType(TargetType* ptr) {
            if (ptr){
                Hold();
                return SharedPointer<TargetType>(ptr, _sharedCount, _weakCount);
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

    template <class T>
    class ManagedObject;
    template<class T, class... ArgTypes>
    inline SharedPointer<T> MakeShared(ArgTypes&&... objects){
        auto p = new T(Forward<ArgTypes>(objects)...);
        SharedPointer<T> sp(p);
        if constexpr (IsBaseOf<ManagedObject<T>, T>::Value) {
            dynamic_cast<ManagedObject<T>*>(p)->Feed(sp);
        }
        return sp;
    }

    template<class T, class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return lhs.GetRawPointer() == rhs.GetRawPointer();
    }
    template<class T, class U>
    bool operator!=(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return !(lhs == rhs);
    }

    template <class T>
    struct WeakPointer{
    private:
        T* _data;
        int* _sharedCount;
        int* _weakCount;

        void Hold() {
            if (_weakCount)
                ++*_weakCount;
        }
        void Release() {
            if (_weakCount) {
                if (!--*_weakCount) {
                    delete _sharedCount;
                    delete _weakCount;
                }
            }
        }
    public:
        explicit WeakPointer(const SharedPointer<T>& p): _data(p._data), _sharedCount(p._sharedCount), _weakCount(p._weakCount){
            Hold();
        }
        WeakPointer(decltype(nullptr)): _data(nullptr), _sharedCount(nullptr), _weakCount(nullptr) { }

        SharedPointer<T> Pin() const {
            if (_sharedCount && *_sharedCount)
                return SharedPointer<T>(_data, _sharedCount, _weakCount);
            return nullptr;
        }

        WeakPointer& operator=(const SharedPointer<T>& p) noexcept {
            if (_data != p._data) {
                Release();
                _data = p._data;
                _sharedCount = p._sharedCount;
                _weakCount = p._weakCount;
                Hold();
            }
            return *this;
        }
        
        WeakPointer& operator=(decltype(nullptr)) noexcept {
            Release();
            _data = nullptr;
            _sharedCount = nullptr;
            _weakCount = nullptr;
            return *this;
        }
    };

    template <class T>
    class ManagedObject{
    private:
        WeakPointer<T> _this;
    public:
        ManagedObject(): _this(nullptr) { }

        void Feed(SharedPointer<T> a) {
            _this = a;
        }
        SharedPointer<T> SharedPointerToThis() const {
            return _this.Pin();
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

        Ret operator()(Args&&... args) const {
            return Invoke(args...);
        }
    };

    /**
     * Generic functor type
     * The constructor is intentionally implicit
     * @tparam RetType
     * @tparam ArgTypes
     */
    template <class RetType, class... ArgTypes>
    struct Fun {};
    template <class RetType, class... ArgTypes>
    struct Fun<RetType(ArgTypes...)>: public Functor<RetType(ArgTypes...)> {
    private:
        Function(RetType, ArgTypes...) _fpointer;

    public:
        Fun(Function(RetType, ArgTypes...) f): _fpointer(f){}

        bool operator==(const Fun<RetType, ArgTypes...>& other) const{
            return _fpointer == other._fpointer;
        }

        operator Function(RetType, ArgTypes...)(){
            return _fpointer;
        }
        RetType Invoke(ArgTypes... args) const override {
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
        Ret Invoke(Args... args) const override {
            return _capture(args...);
        }
    };

    template <class Y, class Ret, class...Args>
    SharedPointer<Functor<Ret(Args...)>> LambdaToFunctor(Y lambdaExpression){
        return MakeShared<Capture<Y,Ret,Args...>>(lambdaExpression).template StaticCast<Functor<Ret(Args...)>>();
    }
#define MacroDeclareLambdaFunctor0(name, capture, Ret, block) auto __lambda_##name = capture() -> Ret block; \
        auto name = MakeShared<Capture<decltype(__lambda_##name), Ret>>(__lambda_##name).template StaticCast<Functor<Ret()>>()
#define MacroDeclareLambdaFunctor1(name, capture, Arg1, Ret, block) auto __lambda_##name = capture(Arg1 arg1) -> Ret block; \
        auto name = MakeShared<Capture<decltype(__lambda_##name), Ret, Arg1>>(__lambda_##name).template StaticCast<Functor<Ret(Arg1)>>()
#define MacroDeclareLambdaFunctor2(name, capture, Arg1, Arg2, Ret, block) auto __lambda_##name = capture(Arg1 arg1, Arg2 arg2) -> Ret block; \
        auto name = MakeShared<Capture<decltype(__lambda_##name), Ret, Arg1, Arg2>>(__lambda_##name).template StaticCast<Functor<Ret(Arg1, Arg2)>>()
#define MacroDeclareLambdaFunctor3(name, capture, Arg1, Arg2, Arg3, Ret, block) auto __lambda_##name = capture(Arg1 arg1, Arg2 arg2, Arg3 arg3) -> Ret block; \
        auto name = MakeShared<Capture<decltype(__lambda_##name), Ret, Arg1, Arg2, Arg3>>(__lambda_##name).template StaticCast<Functor<Ret(Arg1, Arg2, Arg3)>>()


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


namespace Commons {
#if ENABLECONCEPT
	template <class T1, class T2>
	concept SameAs = IsSame<T1, T2>::Value && IsSame<T2, T1>::Value;
	
	template <class From, class To>
	concept ConvertibleTo = IsConvertible<From, To>::Value && requires(AddRvalueReferenceType<From>(&f)()){
	    static_cast<To>(f());
	};

	template<class T>
	concept Char = CharTraits<T>::Value;
	template <class T>
	concept Integral = IsIntegral<T>::Value;

    template <class T>
    concept ValueType = Char<T> || Integral<T> || IsValueType<T>::Value;
#endif
}


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


namespace Commons::Collections{
    template <class T>
    class IEnumerator{
    public:
        typedef SharedPointer<T> EnumeratorType;
        virtual EnumeratorType Get() const noexcept = 0;
        virtual bool MoveNext() noexcept = 0;
        virtual ~IEnumerator() noexcept {}

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
     * @tparam TKey
     */
    template <ValueType TKey>
    class IEnumerator<TKey>{
    public:
        typedef TKey EnumeratorType;
        virtual EnumeratorType Get() const = 0;
        virtual bool MoveNext() = 0;
        virtual ~IEnumerator() {}

        template <class... ArgTypes>
        static inline EnumeratorType MakeEnumerator(ArgTypes&&... args){
            return TKey(Forward<ArgTypes>(args)...);
        }
    };
    #define IValueEnumerator IEnumerator


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
            SharedPointer<IEnumerator<T>> GetEnumerator() const override {
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
        return t;
    }

    namespace __impl{
        template <class T, class U>
        class _IEnumerable_Transform_IEnumerator;
        template <class T>
        class _IEnumerable_Filter_IEnumerator;
        template <class T>
        class _IEnumerable_Skip_IEnumerator;
        template <class T>
        class _IEnumerable_Take_IEnumerator;

        template <class T>
        class _IEnumerableCppStyleForEachHelper;
        template <class T>
        class _IEnumerableCppStyleForEachHelper_Iterator;
        /**
         * This has to be a template struct although the template parameter is never used.
         * Because otherwise, its definition has to be implemented in a .cpp file which requires an additional complication unit
         * @tparam T: never used. Use int to save the trouble of multiple template specialisation.
         */
        template <class T>
        struct _IEnumerableCppStyleForEachHelper_EndIterator{};
    }

    template <class T>
    class IEnumerable {
    private:

    public:
        typedef T ElementType;
        virtual int Count() const {
            auto h = GetEnumerator();
            int f = 0;
            while (h->MoveNext())
                ++f;
            return f;
        }
        virtual SharedPointer<IEnumerator<T>> GetEnumerator() const = 0;
        virtual ~IEnumerable() { }

        __impl::_IEnumerableCppStyleForEachHelper_Iterator<T> begin() const {
            return __impl::_IEnumerableCppStyleForEachHelper_Iterator<T>(this);
        }
        __impl::_IEnumerableCppStyleForEachHelper_EndIterator<T> end() const {
            __impl::_IEnumerableCppStyleForEachHelper_EndIterator<T> a;
            return a;
        }

        void ForEach(SharedPointer<Functor<void(SharedPointer<T>)>> func) const {
            auto iterator = GetEnumerator();
            while (iterator->MoveNext()){
                func->Invoke(iterator->Get());
            }
        }
        template <class U>
        SharedPointer<IEnumerable<U>> Map(SharedPointer<Functor<U(SharedPointer<T>)>> func) const {
            auto enumerator = MakeShared<__impl::_IEnumerable_Transform_IEnumerator<T, U>>(this, func);
            auto ptr = EnumeratorToEnumerable2<U, __impl::_IEnumerable_Transform_IEnumerator<T, U>>(enumerator);
            return ptr.template StaticCast<IEnumerable<U>>();
        }
        SharedPointer<IEnumerable<T>> Filter(SharedPointer<Functor<bool(SharedPointer<T>)>> func) const {
            auto ptr = EnumeratorToEnumerable(MakeShared<__impl::_IEnumerable_Filter_IEnumerator<T>>(this, func));
            return ptr.template StaticCast<IEnumerable<T>>();
        }
        SharedPointer<IEnumerable<T>> Skip(int number) const {
            if (Count() < number)
                DummyThrow();
            auto ptr = EnumeratorToEnumerable(MakeShared<__impl::_IEnumerable_Skip_IEnumerator<T>>(this, number));
            return ptr.template StaticCast<IEnumerable<T>>();
        }
        SharedPointer<IEnumerable<T>> Take(int number) const {
            if (Count() < number)
                DummyThrow();
            auto ptr = EnumeratorToEnumerable(MakeShared<__impl::_IEnumerable_Skip_IEnumerator<T>>(this, number));
            return ptr.template StaticCast<IEnumerable<T>>();
        }
    };


    namespace __impl{
        template <class T, class U>
        class _IEnumerable_Transform_IEnumerator: public IEnumerator<U>{
        private:
            SharedPointer<IEnumerator<T>> _source;
            SharedPointer<Functor<U(SharedPointer<T>)>> _trans;
        public:
            _IEnumerable_Transform_IEnumerator(const IEnumerable<T>* source, SharedPointer<Functor<U(SharedPointer<T>)>> trans): _source(source->GetEnumerator()), _trans(trans){
            }

            typename IEnumerator<U>::EnumeratorType Get() const noexcept override {
                return MakeShared<U>(_trans->Invoke(_source->Get()));
            }
            bool MoveNext() noexcept override {
                return _source->MoveNext();
            }
        };
        
        template <class T>
        class _IEnumerable_Filter_IEnumerator: public IEnumerator<T>{
        private:
            SharedPointer<IEnumerator<T>> _source;
            SharedPointer<Functor<bool(SharedPointer<T>)>> _filter;
        public:
            _IEnumerable_Filter_IEnumerator(const IEnumerable<T>* source, SharedPointer<Functor<bool(SharedPointer<T>)>> filter): _source(source->GetEnumerator()), _filter(filter){}
            
            bool MoveNext() noexcept override {
                while (_source->MoveNext()) {
                    if (_filter(_source->Get()))
                        return true;
                }
                return false;
            }
            SharedPointer<T> Get() const noexcept override
            {
                return _source->Get();
            }
        };

        template <class T>
        class _IEnumerable_Skip_IEnumerator: public IEnumerator<T> {
        private:
            SharedPointer<IEnumerator<T>> _source;
            int _iterateIndex;
        public:
            _IEnumerable_Skip_IEnumerator(const IEnumerable<T>* source, int number): _source(source->GetEnumerator()), _iterateIndex(0) {
                while (++_iterateIndex < number) {
                    _source->MoveNext();
                }
            }

            bool MoveNext() noexcept override {
                return _source->MoveNext();
            }
            SharedPointer<T> Get() const noexcept override {
                return _source->Get();
            }
        };

        template <class T>
        class _IEnumerable_Take_IEnumerator: public IEnumerator<T>{
        private:
            SharedPointer<IEnumerator<T>> _source;
            const int _number;
            int _iterateIndex;
        public:
            _IEnumerable_Take_IEnumerator(const IEnumerable<T>* source, int number): _source(source->GetEnumerator()), _number(number), _iterateIndex(0) {
            }

            bool MoveNext() noexcept override {
                return ++_iterateIndex <= _number && _source->MoveNext();
            }
            SharedPointer<T> Get() const noexcept override {
                return _source->Get();
            }
        };
    }

    namespace __impl {
        template <class T>
        class _IEnumerableCppStyleForEachHelper_Iterator {
        private:
            SharedPointer<IEnumerator<T>> _source;
            bool _moveNextValue;
        public:
            _IEnumerableCppStyleForEachHelper_Iterator(const IEnumerable<T>* source): _source(source->GetEnumerator()), _moveNextValue(_source->MoveNext()) {

            }

            _IEnumerableCppStyleForEachHelper_Iterator& operator++() {
                _moveNextValue = _source->MoveNext();
                return *this;
            }
            typename IEnumerator<T>::EnumeratorType operator*() const {
                return _source->Get();
            }
            bool operator!=(const _IEnumerableCppStyleForEachHelper_EndIterator<T>&) {
                return _moveNextValue;
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

        virtual bool Contains(T elem) const = 0;
        virtual void Remove(T elem) = 0;
    };

    template <class T>
    class ICollection: public IInputCollection<T>, public IOutputCollection<T> {};

    template <class TKey, class TValue>
    class IKeyValueCollection: public IEnumerable<Pair<TKey, TValue>> {
        virtual int GetCount() const = 0;
        virtual void Add(TKey key, TValue value) = 0;
        virtual void Clear() = 0;
        virtual bool Contains(TKey key) const = 0;
        virtual void Remove(TKey key) = 0;
        virtual SharedPointer<TValue> Find(TKey key) const = 0;
    };
}


namespace Commons{
    /**
     * This interface indicates that this type can be compared to type TKey
     * @tparam T
     */
    template <class T>
    class IComparable{
    public:
        virtual int CompareTo(T other) const = 0;
    };

    /**
     * This interface indicates that this is a function that compare two objects (denoted as a, b) of type TKey
     * return value rt > 0 indicates a > b
     * rt == 0 indicates a == b
     * rt < 0 indicates a < b
     * @tparam T
     */
    template <class T>
    using IComparator = Functor<int(const T&, const T&)>;
    template <class T>
    using IValueComparator = Functor<int(T, T)>;
    template <class T>
    using IEqualityComparator = Functor<bool(const T&, const T&)>;
    template <class T>
    using IValueEqualityComparator = Functor<bool(const T, const T)>;

    template <class T>
    SharedPointer<IComparator<T>> GetDefaultComparator()
    #if ENABLECONCEPT
    requires (CharTraits<TKey>::Value||IsIntegral<TKey>::Value)
    #endif
    {
        MacroDeclareLambdaFunctor2(ptr, [], const T&, const T&, int, {
            return arg1 - arg2;
        });
        return ptr;
    }
    namespace __impl {
        template <class T>
        int _DefaultValueCompare(T a, T b) {
            return (int)(a-b);
        }
    }
    template <class T>
    SharedPointer<IValueComparator<T>> GetDefaultValueComparator() {
        auto ptr = MakeShared<Fun<int(T, T)>>(&__impl::_DefaultValueCompare<T>).template StaticCast<IValueComparator<T>>();
        return ptr;
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
        return MakeShared<Fun<bool(const T&, const T&)>>(&__impl::_Equals<T>).template StaticCast<IEqualityComparator<T>>();
    }
    template <class T>
    SharedPointer<IValueEqualityComparator<T>> GetDefaultValueEqualityComparator(){
        return MakeShared<Fun<bool(const T, const T)>>(&__impl::_ValueEquals<T>).template StaticCast<IValueEqualityComparator<T>>();
    }
}


namespace Commons{
    using namespace Collections;
    #if ENABLECONCEPT
    template <Integral TKey>
    #else
    template <class T>
    #endif
    struct Range;

    namespace __impl{
        #if ENABLECONCEPT
        template <Integral TKey>
        #else
        template <class T>
        #endif
        class Range_Enumerator: public IEnumerator<T>{
        private:
            const T _start;
            const T _end;
            T _current;

        public:
            explicit Range_Enumerator(const Range<T>* r):_start(r->GetStart()), _end(r->GetEnd()), _current(r->GetStart() - 1){
                if (_start > _end) {
                    _current = _start + 1;
                }
            }
            bool MoveNext() noexcept override {
                if (_start < _end)
                    return ++_current < _end;
                else
                    return --_current > _end;
            }
            SharedPointer<T> Get() const noexcept override {
                return MakeShared<T>(const_cast<Range_Enumerator<T>*>(this)->_current);
            }
        };
    }

    #if ENABLECONCEPT
    template <Integral TKey>
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

        SharedPointer<IEnumerator<T>> GetEnumerator() const override {
            auto ptr = MakeShared<::Commons::__impl::Range_Enumerator<T>>(this);
            return ptr.template StaticCast<IEnumerator<T>>();
        }

    };
    
    #if ENABLECONCEPT
    template <Integral TKey>
    #else
    template <class T>
    #endif
    Range<T> To(T start, T end){
        return Range(start, end);
    }
    #if ENABLECONCEPT
    template <Integral TKey>
    #else
    template <class T>
    #endif
    Range<T> Until(T start, T end){
        if (end >= start)
            return Range(start, ++end);
        else
            return Range(start, --end);
    }
}


namespace Commons::Collections {
    template <class T>
    class Queue {
    public:
        virtual void Enqueue(T elem) = 0;
        virtual T Dequeue() = 0;
        virtual T Front() const = 0;
    };
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
        inline T Data() const { return _data; }
        inline SharedPointer<ListNode<T>> Prev() const { return _prev.Pin(); }
        inline SharedPointer<ListNode<T>> Next() const { return _next; }
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
            bool MoveNext() noexcept override {
                if (!_cur)
                    return false;
                if (_startState) {
                    _startState = false;
                    return true;
                }
                _cur = _cur->Next();
                return _cur != _root;
            }
            SharedPointer<T> Get() const noexcept override {
                return MakeShared<T>(_cur->Data());
            }
        };
    }


    template <class T>
    class List: public ICollection<T>, public Queue<T> {
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
            if (!node->_prev.Pin() || !node->_next) {
                node = node;
            }
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
                ++_count;
            }
            _root = newNode;
            return newNode;
        }
        SharedPointer<ListNode<T>> AddToTail(T elem) {
            auto newNode = MakeShared<ListNode<T>>(elem);
            if (_root) {
                AddAfter(newNode, _root->_prev.Pin());
            } else {
                newNode->_next = newNode;
                newNode->_prev = newNode;
                _root = newNode;
                ++_count;
            }
            return newNode;
        }

        void Add(T elem) override {
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

        bool Contains(T elem) const override {
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
            int p = 0;
            while (p < deleteIndex) {
                RemoveNode(deleteList[p++]);
            }
        }

        void Remove(T elem) override {
            Remove(elem, GetDefaultValueEqualityComparator<T>());
        }

        ~List() override {
            if (_root)
                _root->_next = nullptr;
        }

        SharedPointer<IEnumerator<T>> GetEnumerator() const override {
            auto enumerator = MakeShared<__impl::ListIterator<T>>(this);
            return enumerator.template StaticCast<IEnumerator<T>>();
        }

        int GetCount() const override {
            return _count;
        }

        void Clear() override {
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
        T Get(int index) const {
            if (index>=0 && index<_count) {
                auto node = _root;
                MacroDeclareLambdaFunctor1(look, [&node], SharedPointer<int>, void, { node = node->_next; });
                To(0, index).ForEach(look);
                return node->Data();
            }
            if (index<0 && -index>=-_count){
                auto node = _root;
                MacroDeclareLambdaFunctor1(look, [&node], SharedPointer<int>, void, { node = node->Prev(); });
                To(0, -index).ForEach(look);
                return node->Data();
            }
            ShallThrow(std::out_of_range("List index out of range"));
            DummyThrow();
        }
        const T& Set(int index, const T& item) {
            if (index>=0 && index<_count) {
                auto node = _root;
                MacroDeclareLambdaFunctor1(look, [&node], SharedPointer<int>, void, { node = node->_next; });
                To(0, index).ForEach(look);
                auto prev = node->Prev();
                RemoveNode(node);
                if (prev == node->Prev()) {
                    AddToTail(item);
                } else {
                    AddAfter(prev, MakeShared<ListNode<T>>(item));
                }
                goto ret;
            }
            if (index<0 && -index>=-_count) {
                auto node = _root;
                MacroDeclareLambdaFunctor1(look, [&node], SharedPointer<int>, void, { node = node->Prev(); });
                To(0, index).ForEach(look);
                auto prev = node->Prev();
                RemoveNode(node);
                if (prev == node->Prev()) {
                    AddToTail(item);
                } else {
                    AddAfter(prev, MakeShared<ListNode<T>>(item));
                }
                goto ret;
            }
            ShallThrow(std::out_of_range("List index out of range"));
            ret: return item;
        }

        void Enqueue(T elem) override {
            AddToTail(elem);
        }
        T Dequeue() override {
            const auto g = (_root->Data());
            RemoveNode(_root);
            return g;
        }
        T Front() const override {
            return _root->Data();
        }

        /**
         * Make the original front the new tail, and return its value
         * @return
         */
        T Skew() {
            const auto& g = _root->Data();
            _root = _root->_next;
            return g;
        }


    };
}


namespace Commons::Math {
    template <class T> struct MathFunctionProvider {
    private:
        static constexpr double logTable[] {-1,
            0, 0.693147, 1.09861, 1.38629, 1.60944, 1.79176, 1.94591, 2.07944, 2.19722, 2.30259, 2.3979, 2.48491, 2.56495, 2.63906, 2.70805, 2.77259, 2.83321, 2.89037, 2.94444, 2.99573, 3.04452, 3.09104, 3.13549, 3.17805, 3.21888, 3.2581, 3.29584, 3.3322, 3.3673, 3.4012,
            3.43399, 3.46574, 3.49651, 3.52636, 3.55535, 3.58352, 3.61092, 3.63759, 3.66356, 3.68888, 3.71357, 3.73767, 3.7612, 3.78419, 3.80666, 3.82864, 3.85015, 3.8712, 3.89182, 3.91202, 3.93183, 3.95124, 3.97029, 3.98898, 4.00733, 4.02535, 4.04305, 4.06044, 4.07754, 4.09434,
            4.11087, 4.12713, 4.14313, 4.15888, 4.17439, 4.18965, 4.20469, 4.21951, 4.23411, 4.2485, 4.26268, 4.27667, 4.29046, 4.30407, 4.31749, 4.33073, 4.34381, 4.35671, 4.36945, 4.38203, 4.39445, 4.40672, 4.41884, 4.43082, 4.44265, 4.45435, 4.46591, 4.47734, 4.48864, 4.49981,
            4.51086, 4.52179, 4.5326, 4.54329, 4.55388, 4.56435, 4.57471, 4.58497, 4.59512, 4.60517, 4.61512, 4.62497, 4.63473, 4.64439, 4.65396, 4.66344, 4.67283, 4.68213, 4.69135, 4.70048, 4.70953, 4.7185, 4.72739, 4.7362, 4.74493, 4.75359, 4.76217, 4.77068, 4.77912, 4.78749,
            4.79579, 4.80402, 4.81218, 4.82028, 4.82831, 4.83628, 4.84419, 4.85203, 4.85981, 4.86753, 4.8752, 4.8828, 4.89035, 4.89784, 4.90527, 4.91265, 4.91998, 4.92725, 4.93447, 4.94164, 4.94876, 4.95583, 4.96284, 4.96981, 4.97673, 4.98361, 4.99043, 4.99721, 5.00395, 5.01064,
            5.01728, 5.02388, 5.03044, 5.03695, 5.04343, 5.04986, 5.05625, 5.0626, 5.0689, 5.07517, 5.0814, 5.0876, 5.09375, 5.09987, 5.10595, 5.11199, 5.11799, 5.12396, 5.1299, 5.1358, 5.14166, 5.14749, 5.15329, 5.15906, 5.16479, 5.17048, 5.17615, 5.18178, 5.18739, 5.19296,
            5.1985, 5.20401, 5.20949, 5.21494, 5.22036, 5.22575, 5.23111, 5.23644, 5.24175, 5.24702, 5.25227, 5.2575, 5.26269, 5.26786, 5.273, 5.27811, 5.2832, 5.28827, 5.2933, 5.29832, 5.3033, 5.30827, 5.31321, 5.31812, 5.32301, 5.32788, 5.33272, 5.33754, 5.34233, 5.34711,
            5.35186, 5.35659, 5.36129, 5.36598, 5.37064, 5.37528, 5.3799, 5.3845, 5.38907, 5.39363, 5.39816, 5.40268, 5.40717, 5.41165, 5.4161, 5.42053, 5.42495, 5.42935, 5.43372, 5.43808, 5.44242, 5.44674, 5.45104, 5.45532, 5.45959, 5.46383, 5.46806, 5.47227, 5.47646, 5.48064,
            5.4848, 5.48894, 5.49306, 5.49717, 5.50126, 5.50533, 5.50939, 5.51343, 5.51745, 5.52146, 5.52545, 5.52943, 5.53339, 5.53733, 5.54126, 5.54518, 5.54908, 5.55296, 5.55683, 5.56068, 5.56452, 5.56834, 5.57215, 5.57595, 5.57973, 5.5835, 5.58725, 5.59099, 5.59471, 5.59842,
            5.60212, 5.6058, 5.60947, 5.61313, 5.61677, 5.6204, 5.62402, 5.62762, 5.63121, 5.63479, 5.63835, 5.64191, 5.64545, 5.64897, 5.65249, 5.65599, 5.65948, 5.66296, 5.66643, 5.66988, 5.67332, 5.67675, 5.68017, 5.68358, 5.68698, 5.69036, 5.69373, 5.69709, 5.70044, 5.70378,
            5.70711, 5.71043, 5.71373, 5.71703, 5.72031, 5.72359, 5.72685, 5.7301, 5.73334, 5.73657, 5.73979, 5.743, 5.7462, 5.74939, 5.75257, 5.75574, 5.7589, 5.76205, 5.76519, 5.76832, 5.77144, 5.77455, 5.77765, 5.78074, 5.78383, 5.7869, 5.78996, 5.79301, 5.79606, 5.79909,
            5.80212, 5.80513, 5.80814, 5.81114, 5.81413, 5.81711, 5.82008, 5.82305, 5.826, 5.82895, 5.83188, 5.83481, 5.83773, 5.84064, 5.84354, 5.84644, 5.84932, 5.8522, 5.85507, 5.85793, 5.86079, 5.86363, 5.86647, 5.8693, 5.87212, 5.87493, 5.87774, 5.88053, 5.88332, 5.8861,
            5.88888, 5.89164, 5.8944, 5.89715, 5.8999, 5.90263, 5.90536, 5.90808, 5.9108, 5.9135, 5.9162, 5.91889, 5.92158, 5.92426, 5.92693, 5.92959, 5.93225, 5.93489, 5.93754, 5.94017, 5.9428, 5.94542, 5.94803, 5.95064, 5.95324, 5.95584, 5.95842, 5.96101, 5.96358, 5.96615,
            5.96871, 5.97126, 5.97381, 5.97635, 5.97889, 5.98141, 5.98394, 5.98645, 5.98896, 5.99146, 5.99396, 5.99645, 5.99894, 6.00141, 6.00389, 6.00635, 6.00881, 6.01127, 6.01372, 6.01616, 6.01859, 6.02102, 6.02345, 6.02587, 6.02828, 6.03069, 6.03309, 6.03548, 6.03787, 6.04025,
            6.04263, 6.04501, 6.04737, 6.04973, 6.05209, 6.05444, 6.05678, 6.05912, 6.06146, 6.06379, 6.06611, 6.06843, 6.07074, 6.07304, 6.07535, 6.07764, 6.07993, 6.08222, 6.0845, 6.08677, 6.08904, 6.09131, 6.09357, 6.09582, 6.09807, 6.10032, 6.10256, 6.10479, 6.10702, 6.10925,
            6.11147, 6.11368, 6.11589, 6.1181, 6.1203, 6.12249, 6.12468, 6.12687, 6.12905, 6.13123, 6.1334, 6.13556, 6.13773, 6.13988, 6.14204, 6.14419, 6.14633, 6.14847, 6.1506, 6.15273, 6.15486, 6.15698, 6.1591, 6.16121, 6.16331, 6.16542, 6.16752, 6.16961, 6.1717, 6.17379,
            6.17587, 6.17794, 6.18002, 6.18208, 6.18415, 6.18621, 6.18826, 6.19032, 6.19236, 6.19441, 6.19644, 6.19848, 6.20051, 6.20254, 6.20456, 6.20658, 6.20859, 6.2106, 6.21261, 6.21461, 6.21661, 6.2186, 6.22059, 6.22258, 6.22456, 6.22654, 6.22851, 6.23048, 6.23245, 6.23441,
            6.23637, 6.23832, 6.24028, 6.24222, 6.24417, 6.24611, 6.24804, 6.24998, 6.2519, 6.25383, 6.25575, 6.25767, 6.25958, 6.26149, 6.2634, 6.2653, 6.2672, 6.2691, 6.27099, 6.27288, 6.27476, 6.27664, 6.27852, 6.2804, 6.28227, 6.28413, 6.286, 6.28786, 6.28972, 6.29157,
            6.29342, 6.29527, 6.29711, 6.29895, 6.30079, 6.30262, 6.30445, 6.30628, 6.3081, 6.30992, 6.31173, 6.31355, 6.31536, 6.31716, 6.31897, 6.32077, 6.32257, 6.32436, 6.32615, 6.32794, 6.32972, 6.3315, 6.33328, 6.33505, 6.33683, 6.33859, 6.34036, 6.34212, 6.34388, 6.34564,
            6.34739, 6.34914, 6.35089, 6.35263, 6.35437, 6.35611, 6.35784, 6.35957, 6.3613, 6.36303, 6.36475, 6.36647, 6.36819, 6.3699, 6.37161, 6.37332, 6.37502, 6.37673, 6.37843, 6.38012, 6.38182, 6.38351, 6.38519, 6.38688, 6.38856, 6.39024, 6.39192, 6.39359, 6.39526, 6.39693,
            6.39859, 6.40026, 6.40192, 6.40357, 6.40523, 6.40688, 6.40853, 6.41017, 6.41182, 6.41346, 6.4151, 6.41673, 6.41836, 6.41999, 6.42162, 6.42325, 6.42487, 6.42649, 6.42811, 6.42972, 6.43133, 6.43294, 6.43455, 6.43615, 6.43775, 6.43935, 6.44095, 6.44254, 6.44413, 6.44572,
            6.44731, 6.44889, 6.45047, 6.45205, 6.45362, 6.4552, 6.45677, 6.45834, 6.4599, 6.46147, 6.46303, 6.46459, 6.46614, 6.4677, 6.46925, 6.4708, 6.47235, 6.47389, 6.47543, 6.47697, 6.47851, 6.48004, 6.48158, 6.48311, 6.48464, 6.48616, 6.48768, 6.4892, 6.49072, 6.49224,
            6.49375, 6.49527, 6.49677, 6.49828, 6.49979, 6.50129, 6.50279, 6.50429, 6.50578, 6.50728, 6.50877, 6.51026, 6.51175, 6.51323, 6.51471, 6.51619, 6.51767, 6.51915, 6.52062, 6.52209, 6.52356, 6.52503, 6.52649, 6.52796, 6.52942, 6.53088, 6.53233, 6.53379, 6.53524, 6.53669,
            6.53814, 6.53959, 6.54103, 6.54247, 6.54391, 6.54535, 6.54679, 6.54822, 6.54965, 6.55108, 6.55251, 6.55393, 6.55536, 6.55678, 6.5582, 6.55962, 6.56103, 6.56244, 6.56386, 6.56526, 6.56667, 6.56808, 6.56948, 6.57088, 6.57228, 6.57368, 6.57508, 6.57647, 6.57786, 6.57925,
            6.58064, 6.58203, 6.58341, 6.58479, 6.58617, 6.58755, 6.58893, 6.5903, 6.59167, 6.59304, 6.59441, 6.59578, 6.59715, 6.59851, 6.59987, 6.60123, 6.60259, 6.60394, 6.6053, 6.60665, 6.608, 6.60935, 6.6107, 6.61204, 6.61338, 6.61473, 6.61607, 6.6174, 6.61874, 6.62007,
            6.62141, 6.62274, 6.62407, 6.62539, 6.62672, 6.62804, 6.62936, 6.63068, 6.632, 6.63332, 6.63463, 6.63595, 6.63726, 6.63857, 6.63988, 6.64118, 6.64249, 6.64379, 6.64509, 6.64639, 6.64769, 6.64898, 6.65028, 6.65157, 6.65286, 6.65415, 6.65544, 6.65673, 6.65801, 6.65929,
            6.66058, 6.66185, 6.66313, 6.66441, 6.66568, 6.66696, 6.66823, 6.6695, 6.67077, 6.67203, 6.6733, 6.67456, 6.67582, 6.67708, 6.67834, 6.6796, 6.68085, 6.68211, 6.68336, 6.68461, 6.68586, 6.68711, 6.68835, 6.6896, 6.69084, 6.69208, 6.69332, 6.69456, 6.6958, 6.69703,
            6.69827, 6.6995, 6.70073, 6.70196, 6.70319, 6.70441, 6.70564, 6.70686, 6.70808, 6.7093, 6.71052, 6.71174, 6.71296, 6.71417, 6.71538, 6.71659, 6.7178, 6.71901, 6.72022, 6.72143, 6.72263, 6.72383, 6.72503, 6.72623, 6.72743, 6.72863, 6.72982, 6.73102, 6.73221, 6.7334,
            6.73459, 6.73578, 6.73697, 6.73815, 6.73934, 6.74052, 6.7417, 6.74288, 6.74406, 6.74524, 6.74641, 6.74759, 6.74876, 6.74993, 6.7511, 6.75227, 6.75344, 6.7546, 6.75577, 6.75693, 6.75809, 6.75926, 6.76041, 6.76157, 6.76273, 6.76388, 6.76504, 6.76619, 6.76734, 6.76849,
            6.76964, 6.77079, 6.77194, 6.77308, 6.77422, 6.77537, 6.77651, 6.77765, 6.77878, 6.77992, 6.78106, 6.78219, 6.78333, 6.78446, 6.78559, 6.78672, 6.78784, 6.78897, 6.7901, 6.79122, 6.79234, 6.79347, 6.79459, 6.79571, 6.79682, 6.79794, 6.79906, 6.80017, 6.80128, 6.80239,
        };
    public:
        static constexpr double Log(int v) {
            return logTable[v];
        }
    };
}


namespace Commons::Collections {
    template<class TKey, class TValue>
    class FibHeap;
    namespace __impl {
        template<class TKey, class TValue>
        class _FibHeapTraverse;
    }

    template<class TKey, class TValue = TKey>
    class FibNode :public ManagedObject<FibNode<TKey, TValue>> {
    private:
        using Node = FibNode<TKey, TValue>;
        friend class FibHeap<TKey, TValue>;
        friend class __impl::_FibHeapTraverse<TKey, TValue>;

        TKey key;
        TValue value;
        int degree;
        WeakPointer <Node> left;
        SharedPointer <Node> right;
        SharedPointer <Node> child;
        WeakPointer <Node> parent;
        bool marked;
        bool minimumMarkDuringRemove;

    public:
        FibNode(TKey key, TValue value) :key(key), value(value), degree(0), marked(false), left(nullptr), right(nullptr), child(nullptr), parent(nullptr), minimumMarkDuringRemove(false) { }
        FibNode(TKey key) :FibNode(key, key) { }

        TKey Key() const { return key; }
        TValue Value() const { return value; }
        SharedPointer<Node> Left() const { return left.Pin(); }
        SharedPointer<Node> Right() const { return right; }
        SharedPointer<Node> Parent() const { return parent.Pin(); }
    };

    template<class TKey, class TValue = TKey>
    class FibHeap :public ManagedObject<FibHeap<TKey, TValue>>, public IKeyValueCollection<TKey, TValue> {
    private:
        using Node = FibNode<TKey, TValue>;
        using Shared = SharedPointer<Node>;
        using Weak = WeakPointer<Node>;
        using EnumerateType = SharedPointer<Pair<TKey, TValue>>;

        int num;
        Shared min;
        SharedPointer<IValueComparator<TKey>> comparator;

    public:
        FibHeap(SharedPointer<IValueComparator<TKey>> c) :num(0), comparator(c), min(nullptr) { }

    private:
        int CompareKey(TKey a, TKey b) const {
            return comparator->Invoke(a, b);
        }
        int CompareNode(Shared a, Shared b) const {
            if (a->minimumMarkDuringRemove)
                return -1;
            if (b->minimumMarkDuringRemove)
                return 1;
            return CompareKey(a->key, b->key);
        }

        void RemoveNode(Shared node) {
            node->Left()->right = node->right;
            node->right->left = node->Left();
        }

        void AddNode(Shared node, Shared root) {
            node->left = root->Left();
            node->Left()->right=node;
            node->right=root;
            root->left=node;
        }
        void Insert(Shared node) {
            if (num == 0) {
                min = node;
                node->left = node;
                node->right = node;
            }
            else {
                AddNode(node, min);
                if(node->key < min->key)
                    min = node;
            }
            ++num;
        }

        void ConcatenateRootList(Shared a, Shared b) {
            Shared t;
            t=a->right;
            a->right=b->right;
            b->right->left=a;
            b->right=t;
            t->left=b;
        }

        void Combine(SharedPointer<FibHeap<TKey, TValue>> other) {
            ThrowIf0(other);
            if (!min) {
                min=other->min;
                num = other->num;
            }
            else if (!other->min) {
            }
            else {
                ConcatenateRootList(min, other->min);
                if (CompareKey(min->key, other->min->key) > 0) {
                    min = other->min;
                }
                num += other->num;
            }
        }

        void Link(Shared node, Shared root) {
            RemoveNode(node);
            if (!root->child)
                root->child = node;
            else
                AddNode(node, root->child);
            node->parent = root;
            ++root->degree;
            node->marked = false;
        }
        void Consolidate() {
            auto log = &Commons::Math::MathFunctionProvider<int>::Log;
            int D = log(num)/log(2) + 1;
            Shared a[D+1];
            while (min) {
                Shared x = ExtractMin();
                int d = x->degree;
                while (a[d]) {
                    auto y = a[d];
                    if (CompareNode(x, y) > 0)
                        Swap(x, y);
                    Link(y, x);
                    a[d] = nullptr;
                    ++d;
                }
                a[d] = x;
            }
            min = nullptr;
            for (int i=0; i<D; ++i) {
                if (!a[i]) {
                    if (!min)
                        min = a[i];
                    else {
                        AddNode(a[i], min);
                        if (CompareNode(a[i], min) < 0)
                            min = a[i];
                    }
                }
            }
        }
        void RemoveMin(){
            if (!min)
                return;
            Shared child = nullptr;
            Shared m = min;
            while (m->child) {
                child = m->child;
                RemoveNode(child);
                if (child->right == child)
                    m->child = nullptr;
                else
                    m->child = child->right;
            }
            RemoveNode(m);
            if (m->right == m)
                min = nullptr;
            else {
                min = m->right;
                Consolidate();
            }
            --num;
        }

        void RefreshDegree(Shared parent, int degree){
            parent->degree -= degree;
            if (!parent->parent)
                RefreshDegree(parent->parent, degree);
        }
        void Cut(Shared node, Shared parent) {
            RemoveNode(node);
            RefreshDegree(parent, node->degree);
            if (node == node->right)
                parent->child = nullptr;
            else
                parent->child = node->right;
            node->parent = nullptr;
            node->left = node->right = node;
            node->marked = false;
            AddNode(node, min);
        }
        void CascadingCut(Shared node) {
            auto parent = node->Parent();
            if (!parent) {
                if (!node->marked)
                    node->marked = true;
                else {
                    Cut(node, parent);
                    CascadingCut(parent);
                }
            }
        }

        Shared Find(Shared root, TKey key) const {
            auto t = root;
            Shared p = nullptr;
            if (!root)
                return root;
            do {
                if (CompareKey(t->key, key) == 0) {
                    p = t;
                    break;
                } else {
                    if (!(p = Find(t->child, key)))
                        break;
                }
                t = t->right;
            } while (t != root);
            return p;
        }

        void Remove(Shared node) {
            node->minimumMarkDuringRemove = true;
            RemoveMin();
        }

        void BreakChain(Shared node) {
            if (!node)
                return;
            const auto start = node;
            do {
                BreakChain(node->child);
                node = node->right;
            } while (node != start);
        }

    public:
        Shared Insert(TKey key, TValue value) {
            auto node = MakeShared<Node>(key, value);
            Insert(node);
            return node;
        }

        Shared ExtractMin() {
            Shared p = min;
            if (p == p->right)
                min = nullptr;
            else {
                RemoveNode(p);
                min = p->right;
            }
            p->left=p->right=p;
            return p;
        }

        Shared Minimum() const {
            return min;
        }

        void DecreaseKey(Shared node, TKey key) {
            if (!min || !node)
                DummyThrow();
            if (CompareKey(key, node->key) > 0) {
                DummyThrow();
            }
            node->key = key;
            auto parent = node->Parent();
            if (!parent && CompareNode(node, parent) < 0) {
                Cut(node, parent);
                CascadingCut(parent);
            }
            if (CompareNode(node, min) < 0)
                min = node;
        }
        void IncreaseKey(Shared node, TKey key) {
            if (!min || !node)
                DummyThrow();
            if (CompareKey(key, node->key) <= 0)
                DummyThrow();
            while (node->child) {
                auto child = node->child;
                RemoveNode(child);
                if (child->right == child)
                    node->child = nullptr;
                else
                    node->child = child->right;
                AddNode(child, min);
                child->parent = nullptr;
            }
            node->degree = nullptr;
            node->key = key;
            auto parent = node->Parent();
            if (!parent) {
                Cut(node, parent);
                CascadingCut(parent);
            }
            else if (min == node) {
                auto right = node->right;
                while (right != node) {
                    if (CompareNode(node, right) > 0)
                        min = right;
                    right = right->right;
                }
            }
        }

        void UpdateKey(TKey oldKey, TKey newKey) {
            auto node = Find(oldKey);
            if (!node)
                DummyThrow();
            Update(node, newKey);
        }

        void Add(TKey key, TValue value) override {
            Insert(key, value);
        }

        SharedPointer<IEnumerator<Pair<TKey, TValue>>> GetEnumerator() const override {
            return MakeShared<__impl::_FibHeapTraverse<TKey, TValue>>(this).template StaticCast<IEnumerator<Pair<TKey, TValue>>>();
        }

        int GetCount() const override {
            return num;
        }

        SharedPointer<TValue> Find(TKey key) const override {
            if (!min)
                return nullptr;
            auto p = Find(min, key);
            if (!p)
                return nullptr;
            return MakeShared<TValue>(p->value);
        }

        bool Contains(TKey key) const override {
            return Find(key);
        }

        void Remove(TKey key) override {
            if (!min)
                return;
            auto node = Find(key);
            if (!node)
                return;
            Remove(node);
        }

        void Clear() override {
            BreakChain(min);
            min = nullptr;
            num = 0;
        }

        ~FibHeap() noexcept override {
            BreakChain(min);
        }
    };

    namespace __impl {
        template <class TKey, class TValue>
        class _FibHeapTraverse : public IEnumerator<Pair<TKey, TValue>> {
        private:
            typedef FibNode<TKey, TValue> Node;
            typedef Pair<TKey, TValue> EnumerateType;
            typedef SharedPointer<Pair<TKey, TValue>> RetType;

            int state;
            SharedPointer<List<EnumerateType>> a;
            SharedPointer<IEnumerator<EnumerateType>> em;
            SharedPointer<Node> tree_min;

            void AddToList(SharedPointer<Node> node) {
                EnumerateType f(node->key, node->value);
                a->Add(f);
            }
            void Traverse(SharedPointer<Node> node) {
                auto start = node;
                if (!node)
                    return;
                do {
                    AddToList(node);
                    Traverse(node->child);
                    node = node->right;
                } while (node != start);
            }
            void TraverseAll() {
                a = SharedPointer<List<EnumerateType>>();
                if (!tree_min)
                    return;
                auto p = tree_min;
                do {
                    AddToList(p);
                    Traverse(p->child);
                    p = p->right;
                } while (p != tree_min);
                em = a->GetEnumerator();
                a = nullptr;
            }

        public:
            _FibHeapTraverse(const FibHeap<TKey, TValue>* heap): state(0), a(nullptr), tree_min(heap->Minimum()) {
            }

            bool MoveNext() noexcept override {
                if (state == 0) {
                    TraverseAll();
                    state = 1;
                }
                return em->MoveNext();
            }
            RetType Get() const noexcept override {
                return em->Get();
            }
        };
    }

    template <class TKey>
    class PriorityQueue: private FibHeap<TKey, TKey>, public Queue<TKey> {
    public:
        using FibHeap<TKey, TKey>::FibHeap;
        PriorityQueue(IValueComparator<TKey> c): FibHeap<TKey, TKey>(c) {}
        void Enqueue(TKey key) override {
            FibHeap<TKey, TKey>::Insert(key, key);
        }
        TKey Front() const override {
            return FibHeap<TKey, TKey>::Minimum()->Value();
        }
        TKey Dequeue() override {
            return FibHeap<TKey, TKey>::ExtractMin()->Value();
        }
    };
}


/*
 * FIFO Scheduling Algorithm
 * SKELETON IMPLEMENTATION -- TO BE FILLED IN FOR TASK (2)
 */

/*
 * STUDENT NUMBER: s2067807
 */
#include <infos/kernel/sched.h>
#include <infos/kernel/thread.h>
#include <infos/kernel/log.h>
#include <infos/util/list.h>
#include <infos/util/lock.h>

using namespace infos::kernel;
using namespace infos::util;




/**
 * A FIFO scheduling algorithm
 */
class FIFOScheduler : public SchedulingAlgorithm
{
public:
    FIFOScheduler() {

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
        queue.AddToTail(&entity);
	}

	/**
	 * Called when a scheduling entity is no longer eligible for running.
	 * @param entity
	 */
	void remove_from_runqueue(SchedulingEntity& entity) override
	{
        UniqueIRQLock _l;
        queue.Remove(&entity);
	}

	/**
	 * Called every time a scheduling event occurs, to cause the next eligible entity
	 * to be chosen.  The next eligible entity might actually be the same entity, if
	 * e.g. its timeslice has not expired, or the algorithm determines it's not time to change.
	 */
	SchedulingEntity *pick_next_entity() override
	{
        if (!queue.GetCount())
            return nullptr;
        return queue.Get(0);
	}

private:
	// A list containing the current runqueue.
	Commons::Collections::List<SchedulingEntity*> queue;

};

/* --- DO NOT CHANGE ANYTHING BELOW THIS LINE --- */

RegisterScheduler(FIFOScheduler);
