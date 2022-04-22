#include <type_traits>
#include <concepts>
#include <system_error>
#include <exception>
#include <stdexcept>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>
#include <nothing/concepts.h>
#include <nothing/utility.h>

namespace nothing {

// clang-format off

template <class E>
concept regular_exception = std::semiregular<E> &&
    std::derived_from<E, std::exception> &&
    requires(const E &e) {
        { e.what() } noexcept -> std::same_as<const char *>;
    };

// clang-format on

inline void check_errno(std::errc e)
{
    auto error = std::make_error_code(e);
    if (error) {
        throw std::system_error(error);
    }
}

inline void check_errno(int e)
{
    check_errno(std::errc(e));
}

inline void check_errno()
{
    check_errno(errno);
}

} // namespace nothing
