
namespace Commons{
    template <class T> struct SharedPointer;
    template <class T> struct WeakPointer;

    template<class T,class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs);
    template<class T,class U>
    bool operator!=(const SharedPointer<T> lhs, const SharedPointer<U> rhs);

    template <class T>
    struct SharedPointer{
    private:
        T* const _data;
        int* const _count;
        SharedPointer(T* rawPointer, int* counterPointer): _data(rawPointer), _count(counterPointer){
            ++*_count;
        }
        friend struct WeakPointer<T>;

    public:
        using ElementType = T;
        using WeakType = WeakPointer<T>;
        SharedPointer(T* rawPointer): _data(rawPointer), _count(new int(1)){
        }
        SharedPointer(const SharedPointer<T>& p): _data(p._data), _count(p._count) {
            ++*_count;
        }
        SharedPointer(SharedPointer<T>&& p): _data(p._data), _count(p._count) {
            ++*_count;
        }
        SharedPointer<T> operator=(const SharedPointer<T> other) {
            return SharedPointer<T>(other);
        }
        SharedPointer<T> operator=(SharedPointer<T>&& other) {
            return SharedPointer<T>(other);
        }
        ~SharedPointer(){
            if (!--*_count){
                delete _data;
                delete _count;
            }
        }
        T* operator->(){
            return _data;
        }
        T& operator*(){
            return *_data;
        }
        operator bool(){
            return _data != 0;
        }
        T* Get(){return _data;}
        bool IsNull(){return _data == 0;}
    };

    template<class T, class... ArgTypes>
    inline SharedPointer<T> MakeShared(ArgTypes&&... objects){
        auto p=new T(Forward<ArgTypes>(objects)...);
        return SharedPointer<T>(p);
    }

    template<class T,class U>
    bool operator==(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return lhs.Get() == rhs.Get();
    }
    template<class T,class U>
    bool operator!=(const SharedPointer<T> lhs, const SharedPointer<U> rhs){
        return lhs.Get() != rhs.Get();
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
        WeakPointer(const SharedPointer<T>& p): _data(p.data), _count(p._count){
        }
        SharedPointer<T>&& Pin(){
            return SharedPointer<T>(_data, _count);
        }
    };
}

