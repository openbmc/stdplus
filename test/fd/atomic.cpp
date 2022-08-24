#include <filesystem>
#include <memory>
#include <stdplus/fd/atomic.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/gtest/tmp.hpp>
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

    AtomicWriterTest() : filename(fmt::format("{}/out", CaseTmpDir()))
    {
    }

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
    detail::tmpfile_support = true;
    ASSERT_NO_THROW(file = std::make_unique<AtomicWriter>(filename, 0644));
    writeExact(*file, "hi\n"sv);
    EXPECT_FALSE(std::filesystem::exists(filename));
    const auto& tmpname = file->getTmpname();
    if (!tmpname.empty())
    {
        EXPECT_TRUE(std::filesystem::exists(tmpname));
    }
}

TEST_F(AtomicWriterTest, NoCommitNoTmpfile)
{
    detail::tmpfile_support = false;
    ASSERT_NO_THROW(file = std::make_unique<AtomicWriter>(filename, 0644));
    writeExact(*file, "hi\n"sv);
    EXPECT_FALSE(std::filesystem::exists(filename));
    EXPECT_TRUE(std::filesystem::exists(file->getTmpname()));
}

TEST_F(AtomicWriterTest, BadCommit)
{
    detail::tmpfile_support = true;
    auto tmp = fmt::format("{}/tmp.XXXXXX", CaseTmpDir());
    ASSERT_NO_THROW(file =
                        std::make_unique<AtomicWriter>("/dev/null", 0644, tmp));
    writeExact(*file, "hi\n"sv);
    EXPECT_THROW(file->commit(), std::filesystem::filesystem_error);
    EXPECT_EQ(file->getTmpname(), ""sv);
}

TEST_F(AtomicWriterTest, BadCommitNoTmpfile)
{
    detail::tmpfile_support = false;
    auto tmp = fmt::format("{}/tmp.XXXXXX", CaseTmpDir());
    ASSERT_NO_THROW(file =
                        std::make_unique<AtomicWriter>("/dev/null", 0644, tmp));
    writeExact(*file, "hi\n"sv);
    EXPECT_TRUE(std::filesystem::exists(file->getTmpname()));
    EXPECT_THROW(file->commit(), std::filesystem::filesystem_error);
    EXPECT_EQ(file->getTmpname(), ""sv);
}

TEST_F(AtomicWriterTest, Basic)
{
    detail::tmpfile_support = true;
    ASSERT_NO_THROW(file = std::make_unique<AtomicWriter>(filename, 0644));
    writeExact(*file, "hi\n"sv);
    EXPECT_NO_THROW(file->commit());
    EXPECT_EQ(file->getTmpname(), ""sv);
    std::error_code ec;
    EXPECT_EQ(3, std::filesystem::file_size(filename, ec));
    EXPECT_EQ(std::errc{}, ec);
}

TEST_F(AtomicWriterTest, BasicNoTmpfile)
{
    detail::tmpfile_support = false;
    ASSERT_NO_THROW(file = std::make_unique<AtomicWriter>(filename, 0644));
    writeExact(*file, "hi\n"sv);
    EXPECT_TRUE(std::filesystem::exists(file->getTmpname()));
    EXPECT_NO_THROW(file->commit());
    EXPECT_EQ(file->getTmpname(), ""sv);
    std::error_code ec;
    EXPECT_EQ(3, std::filesystem::file_size(filename, ec));
    EXPECT_EQ(std::errc{}, ec);
}

} // namespace fd
} // namespace stdplus
