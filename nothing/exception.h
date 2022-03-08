/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_EXCEPTION_H_
#define NOTHING_EXCEPTION_H_

#include <iostream>
#include <stdexcept>

namespace nothing {
namespace internal {

template <class Exception, std::size_t N = 100>
[[gnu::format(printf, 1, 2)]] constexpr void throw_formatted(const char *fmt...)
{
    char buf[N];
    va_list ap;

    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    throw Exception(buf);
}

} // namespace internal
} // namespace nothing

#endif
