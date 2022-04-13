/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 *
 * TODO: test constexpr with bitcast switch
 */
#ifndef NOTHING_UNALIGNED_H_
#define NOTHING_UNALIGNED_H_

#include <type_traits>

// clang-format off

namespace nothing {

template <class T>
concept alignable = alignof(T) > 1 &&
                    sizeof(T) > 1 && std::trivially_copyable_v<T>;

template <class T>
    requires alignable<T>
class unaligned {
  private:
    std::array<uint8_t, sizeof(T)> _value;

  public:
    using value_type = T;

    constexpr unaligned() noexcept = default;
    constexpr unaligned(const T &value) noexcept
    {
        store(value);
    }

    constexpr unaligned &operator=(const T &value) noexcept
    {
        store(value);
    }

    constexpr T load() const noexcept
    {
        return std::bitcast<T>(_value);
    }

    constexpr void store(const T &value) noexcept
    {
        _value = std::bitcast<decltype(_value)>(value);
    }
};

template <class T>
    requires alignable<T>
inline T unaligned_load(const T *ptr) noexcept
{
    return reinterpret_cast<const unaligned<T> *>(ptr)->load();
}

template <class T>
    requires alignable<T>
inline void unaligned_store(T *ptr, T value) noexcept
{
    reinterpret_cast<const unaligned<T> *>(ptr)->store(value);
}

} // namespace nothing

#endif
