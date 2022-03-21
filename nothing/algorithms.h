#include <ranges>
#include <algorithm>
#include <utility>
#include <iterator>
#include <concepts>

// clang-format off

namespace nothing {
namespace ranges {
namespace detail {

struct starts_with_fn {
    template <std::input_iterator I1, std::sentinel_for<I1> S1,
              std::input_iterator I2, std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to, class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
    constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2,
                              Pred pred = {}, Proj1 proj1 = {},
                              Proj2 proj2 = {}) const
    {
        return std::ranges::mismatch(std::move(first1), last1,
                                     std::move(first2), last2, pred, proj1,
                                     proj2)
                   .in2 == last2;
    }

    template <std::ranges::input_range R1, std::ranges::input_range R2,
              class Pred = std::ranges::equal_to, class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R1>,
                                            std::ranges::iterator_t<R2>, Pred,
                                            Proj1, Proj2>
    constexpr bool operator()(R1 &&r1, R2 &&r2, Pred pred = {},
                              Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(std::ranges::begin(r1), std::ranges::end(r1),
                          std::ranges::begin(r2), std::ranges::end(r2), pred,
                          proj1, proj2);
    }
};

struct ends_with_fn {
    template <std::input_iterator I1, std::sentinel_for<I1> S1,
              std::input_iterator I2, std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to, class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires (std::forward_iterator<I1> ||
                    std::sized_sentinel_for<S1, I1>) &&
                 (std::forward_iterator<I2> ||
                    std::sized_sentinel_for<S2, I2>) &&
                 std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
    constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2,
                              Pred pred = {},  Proj1 proj1 = {},
                              Proj2 proj2 = {}) const
    {
        const auto N1 = std::ranges::distance(first1, last1);
        const auto N2 = std::ranges::distance(first2, last2);

        if (N1 < N2) {
            return false;
        }

        std::ranges::advance(first1, N1 - N2);
        return std::ranges::equal(std::move(first1), last1, std::move(first2),
                                  last2, pred, proj1, proj2);
    }

    template <std::ranges::input_range R1, std::ranges::input_range R2,
              class Pred = std::ranges::equal_to, class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires (std::ranges::forward_range<R1> ||
                    std::ranges::sized_range<R1>) &&
                 (std::ranges::forward_range<R2> ||
                    std::ranges::sized_range<R2>) &&
                 std::indirectly_comparable<std::ranges::iterator_t<R1>,
                                            std::ranges::iterator_t<R2>, Pred,
                                            Proj1, Proj2>
    constexpr bool operator()(R1 &&r1, R2 &&r2, Pred pred = {},
                              Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(std::ranges::begin(r1), std::ranges::end(r1),
                          std::ranges::begin(r2), std::ranges::end(r2), pred,
                          proj1, proj2);
    }
};

} // namespace detail

// [alg.starts.with], starts with (C++23)
inline constexpr detail::starts_with_fn starts_with;

// [alg.ends.with], ends with (C++23)
inline constexpr detail::ends_with_fn ends_with;

} // namespace ranges
} // namespace nothing

// clang-format on
