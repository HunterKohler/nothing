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
#include <nothing/memory.h>

namespace nothing {
namespace abi {

inline std::string demangle(std::string_view view)
{
    int status;
    stdlibc_unique_ptr<char> name{ ::abi::__cxa_demangle(
        std::string(view).c_str(), nullptr, nullptr, &status) };

    switch (status) {
    case -1:
        throw std::bad_alloc();
    case -2:
    case -3:
        throw std::invalid_argument(
            "nothing::abi::demangle: Invalid mangled name");
    }

    return { name.get() };
}

} // namespace abi
} // namespace nothing

#endif
