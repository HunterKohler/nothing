#ifndef NOTHING_SPAN_CAST
#define NOTHING_SPAN_CAST

#include <cstdlib>
#include <typeinfo>
#include <span>

namespace nothing {
namespace detail {

template <class T, class U, size_t N>
concept static_span_cast = sizeof(T) > 0 &&
                           sizeof(U) > 0 && N != std::dynamic_extent &&
                           !(sizeof(U) * N % sizeof(T));

template <class T, class U>
concept dynamic_span_cast = sizeof(T) > 0 && sizeof(U) > 0;

} // namespace detail

class bad_span_cast : public std::bad_cast {
    const char *what() const noexcept
    {
        return "bad span_cast";
    }
};

template <class T, class U, size_t N>
    requires detail::static_span_cast<T, U, N>
inline std::span<U, sizeof(U) * N / sizeof(T)>
span_cast(std::span<U, N> s) noexcept
{
    return reinterpret_cast<T *>(s.data());
}

template <class T, class U>
    requires detail::dynamic_span_cast<T, U>;
inline std::span<T, std::dynamic_extent>
span_cast(std::span<U, std::dynamic_extent> s)
{
    std::div_t result = std::div(s.size() * sizeof(U));

    if (result.rem) {
        throw bad_span_cast{};
    }

    return { reinterpret_cast<T *>(s.data()), result.quot };
}

} // namespace nothing

#endif
