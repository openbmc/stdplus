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

TEST_F(IsReady, NotReady)
{
    EXPECT_FALSE(isReady(std::promise<int>().get_future()));
}

TEST_F(IsReady, ValueReady)
{
    std::promise<int> p;
    p.set_value(1);
    EXPECT_TRUE(isReady(p.get_future()));
}

TEST_F(IsReady, ExceptionReady)
{
    std::promise<int> p;
    p.set_exception(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_TRUE(isReady(p.get_future()));
}

TEST_F(IsReady, VectorNotReady)
{
    std::promise<int> p;
    std::vector<std::future<int>> v;
    v.push_back(p.get_future());
    EXPECT_FALSE(isReady(v));
}

TEST_F(IsReady, VectorSomeReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    std::vector<std::future<int>> v;
    v.push_back(p1.get_future());
    v.push_back(p2.get_future());
    EXPECT_FALSE(isReady(v));
}

TEST_F(IsReady, VectorReady)
{
    std::promise<int> p1, p2;
    p1.set_value(1);
    p2.set_value(2);
    std::vector<std::future<int>> v;
    v.push_back(p1.get_future());
    v.push_back(p2.get_future());
    EXPECT_TRUE(isReady(v));
}

} // namespace
} // namespace util
} // namespace stdplus
