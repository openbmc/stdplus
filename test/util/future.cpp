#include <exception>
#include <future>
#include <gtest/gtest.h>
#include <stdexcept>
#include <stdplus/util/future.hpp>
#include <tuple>

namespace stdplus
{
namespace util
{
namespace
{

TEST(IsReady, NotReady)
{
    EXPECT_FALSE(isReady(std::promise<int>().get_future()));
}

TEST(IsReady, ValueReady)
{
    std::promise<int> p;
    p.set_value(1);
    EXPECT_TRUE(isReady(p.get_future()));
}

TEST(IsReady, ExceptionReady)
{
    std::promise<int> p;
    p.set_exception(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_TRUE(isReady(p.get_future()));
}

TEST(IsReady, VectorNotReady)
{
    std::promise<int> p;
    std::vector<std::future<int>> v;
    v.push_back(p.get_future());
    EXPECT_FALSE(isReady(v));
}

TEST(IsReady, VectorSomeReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    std::vector<std::future<int>> v;
    v.push_back(p1.get_future());
    v.push_back(p2.get_future());
    EXPECT_FALSE(isReady(v));
}

TEST(IsReady, VectorReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    p2.set_value(2);
    std::vector<std::future<int>> v;
    v.push_back(p1.get_future());
    v.push_back(p2.get_future());
    EXPECT_TRUE(isReady(v));
}

TEST(IsReady, None)
{
    EXPECT_TRUE(allReady());
}

TEST(AllReady, NoneReady)
{
    std::promise<int> p1, p2;
    EXPECT_FALSE(allReady(p1.get_future(), p2.get_future()));
}

TEST(AllReady, SomeReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    EXPECT_FALSE(allReady(p1.get_future(), p2.get_future()));
}

TEST(AllReady, AllReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    p2.set_value(2);
    EXPECT_TRUE(allReady(p1.get_future(), p2.get_future()));
}

TEST(IsReady, TupleNotReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    auto t = std::make_tuple(p1.get_future(), p2.get_future());
    EXPECT_FALSE(isReady(t));
}

TEST(IsReady, TupleReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    p2.set_value(2);
    auto t = std::make_tuple(p1.get_future(), p2.get_future());
    EXPECT_TRUE(isReady(t));
}

} // namespace
} // namespace util
} // namespace stdplus
