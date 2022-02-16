/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_TYPE_TRAITS_H_
#define NOTHING_TYPE_TRAITS_H_

#include <type_traits>

namespace nothing {

template <class T, class... U>
struct is_some : std::bool_constant<std::disjunction_v<std::is_same<T, U>...>> {
};

template <class T, class... U>
constexpr bool is_some_v = is_some<T, U...>::value;

template <class Alloc, class T>
using rebind_alloc = typename std::allocator_traits<Alloc>::rebind_alloc<T>;

} // namespace nothing

#endif
