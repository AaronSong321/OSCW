
#include "Traits.hpp"
#include "SmartPointer.hpp"

namespace Commons {
    template<IsChar T>
    int CountLength(T* stringStart){
        int a = 0;
        while (*(stringStart++)) ++a;
        return a;
    }

    template<IsChar T>
    class String {
    private:
        SharedPointer<T> _data;
        int _length;
    public:
        String(T* f){
            _length = CountLength(f);
            _data = SharedPointer<T>(new T[_length+1]);
        }
        String(const String<T>& other) {
            _data = other._data;
            _length = other._length;
        }
        T* Get() { return _data.Get(); }
    };

    using string = String<char>;
    using wstring = String<wchar_t>;
}