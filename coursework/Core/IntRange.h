//
// Created by Dimanche on 30/01/2021.
//

#ifndef CPP_INTRANGE_H
#define CPP_INTRANGE_H

#include "Collection.h"
#include "Concepts.h"

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
            explicit Range_Enumerator(const Range<T>* r):_start(r->Start()), _end(r->End()), _current(r->Start() - 1){
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
            SharedPointer<T> Current() const noexcept override {
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
        T Start() const { return _start; }
        T End() const { return _end; }

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


#endif //CPP_INTRANGE_H
