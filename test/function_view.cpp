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

struct Funcy
{
    int a = 0;

    int operator()(float y) noexcept
    {
        return a += (int)y;
    }

    int operator()(int b) noexcept
    {
        return a += b;
    }
};

struct FuncyPriv : private Funcy
{
    using Funcy::operator();
};

int callFv(stdplus::function_view<int(int, int)> fv)
{
    return fv(3, 5);
}

TEST(FunctionView, Ptr)
{
    function_view<int(int, int)> fv = add;
    EXPECT_TRUE(fv);
    EXPECT_EQ(3, fv(1, 2));
    EXPECT_EQ(14, fv(5, 9));
    fv = mul;
    EXPECT_EQ(14, fv(7, 2));
    EXPECT_EQ(0, fv(0, 10));
    function_view fv2 = add;
    EXPECT_EQ(1, fv2(1, 0));

    fv = nullptr;
    EXPECT_FALSE(fv);

    EXPECT_EQ(8, callFv(add));
}

TEST(FunctionView, Obj)
{
    Funcy f;
    function_view<int(int)> fv = f;
    EXPECT_EQ(2, fv(2));
    EXPECT_EQ(5, fv(3));

    FuncyPriv fp;
    fv = fp;
    EXPECT_EQ(2, fv(2));
    EXPECT_EQ(5, fv(3));
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
    function_view<int(int, int) const> fv3 = addl;
    EXPECT_EQ(1, fv3(1, 0));

    auto mull = [old = 1](int a) mutable { return old *= a; };
    function_view fvm = mull;
    EXPECT_EQ(2, fvm(2));
    EXPECT_EQ(4, fvm(2));

    auto mula = [](auto a, auto b) { return a + b; };
    fv = mula;
    EXPECT_EQ(5, fv(3, 2));

    EXPECT_EQ(8, callFv([](auto a, auto b) { return a + b; }));
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

    std::move_only_function<int(int, int) const noexcept> mulf = mul;
    function_view<int(int, int) const> fv2 = mulf;
    EXPECT_EQ(2, fv2(1, 2));

    std::move_only_function<int(int)> mulfa = [old = 1](auto a) mutable {
        return old *= a;
    };
    function_view<int(int)> fva = mulfa;
    EXPECT_EQ(3, fva(3));
    EXPECT_EQ(9, fva(3));
}

} // namespace stdplus
