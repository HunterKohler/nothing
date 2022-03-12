/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_NARROW_H_
#define NOTHING_NARROW_H_

#include <nothing/type_traits.h>

namespace nothing {

class narrowing_error : public std::exception {
    const char *what() const noexcept
    {
        return "narrowing error";
    }
};

template <class T, class U>
constexpr std::enable_if_t<!std::is_arithmetic_v<T>, T> narrow(U value)
{
    const auto result = static_cast<T>(value);

    if (static_cast<U>(result) != value) {
        throw narrowing_error{};
    }

    return result;
}

template <class T, class U>
constexpr std::enable_if_t<std::is_arithmetic_v<T>, T> narrow(U value)
{
    const auto result = static_cast<T>(value);

    if (static_cast<U>(result) != value ||
        (!is_same_sign_v<T, U> && ((value < U{}) != (result < T{})))) {
        throw narrowing_error{};
    }

    return result;
}

} // namespace nothing

#endif
