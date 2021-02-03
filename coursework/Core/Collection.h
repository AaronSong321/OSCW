//
// Created by Dimanche on 29/01/2021.
//

#ifndef CPP_COLLECTION_H
#define CPP_COLLECTION_H

#include "Func.h"
#include "Memory.h"
#include "Concepts.h"

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

             typename IEnumerator<U>::EnumeratorType Get() const override {
                return MakeEnumerator(trans(*_source));
            }
            bool MoveNext() override {
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
            
            bool MoveNext() override {
                while (_source.MoveNext()) {
                    if (_filter(_source.Get()))
                        return true;
                }
                return false;
            }
            SharedPointer<T> Get() override
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

#endif //CPP_COLLECTION_H
