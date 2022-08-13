#include <gtest/gtest.h>

#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>
#include <sys/mman.h>

namespace stdplus
{
namespace fd
{

TEST(FormatBuffer, Basic)
{
    auto fd = ManagedFd(CHECK_ERRNO(memfd_create("test", 0), "memfd_create"));
    {
        FormatBuffer buf(fd, 4096);
        buf.append("hi\n");
        EXPECT_EQ(0, fd.lseek(0, Whence::Cur));
        buf.flush();

        EXPECT_EQ(3, fd.lseek(0, Whence::Cur));
        buf.append("{}", std::string(2050, 'a'));
        EXPECT_EQ(3, fd.lseek(0, Whence::Cur));
        buf.append("{}", std::string(2050, 'a'));
        EXPECT_EQ(4103, fd.lseek(0, Whence::Cur));

        buf.append("hi\n");
        EXPECT_EQ(4103, fd.lseek(0, Whence::Cur));
    }
    EXPECT_EQ(4106, fd.lseek(0, Whence::Cur));
}

} // namespace fd
} // namespace stdplus
