/*
 * Copyright (C) 2021-2022 John Hunter Kohler <jhunterkohler@gmail.com>
 */
#ifndef NOTHING_IPV4_ADDRESS_H_
#define NOTHING_IPV4_ADDRESS_H_

#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <functional>
#include <iostream>
#include <charconv>
#include <memory_resource>
#include <nothing/unaligned.h>
#include <nothing/ascii.h>

namespace nothing {

class ipv4_address {
  private:
    using bytes_base = std::array<uint8_t, 4>;

  public:
    using uint_type = uint64_t;

    struct bytes_type : bytes_base {
        template <std::convertible_to<uint8_t>... Args>
        constexpr bytes_type(Args... args)
            : bytes_base{ static_cast<uint8_t>(std::forward<Args>(args))... }
        {
        }
    };

    static constexpr size_t string_size = 16;

    constexpr ipv4_address() noexcept : _data{} {}
    constexpr ipv4_address(const bytes_type &value) noexcept : _data{ value } {}
    constexpr ipv4_address(uint_type value) noexcept
        : _data{
              (value >> 24) & 0xFF,
              (value >> 16) & 0xFF,
              (value >> 8) & 0xFF,
              value & 0xFF,
          }
    {
    }

    constexpr auto to_uint() const noexcept
    {
        return (static_cast<uint_type>(_data[0]) << 24) |
               (static_cast<uint_type>(_data[1]) << 16) |
               (static_cast<uint_type>(_data[2]) << 8) |
               (static_cast<uint_type>(_data[3]));
    }

    template <class Allocator = std::allocator<char>>
    constexpr auto to_string(const Allocator &alloc = Allocator()) const
    {
        using string_type =
            std::basic_string<char, std::char_traits<char>, Allocator>;

        char buf[string_size];
        char *end = to_chars(buf);

        return string_type(buf, end, alloc);
    }

    template <std::output_iterator<char> Out>
    constexpr Out to_chars(Out dest) const
    {
        for (int i = 0; i < 3; i++) {
            dest = _to_chars_part(_data[i], dest);
            *dest = '.';
            ++dest;
        }

        dest = _to_chars_part(_data[3], dest);
        return dest;
    }

    constexpr bytes_type to_bytes() const noexcept { return _data; }

    constexpr bool is_loopback() const noexcept
    {
        return (to_uint() & 0xFF000000) == 0x7F000000; // 127.0.0.0/8
    }

    constexpr bool is_link_local() const noexcept
    {
        return (to_uint() & 0xFFFF0000) == 0xA9FE0000; // 169.254.0.0/16
    }

    constexpr bool is_private() const noexcept
    {
        uint32_t value = to_uint();
        return (value & 0xFF000000) == 0xA0000000 || // 10.0.0.0/8
               (value & 0xFFC00000) == 0x64400000 || // 100.64.0.0/10
               (value & 0xFFF00000) == 0xAC100000 || // 172.16.0.0/12
               (value & 0xFFFFFF00) == 0xC0000000 || // 192.0.0.0/24
               (value & 0xFFFF0000) == 0xC0A80000 || // 192.168.0.0/16
               (value & 0xFFFE0000) == 0xC6120000; // 198.18.0.0/15
    }

    constexpr bool is_multicast() const noexcept
    {
        return (to_uint() & 0xF0000000) == 0xE0000000; // 224.0.0.0/4
    }

    static constexpr ipv4_address any() noexcept { return ipv4_address{}; }
    static constexpr ipv4_address loopback() noexcept
    {
        return ipv4_address{ 0x7F000001 };
    }

    static constexpr ipv4_address broadcast() noexcept
    {
        return ipv4_address{ 0xFFFFFFFF };
    }

    static ipv4_address from_string(std::string_view src)
    {
        std::error_code error;
        ipv4_address addr = from_string(src, error);

        if (error) {
            throw std::system_error(error);
        }

        return addr;
    }

    static ipv4_address from_string(std::string_view src,
                                    std::string_view::iterator &last)
    {
        std::error_code error;
        ipv4_address addr = from_string(src, last, error);

        if (error) {
            throw std::system_error(error);
        }

        return addr;
    }

    static ipv4_address from_string(std::string_view src,
                                    std::error_code &error) noexcept
    {
        std::string_view::iterator last;
        ipv4_address addr = from_string(src, last, error);

        if (last != src.end()) {
            error = std::make_error_code(std::errc::invalid_argument);
        }

        return addr;
    }

    static ipv4_address from_string(std::string_view src,
                                    std::string_view::iterator &last,
                                    std::error_code &error) noexcept
    {
        std::string_view::iterator pos = src.begin();
        std::string_view::iterator end = src.end();
        bytes_type data;

        for (int i = 0; i < 4; i++) {
            int value;

            if (pos < end && ascii_isdigit(*pos)) {
                value = *pos++ - '0';

                if (pos < end && ascii_isdigit(*pos) && value) {
                    value = 10 * value + *pos++ - '0';

                    if (pos < end && ascii_isdigit(*pos)) {
                        int next = 10 * value + *pos - '0';

                        if (next < 256) {
                            value = next;
                            pos++;
                        }
                    }
                }
            } else {
                error = std::make_error_code(std::errc::invalid_argument);
                return {};
            }

            if (i < 3 && (pos == end || *pos++ != '.')) {
                error = std::make_error_code(std::errc::invalid_argument);
                return {};
            }

            data[i] = value;
        }

        last = pos;
        error.clear();

        return ipv4_address{ data };
    }

    constexpr auto operator<=>(const ipv4_address &other) const noexcept
    {
        return to_uint() <=> other.to_uint();
    }

  private:
    bytes_type _data;

    constexpr auto _to_chars_part(uint8_t value,
                                  std::output_iterator<char> auto dest) const
    {
        char buf[3];
        auto res = std::to_chars(buf, std::end(buf), value);
        return std::copy(buf, res.ptr, dest);
    }
};

template <class T, class CharT>
std::basic_ostream<T, CharT> &
operator<<(std::basic_ostream<T, CharT> &os, const ipv4_address &addr)
{
    addr.to_chars(std::ostream_iterator<char>{ os });
    return os;
}

} // namespace nothing

namespace std {

template <>
struct hash<nothing::ipv4_address> {
    constexpr std::size_t
    operator()(const nothing::ipv4_address &addr) const noexcept
    {
        return addr.to_uint();
    }
};

} // namespace std

#endif
