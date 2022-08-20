#include <gtest/gtest.h>

#include <filesystem>
#include <fmt/compile.h>
#include <memory>
#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/gtest/tmp.hpp>
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

class FormatToFileTest : public gtest::TestWithTmp
{
  protected:
    std::string tmpname;
    std::unique_ptr<FormatToFile> file;

    FormatToFileTest() :
        tmpname(fmt::format("{}/tmp.XXXXXX", CaseTmpDir())),
        file(std::make_unique<FormatToFile>(tmpname))
    {
        tmpname = file->getTmpname();
        EXPECT_TRUE(std::filesystem::exists(tmpname));
    }

    ~FormatToFileTest() noexcept(true)
    {
        file.reset();
        EXPECT_FALSE(std::filesystem::exists(tmpname));
    }
};

TEST_F(FormatToFileTest, NoCommit)
{
    file->append("hi\n");
    EXPECT_EQ(0, std::filesystem::file_size(tmpname));
}

TEST_F(FormatToFileTest, Basic)
{
    file->append("hi\n");
    file->append("hi\n"sv);
    file->append(FMT_STRING("hi\n"));
    file->append(FMT_COMPILE("hi\n"));
    EXPECT_EQ(0, std::filesystem::file_size(tmpname));
    auto filename = fmt::format("{}/out", CaseTmpDir());
    file->commit(filename);
    EXPECT_FALSE(std::filesystem::exists(tmpname));
    EXPECT_EQ(12, std::filesystem::file_size(filename));
}

} // namespace fd
} // namespace stdplus
