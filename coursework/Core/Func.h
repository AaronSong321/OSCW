//
// Created by Dimanche on 29/01/2021.
//

#ifndef CPP_FUNC_H
#define CPP_FUNC_H
#include "TypeTraits.h"
#include "Memory.h"

template <class RetType, class... ArgTypes>
RetType __SomeUglyFunction(ArgTypes...);
#define Function(RetType, ...) decltype(&__SomeUglyFunction<RetType, __VA_ARGS__>)
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
        virtual RetType Invoke(ArgTypes&&... args) const override {
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

#endif //CPP_FUNC_H
