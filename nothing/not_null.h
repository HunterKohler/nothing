/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_NOT_NULL_H_
#define NOTHING_NOT_NULL_H_

#include <memory>
#include <ostream>
#include <nothing/type_traits.h>

namespace nothing {

template <class T>
class not_null {
  public:
    static_assert(!std::is_same_v<T, std::nullptr_t>, "Must not be nullptr");
    static_assert(is_equality_comparable_with_v<T, std::nullptr_t>,
                  "Must be equality comparable to nullptr");

    using pointer = typename std::pointer_traits<T>::pointer;
    using element_type = typename std::pointer_traits<T>::element_type;

    template <class U, std::enable_if_t<std::is_convertible_v<U, pointer>>>
    explicit constexpr not_null(U &&u) : _ptr{ std::forward<U>(u) }
    {
        _check();
    }

    explicit constexpr not_null(pointer ptr) : _ptr{ std::move(ptr) }
    {
        _check();
    }

    template <class U, std::enable_if_t<std::is_convertible_v<U, pointer>>>
    constexpr not_null(const not_null<U> &other) : not_null(other.get())
    {}

    constexpr not_null(not_null &&other) = default;
    constexpr not_null(const not_null &other) = default;
    constexpr not_null &operator=(not_null &&other) = default;
    constexpr not_null &operator=(const not_null &other) = default;

    constexpr not_null(std::nullptr_t other) = delete;
    constexpr not_null &operator=(std::nullptr_t other) = delete;

    constexpr operator pointer() const noexcept
    {
        return get();
    }

    constexpr pointer get() const noexcept
    {
        return _ptr;
    }

    constexpr pointer operator->() const noexcept
    {
        return get();
    }

    constexpr std::add_lvalue_reference_t<element_type> operator*() const
        noexcept(noexcept(*std::declval<pointer>()))
    {
        return *get();
    }

    friend void swap(not_null &lh, not_null &rh)
    {
        using std::swap;
        swap(lh._ptr, rh._ptr);
    }

    not_null &operator++() = delete;
    not_null &operator--() = delete;
    not_null operator++(int) = delete;
    not_null operator--(int) = delete;
    not_null &operator+=(std::ptrdiff_t) = delete;
    not_null &operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) = delete;

  private:
    pointer _ptr;

    void _check()
    {
        if (_ptr == nullptr) {
            throw std::invalid_argument(
                "nothing::not_null::not_null: value must not be nullptr");
        }
    }
};

template <class T>
not_null<T> operator-(const not_null<T> &, std::ptrdiff_t) = delete;

template <class T>
not_null<T> operator+(const not_null<T> &, std::ptrdiff_t) = delete;

template <class T>
not_null<T> operator+(std::ptrdiff_t, const not_null<T> &) = delete;

template <class T, class U>
std::ptrdiff_t operator-(const not_null<T> &lh, const not_null<U> &rh)
{
    return lh.get() - rh.get();
}

}; // namespace nothing

template <class CharT, class Traits, class T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const nothing::not_null<T> &value)
{
    os << value.get();
    return os;
}

namespace std {

template <class T>
struct hash<nothing::not_null<T>> {
    std::size_t operator()(const nothing::not_null<T> &value)
    {
        return hash<T>{}(value.get());
    }
};

} // namespace std

#endif
