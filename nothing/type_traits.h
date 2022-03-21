#include <type_traits>

namespace nothing {

template <std::size_t, class... Args>
struct nth_element;

template <std::size_t N>
struct nth_element<N> {};

template <class T, class... Args>
struct nth_element<0, T, Args...> : std::type_identity<T> {};

template <std::size_t N, class T, class... Args>
struct nth_element<N, T, Args...> : nth_element<N - 1, Args...> {};

template <std::size_t N, class... Args>
using nth_element_t = typename nth_element<N, Args...>::type;

namespace detail {

template <class T, class U, class = void>
struct is_explicitly_convertible_to_impl : std::false_type {};

template <class T, class U>
struct is_explicitly_convertible_to_impl<
    T, U, std::void_t<decltype(static_cast<U>(std::declval<T>()))>>
    : std::true_type {};

} // namespace detail

template <class T, class U>
struct is_explicitly_convertible_to
    : detail::is_explicitly_convertible_to_impl<T, U> {};

template <class T, class U>
inline constexpr bool is_explicitly_convertible_to_v =
    is_explicitly_convertible_to<T, U>::value;
} // namespace nothing
