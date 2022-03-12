/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_TYPE_TRAITS_H_
#define NOTHING_TYPE_TRAITS_H_

#include <type_traits>

namespace nothing {

template <class T, class... U>
struct is_some : std::disjunction<std::is_same<T, U>...> {};

template <class T>
struct is_boolean_testable
    : std::conjunction<
          std::is_convertible<T, bool>,
          std::is_convertible<decltype(!std::declval<T>()), bool>> {};

template <class T>
inline constexpr bool is_boolean_testable_v = is_boolean_testable<T>::value;

namespace detail {

template <class T, class U, class = void>
struct is_weakly_equality_comparable_with : std::false_type {};

template <class T, class U>
struct is_weakly_equality_comparable_with<
    T, U,
    std::enable_if_t<std::conjunction_v<
        is_boolean_testable<
            decltype(std::declval<const std::remove_reference_t<T> &>() ==
                     std::declval<const std::remove_reference_t<U> &>())>,
        is_boolean_testable<
            decltype(std::declval<const std::remove_reference_t<T> &>() !=
                     std::declval<const std::remove_reference_t<U> &>())>,
        is_boolean_testable<
            decltype(std::declval<const std::remove_reference_t<U> &>() ==
                     std::declval<const std::remove_reference_t<T> &>())>,
        is_boolean_testable<
            decltype(std::declval<const std::remove_reference_t<U> &>() !=
                     std::declval<const std::remove_reference_t<T> &>())>>>>
    : std::true_type {};

}; // namespace detail

template <class T>
struct is_equality_comparable
    : detail::is_weakly_equality_comparable_with<T, T> {};

template <class T>
inline constexpr bool is_equality_comparable_v =
    is_equality_comparable<T>::value;

template <class T, class U>
struct is_equality_comparable_with
    : std::conjunction<is_equality_comparable<T>, is_equality_comparable<U>,
                       detail::is_weakly_equality_comparable_with<T, U>
                       // std::common_reference_with<
                       // const std::remove_reference_t<T>&,
                       // const std::remove_reference_t<U>&> &&
                       //   is_equality_comparable<
                       //     std::common_reference_t<
                       //       const std::remove_reference_t<T>&,
                       //       const std::remove_reference_t<U>&>
                       //       >
                       > {};

template <class T, class U>
inline constexpr bool is_equality_comparable_with_v =
    is_equality_comparable_with<T, U>::value;

template <class T, class U>
struct is_same_sign : std::conjunction<std::is_signed<T>, std::is_signed<U>> {
    static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>,
                  "Types must be arithmetic to compare signedness");
};

template <class T, class U>
inline constexpr bool is_same_sign_v = is_same_sign<T, U>::value;

template <class T, class U>
struct is_mutually_convertible
    : std::conjunction<std::is_convertible<T, U>, std::is_convertible<U, T>> {};

template <class T, class U>
inline constexpr bool is_mutually_convertible_v =
    is_mutually_convertible<T, U>::value;

} // namespace nothing

#endif
