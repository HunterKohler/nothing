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
    requires(E e) {
        { e.what() } -> std::same_as<const char *>;
    };

// clang-format on

template <class T, class E, regular_exception Exception = std::exception>
struct result {
  public:
    using value_type = T;
    using error_type = E;
    using exception_type = Exception;

    [[no_unique_address]] T value;
    [[no_unique_address]] E error;

    // clang-format off

    explicit(!implicitly_constructible_from<T> ||
             !implicitly_constructible_from<E>)
    constexpr result()
        noexcept(std::is_nothrow_default_constructible_v<T> &&
                 std::is_nothrow_default_constructible_v<E>)
        requires(std::is_default_constructible_v<T> &&
                 std::is_default_constructible_v<E>)
        : value{}, error{}
    {}

    explicit(!std::is_convertible_v<const T &, T> ||
             !std::is_convertible_v<const E &, E>)
    constexpr result(const T &val, const E &err)
        noexcept(std::is_nothrow_copy_constructible_v<T> &&
                 std::is_nothrow_copy_constructible_v<E>)
        requires(std::is_copy_constructible_v<T> &&
                 std::is_copy_constructible_v<E>)
        : value{ val }, error{ err }
    {}

    template <class U = T, class V = E>
    explicit(!std::is_convertible_v<U, T> || !std::is_convertible_v<V, E>)
    constexpr result(U &&val, V &&err)
        noexcept(std::is_nothrow_constructible_v<T, U> &&
                 std::is_nothrow_constructible_v<V, E>)
        requires(std::is_constructible_v<T, U> &&
                 std::is_constructible_v<E, V>)
        : value{ std::forward<U>(val) }, error{ std::forward<V>(err) }
    {}

    template <class U, class V>
    explicit(!std::is_convertible_v<U &, T> ||
             !std::is_convertible_v<V &, E>)
    constexpr result(result<U, V> &other)
        noexcept(std::is_nothrow_constructible_v<T, U &> &&
                 std::is_nothrow_constructible_v<E, V &>)
        requires(std::is_constructible_v<T, U &> &&
                 std::is_constructible_v<E, V &>)
        : value{ other.value }, error{ other.error }
    {}

    template <class U, class V>
    explicit(!std::is_convertible_v<const U &, T> ||
             !std::is_convertible_v<const V &, E>)
    constexpr result(const result<U, V> &other)
        noexcept(std::is_nothrow_constructible_v<T, const U &> &&
                 std::is_nothrow_constructible_v<E, const V &>)
        requires(std::is_constructible_v<T, const U &> &&
                 std::is_constructible_v<E, const V &>)
        : value{ other.value }, error{ other.error }
    {}

    template <class U, class V>
    explicit(!std::is_convertible_v<U, T> || !std::is_convertible_v<V, E>)
    constexpr result(result<U, V> &&other)
        noexcept(std::is_nothrow_constructible_v<U, T> &&
                 std::is_nothrow_constructible_v<V, E>)
        requires(std::is_constructible_v<T, U> &&
                 std::is_constructible_v<E, V>)
        : value{ std::forward<U>(other.val) },
          error{ std::forward<V>(other.err) }
    {}

    template <class U, class V>
    explicit(!std::is_convertible_v<const U, T> ||
             !std::is_convertible_v<const V, E>)
    constexpr result(const result<U, V> &&other)
        noexcept(std::is_nothrow_constructible_v<T, const U> &&
                 std::is_nothrow_constructible_v<E, const V>)
        requires(std::is_constructible_v<T, U> &&
                 std::is_constructible_v<E, V>)
        : value{ std::forward<const U>(other.val) },
          error{ std::forward<const V>(other.err) }
    {}

    template <class... Args1, class... Args2>
    constexpr result(std::piecewise_construct_t pc, std::tuple<Args1...> args1,
                     std::tuple<Args2...> args2)
        noexcept(std::is_nothrow_constructible_v<T, Args1...> &&
                 std::is_nothrow_constructible_v<E, Args2...>)
        : result(pc, args1, args2, std::index_sequence_for<Args1...>{},
                 std::index_sequence_for<Args2...>{})
    {}

    constexpr result(const result &) = default;
    constexpr result(result &&) = default;

    constexpr result &operator=(const result &other)
        noexcept(std::is_nothrow_copy_assignable_v<T> &&
                 std::is_nothrow_copy_assignable_v<E>)
        requires(std::is_copy_assignable_v<T> && std::is_copy_assignable_v<E>)
    {
        value = other.value;
        error = other.error;
        return *this;
    }

    constexpr const result &operator=(const result &other) const
        noexcept(std::is_nothrow_copy_assignable_v<const T> &&
                 std::is_nothrow_copy_assignable_v<const E>)
        requires(std::is_copy_assignable_v<const T> &&
                 std::is_copy_assignable_v<const E>)
    {
        value = other.value;
        error = other.error;
        return *this;
    }

    template <class U, class V>
    constexpr result &operator=(const result<U, V> &other)
        noexcept(std::is_nothrow_assignable_v<T &, const U &> &&
                 std::is_nothrow_assignable_v<E &, const V &>)
        requires(std::is_assignable_v<T &, const U &> &&
                 std::is_assignable_v<E &, const V &>)
    {
        value = other.value;
        error = other.error;
        return *this;
    }

    template <class U, class V>
    constexpr const result &operator=(const result<U, V> &other) const
        noexcept(std::is_nothrow_assignable_v<const T &, const U &> &&
                 std::is_nothrow_assignable_v<const E &, const V &>)
        requires(std::is_assignable_v<const T &, const U &> &&
                 std::is_assignable_v<const E &, const V &>)
    {
        value = other.value;
        error = other.error;
        return *this;
    }

    constexpr result &operator=(result &&other)
        noexcept(std::is_nothrow_move_assignable_v<T> &&
                 std::is_nothrow_move_assignable_v<E>)
        requires(std::is_move_assignable_v<T> && std::is_move_assignable_v<E>)
    {
        value = std::move(other.value);
        error = std::move(other.error);
        return *this;
    }

    constexpr const result &operator=(result &&other) const
        noexcept(std::is_nothrow_assignable_v<const T &, T> &&
                 std::is_nothrow_assignable_v<const E &, E>)
        requires(std::is_assignable_v<const T &, T> &&
                 std::is_assignable_v<const E &, E>)
    {
        value = std::move(other.value);
        error = std::move(other.error);
        return *this;
    }

    template <class U, class V>
    constexpr result &operator=(result<U, V> &&other)
        noexcept(std::is_nothrow_assignable_v<T &, U> &&
                 std::is_nothrow_assignable_v<E &, V>)
        requires(std::is_assignable_v<T &, U> && std::is_assignable_v<E &, V>)
    {
        value = std::forward<U>(other.value);
        error = std::forward<V>(other.error);
        return *this;
    }

    template <class U, class V>
    constexpr const result &operator=(result<U, V> &&other) const
        noexcept(std::is_nothrow_assignable_v<const T &, U> &&
                 std::is_nothrow_assignable_v<const E &, V>)
        requires(std::is_assignable_v<const T &, U> &&
                 std::is_assignable_v<const E &, V>)
    {
        value = std::forward<U>(other.value);
        error = std::forward<V>(other.error);
        return *this;
    }

    friend constexpr void swap(result &a, result &b)
        noexcept(std::is_nothrow_swappable_v<T> &&
                 std::is_nothrow_swappable_v<E>)
        requires(std::is_swappable_v<T> && std::is_swappable_v<E>)
    {
        using std::swap;
        swap(a.value, b.value);
        swap(a.error, b.error);
    }

    friend constexpr void swap(const result &a, const result &b)
        noexcept(std::is_nothrow_swappable_v<const T> &&
                 std::is_nothrow_swappable_v<const E>)
        requires(std::is_swappable_v<const T> && std::is_swappable_v<const E>)
    {
        using std::swap;
        swap(a.value, b.value);
        swap(a.error, b.error);
    }

    constexpr bool ok() const noexcept(noexcept(static_cast<bool>(error)))
    {
        return static_cast<bool>(error);
    }

    constexpr operator bool() const noexcept { return ok(); }
    constexpr operator std::pair<T, E>() const noexcept
    {
        return { value, error };
    }

    constexpr operator Exception() const noexcept
    {
        if constexpr (std::constructible_from<Exception, E>) {
            return { error };
        } else {
            return {};
        }
    }

  private:
    template <class... Args1, class... Args2, std::size_t... I1,
              std::size_t... I2>
    constexpr result(std::piecewise_construct_t, std::tuple<Args1...> args1,
                     std::tuple<Args2...> args2,
                     std::index_sequence<I1...>,
                     std::index_sequence<I2...>)
        noexcept(std::is_nothrow_constructible_v<T, Args1...> &&
                 std::is_nothrow_constructible_v<E, Args2...>)
        : value{ std::forward<Args1>(std::get<I1>(args1))... },
          error{ std::forward<Args2>(std::get<I2>(args2))... }
    {}
};

// clang-format on

template <class T, regular_exception Exception = std::exception>
struct binary_result : result<T, bool, Exception> {};

template <class T>
struct system_result : result<T, std::errc, std::system_error> {
    constexpr operator std::system_error() const noexcept
    {
        return { std::error_code{ this->error } };
    }
};

template <class Exception, std::size_t N = 100>
    requires std::derived_from<Exception, std::exception> &&
        std::constructible_from<Exception, const char *>
[[noreturn]] //
[[gnu::format(printf, 1, 2)]] //
void throw_fmt(const char *fmt, ...)
{
    char buf[N];

    if (fmt) {
        va_list ap;
        va_start(ap, fmt);

        std::vsnprintf(buf, N, fmt, ap);

        va_end(ap);
    } else {
        buf[0] = 0;
    }

    throw Exception(buf);
}

void check_errno()
{}

// void check_errno(int value)
// {}

void check_errno(std::errc)
{}

} // namespace nothing
