/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_NARROW_H_
#define NOTHING_NARROW_H_

#include <nothing/concepts.h>

namespace nothing {

class narrowing_error : public std::exception {
    const char *what() const noexcept { return "narrowing error"; }
};

// clang-format off

template <class T, class U>
    requires (!arithmetic<T>) && narrowable_to<U, T>
constexpr T narrow(U value)
{
    const auto result{ static_cast<T>(value) };

    if (static_cast<U>(result) != value) {
        throw narrowing_error{};
    }

    return result;
}

template <class T, class U>
    requires arithmetic<T> && narrowable_to<U, T>
constexpr T narrow(U value)
{
    const auto result{ static_cast<T>(value) };

    if (static_cast<U>(result) != value ||
        (std::is_signed_v<T> != std::is_signed_v<U> &&
         ((value < U{}) != (result < T{})))) {
        throw narrowing_error{};
    }

    return result;
}

// clang-format on

} // namespace nothing

#endif
