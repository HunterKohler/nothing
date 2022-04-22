/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_CONCEPTS_H_
#define NOTHING_CONCEPTS_H_

#include <tuple>
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
        requires(T &t, std::tuple_element_t<0, std::tuple<Args...>> &f) { T{f}; t = f; });

namespace detail {

template <class T>
using with_ref_t = T &;

} // namespace detail

template <class T>
concept referenceable = requires() { typename detail::with_ref_t<T>; };

// See exposition concept:
// https://en.cppreference.com/w/cpp/concepts/totally_ordered
template <class T>
concept partially_ordered = partially_ordered_with<T, T>;

template<class T, class U>
concept partially_ordered_with =
  requires(const std::remove_reference_t<T>& t,
           const std::remove_reference_t<U>& u) {
    { t <  u } -> boolean_testable;
    { t >  u } -> boolean_testable;
    { t <= u } -> boolean_testable;
    { t >= u } -> boolean_testable;
    { u <  t } -> boolean_testable;
    { u >  t } -> boolean_testable;
    { u <= t } -> boolean_testable;
    { u >= t } -> boolean_testable;
  };

} // namespace nothing


#endif
