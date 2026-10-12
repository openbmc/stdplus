#include <fcntl.h>
#include <unistd.h>

#include <stdplus/fd/dupable.hpp>

#include <system_error>
#include <utility>

#include <gtest/gtest.h>

namespace stdplus::fd
{

TEST(DupableFd, InvalidFd)
{
    const int bad_fd = -1;
    EXPECT_THROW(DupableFd{bad_fd}, std::system_error);
    EXPECT_THROW(DupableFd(-1), std::system_error);
}

TEST(DupableFd, DupLowestAvailable)
{
    int high_fd = fcntl(STDIN_FILENO, F_DUPFD_CLOEXEC, 100);
    ASSERT_GE(high_fd, 100);
    DupableFd orig(std::move(high_fd));
    DupableFd copy(orig);
    EXPECT_LT(copy.get(), 100);
}

} // namespace stdplus::fd
