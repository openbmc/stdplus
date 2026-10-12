#include <stdplus/fd/dupable.hpp>

#include <system_error>

#include <gtest/gtest.h>

namespace stdplus::fd
{

TEST(DupableFd, InvalidFd)
{
    const int bad_fd = -1;
    EXPECT_THROW(DupableFd{bad_fd}, std::system_error);
    EXPECT_THROW(DupableFd(-1), std::system_error);
}

} // namespace stdplus::fd
