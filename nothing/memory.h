/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_MEMORY_H_
#define NOTHING_MEMORY_H_

#include <memory>

namespace nothing {

/*
 * Wrapper for a deleting function `Delete` to allow empty-base-class
 * optimization with `unique_ptr`.
 */
template <class T, auto Delete>
struct empty_deleter {
    template <class U>
    using enable_if_convertible =
        std::enable_if_t<std::is_convertible_v<U *, T *>>;

  public:
    constexpr empty_deleter() noexcept = default;

    template <class U, enable_if_convertible<U> * = nullptr>
    constexpr empty_deleter(const empty_deleter<U, Delete> &) noexcept
    {}

    constexpr void operator()(T *ptr) noexcept(noexcept(Delete(ptr)))
    {
        Delete(ptr);
    }
};

/*
 * Partial specialization of `empty_deleter` for array types.
 */
template <class T, auto Delete>
struct empty_deleter<T[], Delete> {
    template <class U>
    using enable_if_convertible =
        std::enable_if_t<std::is_convertible_v<U (*)[], T (*)[]>>;

  public:
    constexpr empty_deleter() noexcept = default;

    template <class U, enable_if_convertible<U> * = nullptr>
    constexpr empty_deleter(const empty_deleter<U, Delete> &d) noexcept
    {}

    template <class U>
    constexpr enable_if_convertible<U>
    operator()(U *ptr) noexcept(noexcept(Delete(ptr)))
    {
        Delete(ptr);
    }
};

/*
 * Deleter C standard-library allocated memory.
 */
template <class T>
using stdlibc_deleter = empty_deleter<T, std::free>;

/*
 * Unique pointer for C standard-library allocated memory.
 */
template <class T>
using stdlibc_unique_ptr = std::unique_ptr<T, stdlibc_deleter<T>>;

}; // namespace nothing

#endif
