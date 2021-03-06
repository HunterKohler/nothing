/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_MEMORY_H_
#define NOTHING_MEMORY_H_

#include <memory>
#include <nothing/concepts>

namespace nothing {

template <class Allocator, class T>
struct rebind_allocator {
    using type = typename std::allocator_traits<Allocator>::rebind_alloc<T>;
};

template <class Allocator, class T>
using rebind_allocator_t = typename rebind_allocator<Allocator, T>::type;

template <class Pointer, class T>
struct rebind_pointer {
    using type = typename std::pointer_traits<Pointer>::rebind<T>;
};

template <class Pointer, class T>
using rebind_pointer_t = typename rebind_pointer<Pointer, T>::type;

/*
 * Wrapper for a deleting function `Delete` to allow empty-base-class
 * optimization with `unique_ptr`.
 */
template <class T, std::invocable<T *> auto Delete>
struct empty_delete {
    constexpr empty_delete() noexcept = default;

    template <class U>
        requires std::convertible_to<U *, T *>
    constexpr empty_delete(const empty_delete<U, Delete> &) noexcept {}

    constexpr void operator()(T *ptr) const noexcept(noexcept(Delete(ptr)))
    {
        Delete(ptr);
    }
};

/*
 * Specialization of `empty_delete` for array types.
 */
template <class T, std::invocable<T *> auto Delete>
struct empty_delete<T[], Delete> {
    constexpr empty_delete() noexcept = default;

    template <class U>
        requires std::convertible_to<U (*)[], T (*)[]>
    constexpr empty_delete(const empty_delete<U[], Delete> &) noexcept {}

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
using stdlibc_delete = empty_delete<T, std::free>;

/*
 * Unique pointer for C standard-library allocated memory.
 */
template <class T>
using stdlibc_unique_ptr = std::unique_ptr<T, stdlibc_delete<T>>;

}; // namespace nothing

#endif
