#include <exception>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <stdplus/future/future.hpp>
#include <stdplus/future/promise.hpp>

namespace stdplus
{
namespace future
{
namespace
{

TEST(Future, SetValueNoFuture)
{
    Promise<int> p;
    EXPECT_THROW(p.set_value(1), std::logic_error);
}

TEST(Future, SetValueDeletedFuture)
{
    Promise<int> p;
    {
        auto f = p.get_future();
        EXPECT_FALSE(f.is_ready());
        EXPECT_THROW(f.get(), std::bad_optional_access);
    }
    p.set_value(1);
}

TEST(Future, BasicOperation)
{
    Promise<int> p;
    size_t calls = 0;
    auto f = p.get_future();
    f.set_wakeup([&](Future<int>&) { calls++; });
    EXPECT_FALSE(f.is_ready());
    p.set_exception(std::make_exception_ptr(std::runtime_error("test")));
    EXPECT_THROW(p.set_value(1), std::logic_error);
    EXPECT_TRUE(f.is_ready());
    EXPECT_THROW(f.get(), std::runtime_error);
    EXPECT_EQ(1, calls);
}

TEST(Future, NoValueOutlivesPromise)
{
    auto p = std::make_unique<Promise<int>>();
    size_t calls = 0;
    auto f = p->get_future();
    f.set_wakeup([&](Future<int>&) { calls++; });
    EXPECT_FALSE(f.is_ready());
    EXPECT_THROW(f.get(), std::bad_optional_access);
    p.reset();
    EXPECT_TRUE(f.is_ready());
    EXPECT_THROW(f.get(), std::logic_error);
    EXPECT_EQ(1, calls);
}

TEST(Future, ValueOutlivesPromise)
{
    auto p = std::make_unique<Promise<int>>();
    size_t calls = 0;
    auto f = p->get_future();
    f.set_wakeup([&](Future<int>&) { calls++; });
    EXPECT_FALSE(f.is_ready());
    EXPECT_THROW(f.get(), std::bad_optional_access);
    p->set_value(1);
    p.reset();
    EXPECT_TRUE(f.is_ready());
    EXPECT_EQ(1, f.get());
    EXPECT_EQ(1, calls);
}

TEST(Future, MoveConstruct)
{
    auto p = std::make_unique<Promise<int>>();
    auto f = std::make_unique<Future<int>>(p->get_future());
    EXPECT_FALSE(f->is_ready());
    Promise<int> p2(std::move(*p));
    p.reset();
    Future<int> f2(std::move(*f));
    f.reset();
    EXPECT_FALSE(f2.is_ready());
    p2.set_value(1);
    EXPECT_TRUE(f2.is_ready());
    EXPECT_EQ(1, f2.get());
    Future<int> f3(std::move(f2));
    EXPECT_TRUE(f3.is_ready());
    EXPECT_EQ(1, f3.get());
}

TEST(Future, MoveAssign)
{
    auto p = std::make_unique<Promise<int>>();
    Promise<int> p2;
    auto f = std::make_unique<Future<int>>(p->get_future());
    auto f2 = Promise<int>().get_future();

    // Ensure our f2 / p2 aren't connected
    EXPECT_FALSE(f->is_ready());
    EXPECT_TRUE(f2.is_ready());
    EXPECT_THROW(f2.get(), std::logic_error);
    EXPECT_THROW(p2.set_value(2), std::logic_error);
    EXPECT_FALSE(f->is_ready());
    EXPECT_TRUE(f2.is_ready());
    EXPECT_THROW(f2.get(), std::logic_error);

    // Move our connected f / p into f2 / p2
    f2 = std::move(*f);
    f.reset();
    p2 = std::move(*p);
    p.reset();

    // Verify overwrite / new connection
    EXPECT_FALSE(f2.is_ready());
    p2.set_value(1);
    EXPECT_TRUE(f2.is_ready());
    EXPECT_EQ(1, f2.get());

    // Verify that moves copy the value in a future
    auto f3 = Promise<int>().get_future();
    f3 = std::move(f2);
    EXPECT_TRUE(f3.is_ready());
    EXPECT_EQ(1, f3.get());
}

} // namespace
} // namespace future
} // namespace stdplus
