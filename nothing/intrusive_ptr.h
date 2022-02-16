/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_INTRUSIVE_PTR_H_
#define NOTHING_INTRUSIVE_PTR_H_

#include <atomic>
#include "type_traits.h"

namespace nothing {

template <class>
class intrusive_ptr; // predeclare

struct thread_safe {};
struct thread_unsafe {};

template <class T, class Policy = thread_safe>
class intrusive_refcount {
  public:
    using policy = Policy;
    using element_type = T;

    constexpr intrusive_refcount() noexcept : _count{} {}
    constexpr intrusive_refcount(const intrusive_refcount &other) noexcept
        : _count{ other._count }
    {}

    constexpr intrusive_refcount &
    operator=(const intrusive_refcount &other) noexcept
    {
        _count = other._count;
        return *this;
    }

  protected:
    ~intrusive_refcount() = default;

  private:
    friend intrusive_ptr<element_type>;

    static_assert(is_some_v<policy, thread_safe, thread_unsafe>,
                  "unknown policy");

    using count_type =
        std::conditional_t<std::is_same_v<policy, thread_safe>,
                           std::atomic<std::size_t>, std::size_t>;

    count_type _count;

    constexpr void _ref() noexcept { _count++; }
    constexpr void _deref() noexcept
    {
        if (!--_count) {
            delete static_cast<element_type *>(this);
        }
    }
};

template <class T>
class intrusive_ptr {
  public:
    using element_type = T;

    constexpr intrusive_ptr() noexcept : _ptr{} {}
    constexpr intrusive_ptr(std::nullptr_t) noexcept : _ptr{} {}

    explicit constexpr intrusive_ptr(element_type *ptr) noexcept : _ptr{ ptr }
    {
        if (_ptr)
            _ref();
    }

    constexpr intrusive_ptr(const intrusive_ptr &other) noexcept
        : intrusive_ptr(other._ptr)
    {}

    constexpr intrusive_ptr(intrusive_ptr &&other) noexcept : _ptr{ other._ptr }
    {
        other._ptr = nullptr;
    }

    ~intrusive_ptr()
    {
        if (_ptr)
            _deref();
    }

    constexpr void reset() noexcept
    {
        if (_ptr) {
            _deref();
            _ptr = nullptr;
        }
    }

    constexpr void reset(std::nullptr_t) noexcept { reset(); }
    constexpr void reset(element_type *ptr) noexcept
    {
        if (_ptr != ptr) {
            if (_ptr) {
                _deref();
                _ptr = ptr;
                if (_ptr)
                    _ref();
            } else {
                _ptr = ptr;
                _ref();
            }
        }
    }

    constexpr void swap(intrusive_ptr &other) noexcept
    {
        std::swap(_ptr, other._ptr);
    }

    constexpr element_type *get() const noexcept { return _ptr; }

    constexpr intrusive_ptr &operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    constexpr intrusive_ptr &operator=(const intrusive_ptr &other)
    {
        reset(other._ptr);
        return *this;
    }

    constexpr intrusive_ptr &operator=(intrusive_ptr &&other) noexcept
    {
        if (_ptr != other._ptr) {
            if (_ptr)
                _deref();
            _ptr = other._ptr;
        }

        other._ptr = nullptr;
        return *this;
    }

    constexpr element_type &operator*() const noexcept { return *_ptr; }
    constexpr element_type *operator->() const noexcept { return _ptr; }
    constexpr element_type &operator[](std::ptrdiff_t index) const noexcept
    {
        return _ptr + index;
    }

    explicit constexpr operator bool() const noexcept { return _ptr; }

  private:
    static constexpr bool is_thread_safe =
        std::is_base_of_v<intrusive_refcount<element_type, thread_safe>,
                          element_type>;

    static constexpr bool is_thread_unsafe =
        std::is_base_of_v<intrusive_refcount<element_type, thread_unsafe>,
                          element_type>;

    static_assert(is_thread_safe || is_thread_unsafe,
                  "element type must inherit from intrusive_reference");

    using policy_type =
        std::conditional_t<is_thread_safe, thread_safe, thread_unsafe>;

    using refcount_type = intrusive_refcount<element_type, policy_type>;

    element_type *_ptr;

    constexpr void _deref() const { _refcount()._deref(); }
    constexpr void _ref() const { _refcount()._ref(); }

    constexpr refcount_type &_refcount() const noexcept
    {
        return static_cast<refcount_type &>(*_ptr);
    }
};

template <class Base, class Policy = thread_safe>
struct intrusive_wrapper
    : public Base,
      public intrusive_refcount<intrusive_wrapper<Base, Policy>, Policy> {
    template <class... Args>
    intrusive_wrapper(Args &&...args) : Base(std::forward<Args>(args)...)
    {}
};

template <class T, class... Args>
constexpr intrusive_ptr<T> make_intrusive(Args &&...args)
{
    return intrusive_ptr<T>(::new T(std::forward<Args>(args)...));
}

template <class T>
constexpr intrusive_ptr<T> make_intrusive_for_overwrite()
{
    return intrusive_ptr<T>(::new T);
}

} // namespace nothing

#endif
