/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 *
 * Itanium ABI wrappers and general tomfoolery.
 *
 * Reference:
 * https://itanium-cxx-abi.github.io/cxx-abi/abi.html
 */
#ifndef NOTHING_ABI_H_
#define NOTHING_ABI_H_

#include <string>
#include <string_view>
#include <string_view>
#include <exception>
#include <stdexcept>
#include <cxxabi.h>
#include <nothing/memory.h>

namespace nothing {
namespace abi {

void demangle(const char *mangled, std::output_iterator<char> auto dest)
{
    int status;
    cstdlib_unique_ptr<char[]> name(
        ::abi::__cxa_demangle(mangled, nullptr, nullptr, &status));

    switch (status) {
    case -1:
        throw std::bad_alloc();
    case -2:
        throw std::invalid_argument(
            "nothing::abi::demangle: Invalid mangled name.");
    case -3:
        throw std::invalid_argument(
            "nothing::abi::demangle: Invalid arguments.");
    }

    for (int i = 0; name[i]; i++)
        *dest++ = name[i];
}

} // namespace abi
} // namespace nothing

#endif
