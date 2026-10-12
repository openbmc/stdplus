#include <stdplus/exception.hpp>

#include <gtest/gtest.h>

namespace stdplus
{
namespace exception
{

TEST(Exception, IgnoreNoError)
{
    ignore([] {})();
    ignore([]() mutable { throw std::runtime_error("Boom"); })();
    EXPECT_EQ(int(), ignore([]() -> int { throw 1; })());
    auto x = std::make_unique<int>(1);
    auto y = std::make_unique<int>(2);
    auto z = std::make_unique<int>(3);
    EXPECT_EQ(3, ignore([x = std::move(x)](auto&& v) { return *v + *x; })(y));
    EXPECT_EQ(5, ignore([z = std::move(z)](auto v) {
                  return *v + *z;
              })(std::move(y)));

    std::string s = "hello";
    auto lval = [s]() { return s.size(); };
    EXPECT_EQ(5u, ignore(lval)());
    EXPECT_EQ(5u, lval());
}

TEST(Exception, IgnoreQuiet)
{
    ignoreQuiet([] {})();
    ignoreQuiet([]() mutable { throw std::runtime_error("Boom"); })();

    std::string s = "world";
    auto lval = [s]() { return s.size(); };
    EXPECT_EQ(5u, ignoreQuiet(lval)());
    EXPECT_EQ(5u, lval());
}

} // namespace exception
} // namespace stdplus
