/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_CONCEPTS_H_
#define NOTHING_CONCEPTS_H_

#include <concepts>
#include <iterator>
#include <nothing/type_traits.h>

// clang-format off

namespace nothing {

template <class B>
concept boolean_testable = std::convertible_to<B, bool> &&
    requires(B &&b) { { !std::forward<B>(b) } -> std::convertible_to<bool>; };

template <class T>
concept arithmetic = std::is_arithmetic_v<T>;

template <class T, class... Args>
concept implicitly_constructible_from =
    std::constructible_from<T, Args...> &&
    (sizeof...(Args) == 0 && requires(T &t) { t = {}; }) &&
    (sizeof...(Args) == 1 &&
        requires(T &t, nth_element_t<0, Args...> &f) { T{f}; t = f; });

template <class T, class U>
concept narrowable_to =
    requires(T t, U u) { static_cast<U>(t); static_cast<T>(u); } &&
    ((!arithmetic<T> && !arithmetic<U>) ||
     (arithmetic<T> && arithmetic<U> &&
      std::totally_ordered<T> && std::totally_ordered<U> &&
      std::is_default_constructible_v<T> &&
      std::is_default_constructible_v<U>));

template <class T>
concept trivially_copyable = std::is_trivially_copyable_v<T>;

template <class T, class U>
concept explicitly_convertible_to = requires(T t) { static_cast<U>(t); };

} // namespace nothing

#endif
