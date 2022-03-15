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

namespace nothing {
namespace abi {

std::string demangle(const char *mangled);
std::string demangle(const std::string &mangled);
std::string demangle(const char *mangled, std::size_t size);
std::string demangle(std::string_view mangled);

} // namespace abi
} // namespace nothing

#endif
