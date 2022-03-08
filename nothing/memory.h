/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_MEMORY_H_
#define NOTHING_MEMORY_H_

#include <memory>
#include <concepts>

namespace nothing {

/*
 * Wraps a function pointer, or reference, `Delete` as an empty base struct to
 * be passed with 'no' overhead to `unique_ptr` classes and similar.
 */
template <auto Delete>
struct empty_base_deleter {
    constexpr empty_base_deleter() noexcept = default;

    template <class T>
    constexpr void operator()(T *ptr) noexcept(noexcept(Delete(ptr)))
    {
        Delete(ptr);
    }
};

/*
 * Smart pointer deleter for cstdlib allocated memory. Standard does not mandate
 * that `new` operators be implemented with cstdlib allocators. Template argument
 * `T` provides an arbitrary type constraint on the pointed-to type bein
 * passed freed.
 */
template <class T>
struct cstdlib_deleter {
    constexpr cstdlib_deleter() noexcept = default;

    template <class U>
        requires std::convertible_to<U *, T *>
    constexpr cstdlib_deleter(const cstdlib_deleter<U> &)
    {}

    constexpr void operator()(T *ptr) const
    {
        std::free(ptr);
    }
};

/*
 * Specialization for arrays.
 */
template <class T>
struct cstdlib_deleter<T[]> {
    constexpr cstdlib_deleter() noexcept = default;

    template <class U>
        requires std::convertible_to<U (*)[], T (*)[]>
    constexpr cstdlib_deleter(const cstdlib_deleter<U[]> &) noexcept {};

    template <class U>
        requires std::convertible_to<U (*)[], T (*)[]>
    constexpr void operator()(U *ptr) const
    {
        std::free(ptr);
    }
};

/*
 * Type alias for unique pointers using cstdlib allocation.
 */
template <class T>
using cstdlib_unique_ptr = std::unique_ptr<T, cstdlib_deleter<T>>;

}; // namespace nothing

#endif
