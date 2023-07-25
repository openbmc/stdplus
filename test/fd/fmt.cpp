#include <fmt/compile.h>
#include <sys/mman.h>

#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>

#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace stdplus
{
namespace fd
{

using std::literals::string_view_literals::operator""sv;

TEST(FormatBuffer, Basic)
{
    auto fd = ManagedFd(CHECK_ERRNO(memfd_create("test", 0), "memfd_create"));
    {
        FormatBuffer buf(fd, 4096);
        buf.appends("hi\n", "hi\n"sv);
        EXPECT_EQ(0, fd.lseek(0, Whence::Cur));
        buf.flush();

        EXPECT_EQ(6, fd.lseek(0, Whence::Cur));
        buf.append(FMT_COMPILE("{}"), std::string(2050, 'a'));
        EXPECT_EQ(6, fd.lseek(0, Whence::Cur));
        buf.append("{}", std::string(2050, 'a'));
        EXPECT_EQ(4106, fd.lseek(0, Whence::Cur));

        buf.appendsv("hi\n");
        EXPECT_EQ(4106, fd.lseek(0, Whence::Cur));
    }
    EXPECT_EQ(4109, fd.lseek(0, Whence::Cur));
}

} // namespace fd
} // namespace stdplus
