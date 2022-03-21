/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_BIT_H_
#define NOTHING_BIT_H_

#include <bit>
#include <nothing/concepts.h>

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

namespace detail {

template <class T>
struct [[gnu::packed]] unaligned_value {
    T value;
};

} // namespace detail

// clang-format off

template <class T>
concept alignable = alignof(T) > 1 && sizeof(T) > 0;

template <class T, class U, std::size_t N>
    requires (sizeof(T) && sizeof(U) && N != std::dynamic_extent &&
             !(sizeof(U) * N % sizeof(T)))
inline std::span<U, sizeof(U) * N / sizeof(T)>
span_cast(std::span<U, N> s) noexcept
{
    return reinterpret_cast<T *>(s.data());
}

template <class T, class U>
    requires (sizeof(T) && sizeof(U))
inline std::span<T, std::dynamic_extent>
span_cast(std::span<U, std::dynamic_extent> s)
{
    auto [quot, rem] = std::div(s.size() * sizeof(U));

    if (rem) {
        throw std::runtime_error(
            "nothing::span_cast: Invalid size for span_cast");
    }

    return { reinterpret_cast<T *>(s.data()), quot };
}

template <class T>
    requires trivially_copyable<T> && alignable<T>
T unaligned_load(const T *ptr) noexcept
{
    return reinterpret_cast<const detail::unaligned_value<T> *>(ptr)->value;
}

template <class T>
    requires trivially_copyable<T> && alignable<T>
T unaligned_load(std::span<const std::byte, sizeof(T)> s) noexcept
{
    return unaligned_load<T>(reinterpret_cast<const T *>(s.data()));
}

template <class T>
    requires trivially_copyable<T> && alignable<T>
void unaligned_store(T *ptr, T value) noexcept
{
    reinterpret_cast<detail::unaligned_value<T> *>(ptr)->value = value;
}

template <class T>
    requires trivially_copyable<T> && alignable<T>
void unaligned_store(std::span<std::byte, sizeof(T)> s, T value) noexcept
{
    unaligned_store<T>(reinterpret_cast<T *>(s.data()), value);
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
