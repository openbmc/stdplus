#include <stdplus/cancel.hpp>

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
        EXPECT_EQ(c.count, 0);
    }
    EXPECT_EQ(c.count, 1);
}

} // namespace stdplus
