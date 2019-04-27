#include <exception>
#include <future>
#include <gtest/gtest.h>
#include <stdexcept>
#include <stdplus/util/future.hpp>

namespace stdplus
{
namespace util
{
namespace
{

class IsReady : public testing::Test
{
  protected:
    std::promise<int> p1, p2, p3;
    std::future<int> f1 = p1.get_future();
    std::future<int> f2 = p2.get_future();
    std::future<int> f3 = p3.get_future();
};

TEST_F(IsReady, Nothing)
{
    EXPECT_TRUE(areAllReady());
}

TEST_F(IsReady, NoneReady)
{
    EXPECT_FALSE(areAllReady(f1, f2, f3));
}

TEST_F(IsReady, SomeReady)
{
    p1.set_value(1);
    p3.set_exception(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_FALSE(areAllReady(f1, f2, f3));
    EXPECT_FALSE(areAllReady(f2, f3, f1));
}

TEST_F(IsReady, AllReady)
{
    p1.set_value(1);
    p2.set_value(2);
    p3.set_value(3);
    EXPECT_TRUE(areAllReady(f1, f2, f3));
}

} // namespace
} // namespace util
} // namespace stdplus
