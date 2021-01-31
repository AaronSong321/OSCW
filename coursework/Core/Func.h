//
// Created by Dimanche on 29/01/2021.
//

#ifndef CPP_FUNC_H
#define CPP_FUNC_H
#include "TypeTraits.h"

template <class RetType, class... ArgTypes>
RetType __SomeUglyFunction(ArgTypes...);
#define Function(RetType, ...) decltype(&__SomeUglyFunction<RetType, __VA_ARGS__>)

namespace Commons{
    /**
     * Generic functor type
     * The constructor is intentionally implicit
     * @tparam RetType
     * @tparam ArgTypes
     */
    template <class RetType, class... ArgTypes>
    struct Fun {
    private:
        Function(RetType, ArgTypes...) _fpointer;

    public:
        Fun(Function(RetType, ArgTypes...) f): _fpointer(f){}
        RetType operator()(ArgTypes&&... args) const {
            return _fpointer(Forward<ArgTypes>(args)...);
        }
        friend bool operator==(const Fun<RetType, ArgTypes...>&, const Fun<RetType, ArgTypes...>&) = default;

        operator Function(RetType, ArgTypes...)(){
            return _fpointer;
        }

        template <class RetType2, class... ArgTypes2>
        friend struct Fun;
        template <class RetType2, class... ArgTypes2>
        Fun<RetType2, ArgTypes2...> ReinterpretCast(){
            return Fun(reinterpret_cast<RetType2(*)(ArgTypes2...)>(_fpointer));
        }
    };

}

#endif //CPP_FUNC_H
