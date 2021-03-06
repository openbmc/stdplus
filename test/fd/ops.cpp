#include <gtest/gtest.h>
#include <stdplus/fd/ops.hpp>

namespace stdplus
{
namespace fd
{

TEST(Flags, Flags)
{
    FdFlags f = FdFlags(0).set(FdFlag::CloseOnExec).unset(FdFlag::CloseOnExec);
    f.set(FdFlag::CloseOnExec).unset(FdFlag::CloseOnExec);
    EXPECT_EQ(0, static_cast<int>(f));
}

} // namespace fd
} // namespace stdplus
