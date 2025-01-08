#include <stdplus/fd/create.hpp>
#include <stdplus/fd/mmap.hpp>

#include <array>
#include <cstddef>
#include <span>

#include <gtest/gtest.h>

namespace stdplus
{
namespace fd
{

TEST(MMap, Basic)
{
    auto fd = open("/dev/zero", OpenAccess::ReadOnly);
    auto map = MMap(fd, 32, ProtFlags().set(ProtFlag::Read),
                    MMapFlags{MMapAccess::Private}, 0);
    auto sp = map.get();
    ASSERT_NE(nullptr, sp.data());
    ASSERT_EQ(32, sp.size());
    for (size_t i = 0; i < 32; ++i)
    {
        EXPECT_EQ(sp[i], std::byte{});
    }
}

} // namespace fd
} // namespace stdplus
