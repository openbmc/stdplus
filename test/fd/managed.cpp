#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/test/sys.hpp>

namespace stdplus
{
namespace fd
{
namespace
{

TEST(ManagedFd, Basic)
{
    ManagedFd fd(ops::open("Makefile.am", 0));
    printf("1: %d\n", fd.getVal());
    DupableFd fd2(ops::open("Makefile.am", 0));
    printf("2: %d\n", fd2.getVal());
    fd = fd2;
    printf("3: %d\n", fd.getVal());
    ops::fcntl_setfl(fd, O_NONBLOCK);

    std::string s;
    int amt = (int)ops::readAppend(fd, s, 10);
    printf("%d\n", amt);
    printf("%.*s\n", amt, s.data());
    printf("%s\n", ops::readAll(fd, 2).c_str());
    printf("%s\n", ops::readAll(fd).c_str());
}

TEST(ManagedFd, Mock)
{
    testing::StrictMock<test::SysMock> mock;
    ManagedFd fd(ops::open("Makefile.am", 0, &mock));
    ops::fcntl_setfl(fd, O_NONBLOCK);
}

} // namespace
} // namespace fd
} // namespace stdplus
