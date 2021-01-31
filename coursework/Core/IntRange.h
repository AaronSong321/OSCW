//
// Created by Dimanche on 30/01/2021.
//

#ifndef CPP_INTRANGE_H
#define CPP_INTRANGE_H

#include "Collection.h"
#include "Concepts.h"

namespace Commons{
    #if ENABLECONCEPT
    template <Integral T>
    #else
    template <class T>
    #endif
    struct Range;

    namespace _impl{
        #if ENABLECONCEPT
        template <Integral T>
        #else
        template <class T>
        #endif
        class Range_Enumerator: public Collections::IValueEnumerator<T>{
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
                return MakeShared(_current);
            }
        };
    }

    #if ENABLECONCEPT
    template <Integral T>
    #else
    template <class T>
    #endif
    struct Range: public Collections::IEnumerable<T> {
    private:
        const T _start;
        const T _end;
    public:
        constexpr Range(T start, T end): _start(start), _end(end){
        }
        T GetStart() const { return _start; }
        T GetEnd() const { return _end; }
        virtual Collections::IEnumerable<T> GetEnumerator() override {
            return Collections::_impl::AnonymousEnumerable(_impl::Range_Enumerator(*this));
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


#endif //CPP_INTRANGE_H
