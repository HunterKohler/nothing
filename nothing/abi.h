/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 *
 * Itanium ABI wrappers.
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
#include <algorithm>
#include <cxxabi.h>
#include <nothing/memory.h>

namespace nothing {
namespace abi {

std::string demangle(const char *mangled)
{
    int status;
    stdlibc_unique_ptr<char[]> name(
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

    return name.get();
}

std::string demangle(const std::string &mangled)
{
    return demangle(mangled.c_str());
}

std::string demangle(const char *mangled, std::size_t size)
{
    return demangle(std::string(mangled, size));
}

std::string demangle(std::string_view mangled)
{
    return demangle(std::string(mangled));
}

} // namespace abi
} // namespace nothing

#endif
