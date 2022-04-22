#ifndef NOTHING_IMPLICIT_CAST_H_
#define NOTHING_IMPLICIT_CAST_H_

#include <type_traits>

template <class T>
constexpr T implicit_cast(std::type_identity_t<T> value)
{
    return value;
}

#endif
