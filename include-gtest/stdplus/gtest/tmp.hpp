#include <string>

#include <gtest/gtest.h>

namespace stdplus
{
namespace gtest
{

/**
 * @brief Provides googletest users with automatic temp directory
 *        handling per test case. You will still want to wrap your
 *        unit test execution in a `TMPDIR` RAII script in case the
 *        unit test binary ends up crashing.
 */
class TestWithTmp : public ::testing::Test
{
  protected:
    TestWithTmp();
    ~TestWithTmp();
    static void SetUpTestSuite();
    static void TearDownTestSuite();

    static std::string SuiteTmpDir();
    inline const std::string& CaseTmpDir() const
    {
        return casedir;
    }

  private:
    std::string casedir;
};

} // namespace gtest
} // namespace stdplus
