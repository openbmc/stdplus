#include <stdplus/fd/managed.hpp>

#include <system_error>

#include <gtest/gtest.h>

namespace stdplus::fd
{

TEST(ManagedFd, InvalidFd)
{
    EXPECT_THROW(ManagedFd(-1), std::system_error);
}

} // namespace stdplus::fd
