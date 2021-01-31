//
// Created by Dimanche on 29/01/2021.
//

#pragma once

#ifndef CPP_MEMORY_H
#define CPP_MEMORY_H


namespace Commons{
    template <class T> struct SharedPointer;
    template <class T> struct WeakPointer;

    template<class T,class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs);

    template <class T>
    struct SharedPointer{
    private:
        T* _data;
        int* _count;
        SharedPointer(T* rawPointer, int* counterPointer): _data(rawPointer), _count(counterPointer){
            ++*_count;
        }
        template <class U>
        friend struct WeakPointer;
        template <class U>
        friend struct SharedPointer;

    private:
        void Hold(){
            ++*_count;
        }
        void Release() noexcept{
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
        SharedPointer<T>& operator=(SharedPointer<T>& other){
            other.Hold();
            Release();
            _data = other._data;
            _count = other._count;
            return *this;
        }
        SharedPointer<T>& operator=(void*){
            Release();
            _data = 0;
            _count = 0;
            return *this;
        }
        SharedPointer<T>& operator=(SharedPointer<T>&& other){
            other.Hold();
            Release();
            _data = other._data;
            _count = other._count;
            return *this;
        }

        bool operator==(const SharedPointer<T>& other) const{
            return _data == other._data;
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
        operator bool(){
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
                return SharedPointer<TargetType>(0);
        }

    public:
        template <class TBase>
        SharedPointer<TBase> StaticCast() {
            auto data = static_cast<TBase*>(_data);
            return ConstructPointerAtType(data);
        }

        template <class TDerived>
        SharedPointer<TDerived> DynamicCast() {
            auto data = dynamic_cast<TDerived*>(_data);
            return ConstructPointerAtType(data);
        }

        template <class TRemoveCV>
        SharedPointer<TRemoveCV> ConstCast() {
            auto data = const_cast<TRemoveCV*>(_data);
            return ConstructPointerAtType(data);
        }

        template <class TFP>
        SharedPointer<TFP> ReinterpretCast(){
            auto data = reinterpret_cast<TFP*>(_data);
            return ConstructPointerAtType(data);
        }
    };

    template<class T, class... ArgTypes>
    inline SharedPointer<T> MakeShared(ArgTypes&&... objects){
        auto p = new T(Forward<ArgTypes>(objects)...);
        return SharedPointer<T>(p);
    }

    template<class T,class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return lhs.Get() == rhs.Get();
    }
    template<class T,class U>
    bool operator!=(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return !(lhs == rhs);
    }
    // template<class T>
    // bool operator==(const SharedPointer<T> p, std::nullptr_t){
    //     return p == nullptr;
    // }
    // template<class T>
    // bool operator!=(const SharedPointer<T> p, std::nullptr_t){
    //     return p != nullptr;
    // }

    template <class T>
    struct WeakPointer{
    private:
        T* const _data;
        int* const _count;

    public:
        using ElementType = T;
        explicit WeakPointer(const SharedPointer<T>& p): _data(p.data), _count(p._count){
        }
        SharedPointer<T>&& Pin(){
            return SharedPointer<T>(_data, _count);
        }
    };
}

// #include <cstdlib>
// void* operator new (unsigned long n){
//     void* const p = malloc(n);
//     return p;
// }
// void operator delete(void* p){
//     if (!p)
//         free(p);
// }


#endif //CPP_MEMORY_H
