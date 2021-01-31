#pragma once

#include "TypeTraits.h"

namespace Commons {
#if ENABLECONCEPT
	template <class T1, class T2>
	concept SameAs = IsSame<T1, T2>::Value && IsSame<T2, T1>::Value;
	
	template <class From, class To>
	concept ConvertibleTo = IsConvertible<From, To>::Value && requires(AddRvalueReferenceType<From>(&f)()){
	    static_cast<To>(f());
	};

	template<class T>
	concept Char = CharTraits<T>::Value;
	template <class T>
	concept Integral = IsIntegral<T>::Value;

    template <class T>
    concept ValueType = Char<T> || Integral<T> || IsValueType<T>::Value;
#endif
}

