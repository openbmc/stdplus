#include <sys/stat.h>
#include <unistd.h>

#include <stdplus/fd/atomic.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/gtest/tmp.hpp>

#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>

namespace stdplus
{
namespace fd
{

using std::literals::string_view_literals::operator""sv;

class AtomicWriterTest : public gtest::TestWithTmp
{
  protected:
    std::string filename;
    std::unique_ptr<AtomicWriter> file;

    AtomicWriterTest() : filename(std::format("{}/out", CaseTmpDir())) {}

    ~AtomicWriterTest() noexcept
    {
        if (file)
        {
            auto tmpname = file->getTmpname();
            file.reset();
            if (!tmpname.empty())
            {
                EXPECT_FALSE(std::filesystem::exists(tmpname));
            }
        }
    }
};

TEST_F(AtomicWriterTest, NoCommit)
{
    ASSERT_NO_THROW(file = std::make_unique<AtomicWriter>(filename, 0644));
    writeExact(*file, "hi\n"sv);
    EXPECT_FALSE(std::filesystem::exists(filename));
    EXPECT_TRUE(std::filesystem::exists(file->getTmpname()));
}

TEST_F(AtomicWriterTest, BadCommit)
{
    auto tmp = std::format("{}/tmp.XXXXXX", CaseTmpDir());
    ASSERT_NO_THROW(
        file = std::make_unique<AtomicWriter>("/dev/null", 0644, tmp));
    writeExact(*file, "hi\n"sv);
    EXPECT_TRUE(std::filesystem::exists(file->getTmpname()));
    EXPECT_THROW(file->commit(), std::filesystem::filesystem_error);
    EXPECT_EQ(file->getTmpname(), ""sv);
}

TEST_F(AtomicWriterTest, Basic)
{
    ASSERT_NO_THROW(file = std::make_unique<AtomicWriter>(filename, 0644));
    writeExact(*file, "hi\n"sv);
    EXPECT_TRUE(std::filesystem::exists(file->getTmpname()));
    EXPECT_EQ(std::filesystem::status(file->getTmpname()).permissions(),
              std::filesystem::perms::owner_read |
                  std::filesystem::perms::owner_write);
    EXPECT_NO_THROW(file->commit());
    EXPECT_THROW(file->commit(), std::system_error);
    EXPECT_EQ(file->getTmpname(), ""sv);
    std::error_code ec;
    EXPECT_EQ(3, std::filesystem::file_size(filename, ec));
    EXPECT_EQ(std::errc{}, ec);
    EXPECT_EQ(std::filesystem::status(filename).permissions(),
              std::filesystem::perms::owner_read |
                  std::filesystem::perms::owner_write |
                  std::filesystem::perms::group_read |
                  std::filesystem::perms::others_read);
}

TEST_F(AtomicWriterTest, MoveAssign)
{
    ASSERT_NO_THROW(file = std::make_unique<AtomicWriter>(filename, 0644));
    auto oldTmp = file->getTmpname();
    EXPECT_TRUE(std::filesystem::exists(oldTmp));

    AtomicWriter other(filename, 0644);
    auto newTmp = other.getTmpname();
    EXPECT_TRUE(std::filesystem::exists(newTmp));

    *file = std::move(other);
    EXPECT_FALSE(std::filesystem::exists(oldTmp));
    EXPECT_TRUE(std::filesystem::exists(newTmp));
    EXPECT_EQ(file->getTmpname(), newTmp);
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    EXPECT_EQ(other.getTmpname(), ""sv);
}

TEST_F(AtomicWriterTest, CrossDeviceCopy)
{
    struct stat st_case{}, st_shm{};
    if (stat(CaseTmpDir().c_str(), &st_case) != 0 ||
        stat("/dev/shm", &st_shm) != 0 || st_case.st_dev == st_shm.st_dev ||
        access("/dev/shm", W_OK) != 0)
    {
        GTEST_SKIP();
    }
    auto shm_tmpl = std::format("/dev/shm/stdplus-atomic-{}.XXXXXX", getpid());
    ASSERT_NO_THROW(
        file = std::make_unique<AtomicWriter>(filename, 0640, shm_tmpl));
    writeExact(*file, "hi\n"sv);
    auto tmp = file->getTmpname();
    EXPECT_TRUE(std::filesystem::exists(tmp));
    EXPECT_THROW(file->commit(false), std::filesystem::filesystem_error);
    EXPECT_FALSE(std::filesystem::exists(tmp));

    ASSERT_NO_THROW(
        file = std::make_unique<AtomicWriter>(filename, 0640, shm_tmpl));
    writeExact(*file, "cross\n"sv);
    tmp = file->getTmpname();
    // Replacing an existing destination must work across devices too.
    std::ofstream(filename) << "previous contents\n";
    EXPECT_NO_THROW(file->commit(true));
    EXPECT_FALSE(std::filesystem::exists(tmp));
    EXPECT_EQ(6u, std::filesystem::file_size(filename));
    std::string contents;
    std::getline(std::ifstream(filename), contents);
    EXPECT_EQ("cross", contents);
}

} // namespace fd
} // namespace stdplus
