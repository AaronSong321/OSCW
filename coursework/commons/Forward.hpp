
namespace Commons{
    template <class T> struct RemoveReference {typedef T Type;};
    template <class T> struct RemoveReference<T&> {typedef T Type;};
    template <class T> struct RemoveReference<T&&> {typedef T Type;};

    template <typename T>
    inline T&& Forward(typename RemoveReference<T>::Type& Obj)
    {
        return (T&&)Obj;
    }

    template <typename T>
    inline T&& Forward(typename RemoveReference<T>::Type&& Obj)
    {
        return (T&&)Obj;
    }

    template <typename T> struct RemoveCV                   { typedef T Type; };
    template <typename T> struct RemoveCV<const T>          { typedef T Type; };
    template <typename T> struct RemoveCV<volatile T>       { typedef T Type; };
    template <typename T> struct RemoveCV<const volatile T> { typedef T Type; };
}