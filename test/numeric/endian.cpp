#include <stdplus/numeric/endian.hpp>

#include <array>
#include <cstring>

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

TEST(EndianPacked, Uint8)
{
    uint8_unt n = 0;
    EXPECT_EQ(n.value(), 0);
    EXPECT_EQ(n, 0);

    n = 15;
    EXPECT_EQ(n.value(), 15);
    EXPECT_EQ(n, 15);
}

TEST(EndianPacked, Uint32)
{
    uint32_unt n = 0;
    EXPECT_EQ(n.value(), 0);
    EXPECT_EQ(n, 0);

    n = 15;
    EXPECT_EQ(n.value(), 15);
    EXPECT_EQ(n, 15);
}

TEST(EndianPacked, ValidateUnderlying)
{
    uint32_ubt b(15);
    uint32_ult l = {};
    EXPECT_EQ(b, 15);
    EXPECT_EQ(l, 0);
    std::memcpy(&l, &b, sizeof(l));
    EXPECT_EQ(0x0f000000, l);
}

} // namespace stdplus
