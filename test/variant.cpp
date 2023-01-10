#include <stdplus/variant.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

using namespace std::literals::string_literals;
using V = std::variant<int, float, std::string>;

TEST(variantEqFuzzy, Basic)
{
    EXPECT_TRUE(variantEqFuzzy(V{1}, 1));
    EXPECT_TRUE(variantEqFuzzy(V{1}, V{1}));
    EXPECT_TRUE(variantEqFuzzy(V{1}, 1.));
    EXPECT_TRUE(variantEqFuzzy(V{1}, V{1.f}));
    EXPECT_TRUE(variantEqFuzzy(V{1.f}, 1));
    EXPECT_TRUE(variantEqFuzzy(V{1.f}, 1.));
    EXPECT_TRUE(variantEqFuzzy(V{1.f}, 1.f));
    EXPECT_FALSE(variantEqFuzzy(V{"1"}, 1));
    EXPECT_FALSE(variantEqFuzzy(V{"1"}, 1.));
    EXPECT_TRUE(variantEqFuzzy(V{"1"}, "1"));
    EXPECT_TRUE(variantEqFuzzy(V{"1"}, V{"1"}));
    EXPECT_FALSE(variantEqFuzzy(V{"1"}, V{1}));
}

TEST(variantEqStrict, Basic)
{
    EXPECT_TRUE(variantEqStrict(V{1}, 1));
    EXPECT_TRUE(variantEqStrict(V{1}, V{1}));
    EXPECT_FALSE(variantEqStrict(V{1}, 1.f));
    EXPECT_FALSE(variantEqStrict(V{1}, V{1.f}));
    EXPECT_FALSE(variantEqStrict(V{1.f}, 1));
    EXPECT_TRUE(variantEqStrict(V{1.f}, 1.f));
    EXPECT_FALSE(variantEqStrict(V{"1"}, 1));
    EXPECT_FALSE(variantEqStrict(V{"1"}, 1.f));
    EXPECT_TRUE(variantEqStrict(V{"1"}, "1"s));
    EXPECT_TRUE(variantEqStrict(V{"1"}, V{"1"}));
}

} // namespace stdplus
