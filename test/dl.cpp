#include <gtest/gtest.h>
#include <stdplus/dl.hpp>

namespace stdplus
{

TEST(Dl, FailedOpen)
{
    EXPECT_THROW(Dl("nodl.so", DlOpenFlags(DlOpenType::Now)),
                 std::runtime_error);
}

TEST(Dl, GetLinkMap)
{
    Dl dl(nullptr, DlOpenFlags(DlOpenType::Now)
                       .set(DlOpenFlag::Global)
                       .set(DlOpenFlag::NoLoad));
    EXPECT_NE(nullptr, dl.linkMap());
}

} // namespace stdplus
