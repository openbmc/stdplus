#include <function2/function2.hpp>
#include <stdplus/function_view.hpp>

#include <functional>

#include <gtest/gtest.h>

namespace stdplus
{

int add(int a, int b)
{
    return a + b;
}

int mul(int a, int b) noexcept
{
    return a * b;
}

TEST(FunctionView, Ptr)
{
    function_view<int(int, int)> fv = add;
    EXPECT_EQ(3, fv(1, 2));
    EXPECT_EQ(14, fv(5, 9));
    fv = mul;
    EXPECT_EQ(14, fv(7, 2));
    EXPECT_EQ(0, fv(0, 10));
    function_view fv2 = add;
    EXPECT_EQ(1, fv2(1, 0));
}

TEST(FunctionView, Lambda)
{
    auto addl = [](int a, int b) { return a + b; };
    function_view<int(int, int)> fv = addl;
    EXPECT_EQ(3, fv(1, 2));
    EXPECT_EQ(14, fv(5, 9));
    function_view fv2 = addl;
    EXPECT_EQ(1, fv2(1, 0));
    fv = fv2;
    EXPECT_EQ(1, fv(1, 0));
}

TEST(FunctionView, StdFunction)
{
    std::function<int(int, int)> addf = add;
    function_view<int(int, int)> fv = addf;
    EXPECT_EQ(3, fv(1, 2));
    EXPECT_EQ(14, fv(5, 9));
    {
        function_view fv2 = addf;
        EXPECT_EQ(1, fv2(1, 0));
        fv = fv2;
    }
    EXPECT_EQ(6, fv(3, 3));
}

} // namespace stdplus
