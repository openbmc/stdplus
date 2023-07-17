#include <stdplus/gtest/tmp.hpp>

#include <filesystem>
#include <format>

namespace stdplus
{
namespace gtest
{

TestWithTmp::TestWithTmp() :
    casedir(std::format(
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
    return std::format(
        "{}/{}-{}", dir,
        ::testing::UnitTest::GetInstance()->current_test_suite()->name(),
        getpid());
}

} // namespace gtest
} // namespace stdplus
