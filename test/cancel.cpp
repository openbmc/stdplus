#include <stdplus/cancel.hpp>

#include <functional>

#include <gtest/gtest.h>

namespace stdplus
{

struct FakeCancelable : public Cancelable
{
    size_t count = 0;
    void cancel() noexcept override
    {
        count++;
    }
};

TEST(CancelTest, Cancel)
{
    FakeCancelable c;
    EXPECT_EQ(c.count, 0);
    {
        Cancel cancel(&c);
        EXPECT_TRUE(cancel);
        Cancel cancel2(std::move(cancel));
        EXPECT_FALSE(cancel);
        EXPECT_TRUE(cancel2);
        EXPECT_EQ(c.count, 0);
    }
    EXPECT_EQ(c.count, 1);
}

TEST(CancelTest, EmptyCancel)
{
    Cancel cancel, cancel2(std::nullopt);
    EXPECT_FALSE(cancel);
    EXPECT_FALSE(cancel2);
}

TEST(CancelTest, AlwaysCallOnceLambda)
{
    size_t ctr = 0;
    {
        auto i = std::make_unique<size_t>(1);
        auto acb = alwaysCallOnce([&, i = std::move(i)]() { ctr += *i; });
        EXPECT_EQ(ctr, 0);
        acb();
        EXPECT_EQ(ctr, 1);
    }
    EXPECT_EQ(ctr, 1);
    {
        auto acb = alwaysCallOnce([&](size_t i) { ctr += i; }, 3);
        EXPECT_EQ(ctr, 1);
    }
    EXPECT_EQ(ctr, 4);
}

TEST(CancelTest, AlwaysCallOnceFunction)
{
    size_t ctr = 0;
    {
        auto acb = alwaysCallOnce(std::function<void()>(nullptr));
    }
    {
        auto acb = alwaysCallOnce(std::function<void()>([&]() { ctr++; }));
        EXPECT_EQ(ctr, 0);
        acb = alwaysCallOnce(std::function<void()>([&]() { ctr += 2; }));
        EXPECT_EQ(ctr, 1);
        auto& self = acb;
        acb = std::move(self);
        EXPECT_EQ(ctr, 1);
    }
    EXPECT_EQ(ctr, 3);
    {
        auto acb = alwaysCallOnce([&]() {
            ctr += 10;
            throw std::runtime_error("fail");
        });
        EXPECT_THROW(acb(), std::runtime_error);
        EXPECT_EQ(ctr, 13);
    }
    EXPECT_EQ(ctr, 13);
}

} // namespace stdplus
