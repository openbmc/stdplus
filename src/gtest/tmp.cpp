#include <fmt/format.h>

#include <stdplus/gtest/tmp.hpp>

#include <filesystem>

namespace stdplus
{
namespace gtest
{

TestWithTmp::TestWithTmp() :
    casedir(fmt::format(
        "{}/{}", SuiteTmpDir(),
        ::testing::UnitTest::GetInstance()->current_test_info()->name()))
{
    std::filesystem::create_directory(CaseTmpDir());
}

TestWithTmp::~TestWithTmp()
{
    std::filesystem::remove_all(CaseTmpDir());
}

void TestWithTmp::SetUpTestSuite()
{
    std::filesystem::create_directory(SuiteTmpDir());
}

void TestWithTmp::TearDownTestSuite()
{
    std::filesystem::remove_all(SuiteTmpDir());
}

std::string TestWithTmp::SuiteTmpDir()
{
    const char* dir = getenv("TMPDIR");
    if (dir == nullptr)
    {
        dir = "/tmp";
    }
    return fmt::format(
        "{}/{}-{}", dir,
        ::testing::UnitTest::GetInstance()->current_test_suite()->name(),
        getpid());
}

} // namespace gtest
} // namespace stdplus
