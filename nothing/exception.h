#ifndef NOTHING_EXCEPTION_H_
#define NOTHING_EXCEPTION_H_

#include <iostream>
#include <stdexcept>

namespace nothing {
namespace internal {

template <class Exception, std::size_t N = 100,
          class... Args>
void throw_formatted(const char *fmt, const Args &...args)
{
    char buf[N];
    std::snprintf(buf, sizeof(buf), args...);
    throw Exception(buf);
}

}
}

#endif
