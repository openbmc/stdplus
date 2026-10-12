#include <fmt/compile.h>
#include <sys/mman.h>

#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>

#include <string>
#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

namespace stdplus
{
namespace fd
{

using std::literals::string_view_literals::operator""sv;

static_assert(!std::is_copy_constructible_v<FormatBuffer>);
static_assert(std::is_move_constructible_v<FormatBuffer>);
static_assert(!std::is_copy_assignable_v<FormatBuffer>);
static_assert(!std::is_move_assignable_v<FormatBuffer>);

TEST(FormatBuffer, Basic)
{
    auto fd = ManagedFd(CHECK_ERRNO(memfd_create("test", 0), "memfd_create"));
    {
        FormatBuffer buf(fd, 4096);
        buf.appends("hi\n", "hi\n"sv);
        EXPECT_EQ(0, fd.lseek(0, Whence::Cur));
        buf.flush();

        EXPECT_EQ(6, fd.lseek(0, Whence::Cur));
        buf.append("{}", std::string(2050, 'a'));
        EXPECT_EQ(6, fd.lseek(0, Whence::Cur));
        buf.append("{}", std::string(2050, 'a'));
        EXPECT_EQ(4106, fd.lseek(0, Whence::Cur));

        buf.appends("hi\n");
        EXPECT_EQ(4106, fd.lseek(0, Whence::Cur));
    }
    EXPECT_EQ(4109, fd.lseek(0, Whence::Cur));
}

TEST(FormatBuffer, UnwindException)
{
    ManagedFd fd;
    EXPECT_THROW(
        {
            FormatBuffer buf(fd, 4096);
            buf.appends("hi\n");
            throw std::runtime_error("unwind");
        },
        std::runtime_error);
}

} // namespace fd
} // namespace stdplus
