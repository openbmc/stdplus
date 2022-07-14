#include <filesystem>
#include <gtest/gtest.h>
#include <stdplus/gtest/tmp.hpp>

namespace stdplus
{
namespace gtest
{

class TestWithTmpTest : public TestWithTmp
{
};

TEST_F(TestWithTmpTest, One)
{
    EXPECT_TRUE(std::filesystem::create_directory(
        std::filesystem::path(CaseTmpDir()) / "a"));
    EXPECT_TRUE(std::filesystem::create_directory(
        std::filesystem::path(SuiteTmpDir()) / "a"));
}

TEST_F(TestWithTmpTest, Two)
{
    EXPECT_TRUE(std::filesystem::create_directory(
        std::filesystem::path(CaseTmpDir()) / "a"));
    EXPECT_FALSE(std::filesystem::create_directory(
        std::filesystem::path(SuiteTmpDir()) / "a"));
}

class TestWithTmpTest2 : public TestWithTmp
{
};

TEST_F(TestWithTmpTest2, One)
{
    EXPECT_TRUE(std::filesystem::create_directory(
        std::filesystem::path(SuiteTmpDir()) / "a"));
    EXPECT_TRUE(std::filesystem::create_directory(
        std::filesystem::path(CaseTmpDir()) / "a"));
}

} // namespace gtest
} // namespace stdplus
