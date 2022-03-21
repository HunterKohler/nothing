/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_UTILITY_H_
#define NOTHING_UTILITY_H_

#include <type_traits>

namespace nothing {

// [utility.underlying], to_underlying (C++23)
template <class T>
constexpr std::underlying_type_t<T> to_underlying(T value) noexcept
{
    return static_cast<std::underlying_type_t<T>>(value);
}

}; // namespace nothing

#endif
