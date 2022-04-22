/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_UNALIGNED_H_
#define NOTHING_UNALIGNED_H_

#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <nothing/bit.h>

namespace nothing {
namespace detail {

template <class T>
auto unaligned_load_impl(const void *ptr)
{
    return std::bit_cast<T>(
        *reinterpret_cast<const std::array<uint8_t, sizeof(T)> *>(ptr));
}

template <class T>
auto unaligned_store_impl(T value, void *ptr)
{
    *reinterpret_cast<std::array<uint8_t, sizeof(T)> *>(ptr) =
        std::bit_cast<std::array<uint8_t, sizeof(T)>>(value);
}

} // namespace detail

template <class T>
concept alignable = alignof(T) > 1 && std::is_trivially_copyable_v<T>;

template <alignable T>
    requires(!std::integral<T>)
auto unaligned_load(const void *ptr)
{
    return detail::unaligned_load_impl<T>(ptr);
}

template <alignable T, std::endian ByteOrder = std::endian::native>
    requires std::integral<T>
auto unaligned_load(const void *ptr)
{
    auto value = detail::unaligned_load_impl<T>(ptr);

    if constexpr (ByteOrder != std::endian::native) {
        value = byteswap(value);
    }

    return value;
}

template <alignable T>
    requires(!std::integral<T>)
void unaligned_store(T value, void *ptr)
{
    detail::unaligned_store_impl(value, ptr);
}

template <alignable T, std::endian ByteOrder = std::endian::native>
    requires std::integral<T>
void unaligned_store(T value, void *ptr)
{
    if constexpr (ByteOrder != std::endian::native) {
        value = byteswap(value);
    }

    detail::unaligned_store_impl(value, ptr);
}

uint64_t unaligned_load_be64(const void *ptr)
{
    return unaligned_load<uint64_t, std::endian::big>(ptr);
}

uint64_t unaligned_load_le64(const void *ptr)
{
    return unaligned_load<uint64_t, std::endian::little>(ptr);
}

uint32_t unaligned_load_be32(const void *ptr)
{
    return unaligned_load<uint32_t, std::endian::big>(ptr);
}

uint32_t unaligned_load_le32(const void *ptr)
{
    return unaligned_load<uint32_t, std::endian::little>(ptr);
}

uint16_t unaligned_load_be16(const void *ptr)
{
    return unaligned_load<uint16_t, std::endian::big>(ptr);
}

uint16_t unaligned_load_le16(const void *ptr)
{
    return unaligned_load<uint16_t, std::endian::little>(ptr);
}

void unaligned_store_be64(uint64_t value, void *ptr)
{
    unaligned_store<uint64_t, std::endian::big>(value, ptr);
}

void unaligned_store_le64(uint64_t value, void *ptr)
{
    unaligned_store<uint64_t, std::endian::little>(value, ptr);
}

void unaligned_store_be32(uint32_t value, void *ptr)
{
    unaligned_store<uint32_t, std::endian::big>(value, ptr);
}

void unaligned_store_le32(uint32_t value, void *ptr)
{
    unaligned_store<uint32_t, std::endian::little>(value, ptr);
}

void unaligned_store_be16(uint16_t value, void *ptr)
{
    unaligned_store<uint16_t, std::endian::big>(value, ptr);
}

void unaligned_store_le16(uint16_t value, void *ptr)
{
    unaligned_store<uint16_t, std::endian::little>(value, ptr);
}

} // namespace nothing

#endif
