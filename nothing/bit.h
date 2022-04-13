/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_BIT_H_
#define NOTHING_BIT_H_

#include <bit>
#include <type_traits>

namespace nothing {

// [bit.byteswap], byteswap (C++23)
template <std::integral T>
constexpr T byteswap(T value) noexcept
{
    if constexpr (sizeof(T) == 1) {
        return value;
    } else if constexpr (sizeof(T) == 2) {
        if constexpr (__has_builtin(__builtin_bswap16)) {
            return __builtin_bswap16(value);
        } else {
            return ((value << 8) & 0xFF00) | ((value >> 8) & 0xFF);
        }
    } else if constexpr (sizeof(T) == 4) {
        if constexpr (__has_builtin(__builtin_bswap32)) {
            return __builtin_bswap32(value);
        } else {
            return ((value << 24) & 0xFF000000) | ((value << 8) & 0xFF0000) |
                   ((value >> 8) & 0xFF00) | ((value >> 24) & 0xFF);
        }
    } else if constexpr (sizeof(T) == 8) {
        if constexpr (__has_builtin(__builtin_bswap64)) {
            return __builtin_bswap64(value);
        } else {
            return ((value << 56) & 0xFF00000000000000) |
                   ((value << 40) & 0xFF000000000000) |
                   ((value << 24) & 0xFF0000000000) |
                   ((value << 8) & 0xFF00000000) | ((value >> 8) & 0xFF000000) |
                   ((value >> 24) & 0xFF0000) | ((value >> 40) & 0xFF00) |
                   ((value >> 56) & 0xFF);
        }
    } else {
        static_assert(!sizeof(T), "Byte swap not supported for type");
        return 0;
    }
}

template <std::integral T>
constexpr T little_endian(T value) noexcept
{
    if constexpr (std::endian::native == std::endian::little) {
        return value;
    } else {
        return byteswap<T>(value);
    }
}

template <std::integral T>
constexpr T big_endian(T value) noexcept
{
    if constexpr (std::endian::native == std::endian::big) {
        return value;
    } else {
        return byteswap<T>(value);
    }
}

} // namespace nothing

#endif
