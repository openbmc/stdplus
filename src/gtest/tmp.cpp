#include <stdplus/gtest/tmp.hpp>

#include <filesystem>
#include <format>
#include <system_error>

namespace stdplus
{
namespace gtest
{

TestWithTmp::TestWithTmp() :
    casedir(std::format(
        "{}/{}", SuiteTmpDir(),
        ::testing::UnitTest::GetInstance()->current_test_info()->name()))
{
    std::filesystem::create_directories(CaseTmpDir());
}

TestWithTmp::~TestWithTmp()
{
    std::error_code ec;
    std::filesystem::remove_all(CaseTmpDir(), ec);
}

void TestWithTmp::SetUpTestSuite()
{
    std::filesystem::create_directories(SuiteTmpDir());
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
    return std::format(
        "{}/{}-{}", dir,
        ::testing::UnitTest::GetInstance()->current_test_suite()->name(),
        getpid());
}

} // namespace gtest
} // namespace stdplus
