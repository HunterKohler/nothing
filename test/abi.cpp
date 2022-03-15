#include <gtest/gtest.h>
#include <nothing/abi.h>

class NothingABI : testing::Test {};

TEST(NothingABI, Demangle)
{
    std::pair<std::string, std::string> input[] = {
        { "_ZN9wikipedia7article6formatE", "wikipedia::article::format" },
        { "_ZN6__pstl9execution2v1L5unseqE", "__pstl::execution::v1::unseq" },
        { "_ZN9__gnu_cxxL21__default_lock_policyE",
          "__gnu_cxx::__default_lock_policy" },
    };

    for (auto [mangled, normal]: input) {
        EXPECT_EQ(nothing::abi::demangle(mangled), normal);
        EXPECT_EQ(nothing::abi::demangle(mangled.c_str()), normal);
        EXPECT_EQ(nothing::abi::demangle(mangled.c_str(), mangled.size()),
                  normal);
        EXPECT_EQ(nothing::abi::demangle(std::string_view(mangled)), normal);
    }

    EXPECT_THROW(nothing::abi::demangle(nullptr), std::invalid_argument);
    EXPECT_THROW(nothing::abi::demangle("nothing::abi::demangle"),
                 std::invalid_argument);
}
