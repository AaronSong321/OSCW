//
// Created by Dimanche on 29/01/2021.
//

#pragma once

#include "Exception.h"
//#include <iostream>
//using namespace std;

#ifndef CPP_MEMORY_H
#define CPP_MEMORY_H

namespace Commons{
//    namespace __impl {
//        struct SharedCountBase {
//        private:
//            long _useCount;
//            long _weakCount;
//        public:
//            SharedCountBase(): _useCount(1), _weakCount(1){
//            }
//            SharedCountBase(const SharedCountBase& other) {}
//            SharedCountBase& operator=(const SharedCountBase& other) {
//
//            }
//
//            virtual ~SharedCountBase() noexcept {}
//            virtual void Dispose() noexcept = 0;
//            virtual void Destruct() {
//                delete this;
//            }
//            void AddRefCopy() {
//                ++_useCount;
//            }
//            void AddRefLock(){
//                if (!_useCount)
//                    //ShallThrow()
//                    ;
//                ++_useCount;
//            }
//            void Release() noexcept {
//                {
//                    // lock here
//                    long newUseCount = --_useCount;
//                    if (newUseCount)
//                        return;
//                }
//                Dispose();
//                WeakRelease();
//            }
//            void WeakAddRef() noexcept {
//                // lock here
//                ++_weakCount;
//            }
//            void WeakRelease() noexcept {
//                long newWeakCount;
//                {
//                    // lock here
//                    newWeakCount = --_weakCount;
//                }
//                if (!newWeakCount) {
//                    Destruct();
//                }
//            }
//            long UseCount() const noexcept {
//                // lock here
//                return _useCount;
//            }
//        };
//
//        template <class T, class Deleter>
//        struct SharedCountBaseImplementation: public SharedCountBase {
//        private:
//            T _pointer;
//
//        public:
//            SharedCountBaseImplementation(const SharedCountBaseImplementation&);
//            SharedCountBaseImplementation& operator=(const SharedCountBaseImplementation&);
//
//        public:
//            SharedCountBaseImplementation(T ptr): _pointer(ptr) {
//            }
//            void Dispose() noexcept override {
//                _deleter(_pointer);
//            }
//
//        };
//
//        struct WeakCount;
//        struct SharedCount {
//        public:
//            SharedCountBase* _pi;
//            friend struct WeakCount;
//
//        public:
//            SharedCount(): _pi(nullptr) {}
//            template <class T> SharedCount(T ptr): _pi(nullptr) {
//                try {
//                    _pi = new SharedCountBaseImplementation<T>(ptr);
//                }
//                catch (...) {
//                    throw;
//                }
//            }
//
//            ~SharedCount() noexcept {
//                if (!_pi)
//                    _pi->Release();
//            }
//
//            SharedCount(const SharedCount& other): _pi(r._pi) noexcept {
//                if (!_pi)
//                    _pi->AddRefCopy();
//            }
//            explicit SharedCount(const WeakCount& other);
//
//            SharedCount& operator=(const SharedCount& other) noexcept {
//                SharedCountBase* temp = other._pi;
//                if (temp != _pi){
//                    if (temp) temp->AddRefCopy();
//                    if (_pi) _pi->Release();
//                    _pi = temp;
//                }
//                return *this;
//            }
//            void Swap(SharedCount& other) noexcept {
//                SharedCountBase* temp = other._pi;
//                other._pi = _pi;
//                _pi = temp;
//            }
//            long UseCount() const noexcept {
//                return _pi?_pi->UseCount():0;
//            }
//            bool Unique() const noexcept {
//                return UseCount() == 1;
//            }
//            friend inline bool operator==(const SharedCount& lhs, const SharedCount& rhs) {
//                return lhs._pi == rhs._pi;
//            }
//            friend inline bool operator<(const SharedCount& lhs, SharedCount& rhs) {
//                return Less<SharedCountBase*>(lhs._pi, rhs._pi);
//            }
//        };
//
//        struct WeakCount {
//        private:
//            SharedCountBase* _pi;
//            friend struct SharedCount;
//
//        public:
//            WeakCount(): _pi(nullptr) {}
//            WeakCount(const SharedCount& other): _pi(other._pi) {
//                if (_pi)
//                    _pi->WeakAddRef();
//            }
//            WeakCount(const WeakCount& other): _pi(other._pi) {
//                if (_pi)
//                    _pi->WeakAddRef();
//            }
//            ~WeakCount() noexcept {
//                if (_pi)
//                    _pi->WeakRelease();
//            }
//
//            WeakCount& operator=(const SharedCount& other) noexcept {
//                SharedCountBase* tmp=other._pi;
//                if (tmp) tmp->WeakAddRef();
//                if (_pi) _pi->WeakRelease();
//                _pi=tmp;
//                return *this;
//            }
//            WeakCount& operator=(const WeakCount& other) noexcept {
//                SharedCountBase* tmp=other._pi;
//                if (tmp) tmp->WeakAddRef();
//                if (_pi) _pi->WeakRelease();
//                _pi=tmp;
//                return *this;
//            }
//            void Swap(WeakCount& other) {
//                SharedCountBase* tmp=other._pi;
//                other._pi=_pi;
//                _pi=tmp;
//            }
//            long UseCount() const noexcept {
//                return _pi?_pi->UseCount():0;
//            }
//            friend inline bool operator==(const WeakCount& lhs, const WeakCount& rhs) {
//                return lhs._pi == rhs._pi;
//            }
//            friend inline bool operator<(const WeakCount& lhs, const WeakCount& rhs) {
//                return Less<SharedCountBase*>()(lhs._pi, rhs._pi);
//            }
//        };
//
//        inline SharedCount::SharedCount(const WeakCount& other): _pi(other._pi) {
//            if (_pi)
//                _pi->AddRefLock();
//            else
//                throw 0;
//        }
//    }
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
            Hold();
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
        T* Get() const { return _data; }
//        friend ostream& operator<<(ostream& f, const SharedPointer<T>& obj) {
//            f << "(data=" << obj._data;
//            f<<", shared count="<<obj._sharedCount;
//            if (obj._sharedCount)
//                f<<" "<<*obj._sharedCount;
//            f<<", weak count="<<obj._weakCount;
//            if (obj._weakCount)
//                f<<" "<<*obj._weakCount;
//            f<<")";
//            return f;
//        }

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

//        friend ostream& operator<<(ostream& f, const WeakPointer<T>& obj) {
//            f << "(data=" << obj._data;
//            f<<", shared count="<<obj._sharedCount;
//            if (obj._sharedCount)
//                f<<" "<<*obj._sharedCount;
//            f<<", weak count="<<obj._weakCount;
//            if (obj._weakCount)
//                f<<" "<<*obj._weakCount;
//            f<<")";
//            return f;
//        }
    };
}


#endif //CPP_MEMORY_H
