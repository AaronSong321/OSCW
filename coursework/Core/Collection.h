//
// Created by Dimanche on 29/01/2021.
//

#ifndef CPP_COLLECTION_H
#define CPP_COLLECTION_H

#include "Func.h"
#include "Memory.h"
#include "Concepts.h"
#include "Pair.h"

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
    // #else
    // template <class TKey>
    // class IValueEnumerator{
    // public:
    //     typedef TKey EnumeratorType;
    //     virtual EnumeratorType GetRawPointer() const = 0;
    //     virtual bool MoveNext() = 0;
    //     template <class... ArgTypes>
    //     static inline EnumeratorType MakeEnumerator(ArgTypes&&... args){
    //         return TKey(Forward<ArgTypes>(args)...);
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
        // using IEnumerable<TKey>::GetEnumerator;
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

#endif //CPP_COLLECTION_H
