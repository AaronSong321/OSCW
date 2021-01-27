

namespace Commons{
    template<typename T, T v>
    struct IntegralConstant{
        static constexpr T Value = v;
        typedef T ValueType;
        typedef IntegralConstant<T,v> Type;
        constexpr operator T() const noexcept { return v; }
        constexpr T operator()() const noexcept { return v; }
    };
    struct TrueType: public IntegralConstant<bool, true>{};
    struct FalseType: public IntegralConstant<bool, false>{};


    
    template <typename T>
    struct CharTraits: public FalseType{};
    template <>
    struct CharTraits<char>: public TrueType{};
    template <>
    struct CharTraits<wchar_t>: public TrueType{};
    
    template<typename T>
    concept IsChar = CharTraits<T>::Value;
}