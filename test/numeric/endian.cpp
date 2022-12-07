#include <stdplus/numeric/endian.hpp>

#include <array>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(Byteswap, Swap)
{
    EXPECT_EQ(38, bswap(uint8_t{38}));
    EXPECT_EQ(38 << 8, bswap(uint16_t{38}));
    EXPECT_EQ(0x240082fe, bswap(uint32_t{0xfe820024}));
    EXPECT_EQ(0x240082fe00000000, bswap(uint64_t{0xfe820024}));
    struct
    {
        std::array<char, 4> a = {1, 2, 3, 4};
    } s;
    EXPECT_EQ((std::array<char, 4>{4, 3, 2, 1}), bswap(s).a);
    EXPECT_EQ(40, ntoh(hton(40)));
}

} // namespace stdplus
