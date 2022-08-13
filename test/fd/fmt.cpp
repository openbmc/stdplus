#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/gtest/tmp.hpp>
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

class FormatToFileTest : public gtest::TestWithTmp
{
  protected:
    std::string tmpname;
    std::unique_ptr<FormatToFile> file;

    FormatToFileTest() :
        tmpname(fmt::format("{}/tmp.XXXXXX", CaseTmpDir())),
        file(std::make_unique<FormatToFile>(tmpname.c_str()))
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
    EXPECT_EQ(0, std::filesystem::file_size(tmpname));
    auto filename = fmt::format("{}/out", CaseTmpDir());
    file->commit(filename);
    EXPECT_FALSE(std::filesystem::exists(tmpname));
    EXPECT_EQ(3, std::filesystem::file_size(filename));
}

} // namespace fd
} // namespace stdplus
