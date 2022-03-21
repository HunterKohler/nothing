#ifndef NOTHING_CHARCONV_H_
#define NOTHING_CHARCONV_H_

#include <system_error>
#include <charconv>

namespace nothing {
namespace detail {

template <class T>
auto check_chars_result(T result)
{
    auto code = std::make_error_code(result.ec);
    if (code) {
        throw std::system_error(code);
    }
    return result.ptr;
}

template <class... Args>
char *to_chars_checked(Args &&...args)
{
    return check_chars_result(std::to_chars(std::forward<Args>(args)...));
}

template <class... Args>
const char *from_chars_checked(Args &&...args)
{
    return check_chars_result(std::from_chars(std::forward<Args>(args)...));
}

} // namespace detail

using std::chars_format;

char *to_chars(char *first, char *last, short value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, int value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, long value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, long long value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, unsigned short value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, unsigned int value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, unsigned long value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, unsigned long long value, int base = 10)
{
    return detail::to_chars_checked(first, last, value, base);
}

char *to_chars(char *first, char *last, float value)
{
    return detail::to_chars_checked(first, last, value);
}

char *to_chars(char *first, char *last, double value)
{
    return detail::to_chars_checked(first, last, value);
}

char *to_chars(char *first, char *last, long double value)
{
    return detail::to_chars_checked(first, last, value);
}

char *to_chars(char *first, char *last, float value, chars_format fmt)
{
    return detail::to_chars_checked(first, last, value, fmt);
}

char *to_chars(char *first, char *last, double value, chars_format fmt)
{
    return detail::to_chars_checked(first, last, value, fmt);
}

char *to_chars(char *first, char *last, long double value, chars_format fmt)
{
    return detail::to_chars_checked(first, last, value, fmt);
}

char *to_chars(char *first, char *last, float value, chars_format fmt,
               int precision)
{
    return detail::to_chars_checked(first, last, value, fmt, precision);
}

char *to_chars(char *first, char *last, double value, chars_format fmt,
               int precision)
{
    return detail::to_chars_checked(first, last, value, fmt, precision);
}

char *to_chars(char *first, char *last, long double value, chars_format fmt,
               int precision)
{
    return detail::to_chars_checked(first, last, value, fmt, precision);
}

const char *from_chars(const char *first, const char *last, short &value,
                       int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last, int &value,
                       int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last, long &value,
                       int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last, long long &value,
                       int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last,
                       unsigned short &value, int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last, unsigned int &value,
                       int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last,
                       unsigned long &value, int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last,
                       unsigned long long &value, int base = 10)
{
    return detail::from_chars_checked(first, last, value, base);
}

const char *from_chars(const char *first, const char *last, float &value,
                       chars_format fmt = std::chars_format::general)
{
    return detail::from_chars_checked(first, last, value, fmt);
}

const char *from_chars(const char *first, const char *last, double &value,
                       chars_format fmt = std::chars_format::general)
{
    return detail::from_chars_checked(first, last, value, fmt);
}

const char *from_chars(const char *first, const char *last, long double &value,
                       chars_format fmt = std::chars_format::general)
{
    return detail::from_chars_checked(first, last, value, fmt);
}

} // namespace nothing

#endif
