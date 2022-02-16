/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_CONCEPTS_H_
#define NOTHING_CONCEPTS_H_

// Clang format struggles with C++20 Concepts.
// clang-format off

#include <type_traits>
#include <concepts>
#include <iterator>

namespace nothing {

template <class B>
concept boolean_testable = std::convertible_to<B, bool> &&
    requires(B&& b) { { !std::forward<B>(b) } -> std::convertible_to<bool>; };

} // namespace nothing

#endif
