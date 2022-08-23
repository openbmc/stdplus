#include <gtest/gtest.h>

#include <fmt/compile.h>
#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>
#include <sys/mman.h>

#include <string>
#include <string_view>

namespace stdplus
{
namespace fd
{

using fmt::operator""_cf;
using std::literals::string_view_literals::operator""sv;

TEST(FormatBuffer, Basic)
{
    auto fd = ManagedFd(CHECK_ERRNO(memfd_create("test", 0), "memfd_create"));
    {
        FormatBuffer buf(fd, 4096);
        buf.append("hi\n");
        buf.append("hi\n"sv);
        EXPECT_EQ(0, fd.lseek(0, Whence::Cur));
        buf.flush();

        EXPECT_EQ(6, fd.lseek(0, Whence::Cur));
        buf.append(FMT_COMPILE("{}"), std::string(2050, 'a'));
        EXPECT_EQ(6, fd.lseek(0, Whence::Cur));
        buf.append("{}"_cf, std::string(2050, 'a'));
        EXPECT_EQ(4106, fd.lseek(0, Whence::Cur));

        buf.append(FMT_STRING("hi\n"));
        EXPECT_EQ(4106, fd.lseek(0, Whence::Cur));
    }
    EXPECT_EQ(4109, fd.lseek(0, Whence::Cur));
}

} // namespace fd
} // namespace stdplus
