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
template <class T, std::invocable<T *> auto Delete>
struct empty_deleter {
    constexpr empty_deleter() noexcept = default;

    template <class U>
        requires std::convertible_to<U *, T *>
    constexpr empty_deleter(const empty_deleter<U, Delete> &) noexcept {}

    constexpr void operator()(T *ptr) const noexcept(noexcept(Delete(ptr)))
    {
        Delete(ptr);
    }
};

/*
 * Specialization of `empty_deleter` for array types.
 */
template <class T, std::invocable<T *> auto Delete>
struct empty_deleter<T[], Delete> {
    constexpr empty_deleter() noexcept = default;

    template <class U>
        requires std::convertible_to<U (*)[], T (*)[]>
    constexpr empty_deleter(const empty_deleter<U[], Delete> &) noexcept {}

    template <class U>
        requires std::convertible_to<U (*)[], T (*)[]> &&
            std::invocable<decltype(Delete), U *>
    constexpr void operator()(U *ptr) const noexcept(noexcept(Delete(ptr)))
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
