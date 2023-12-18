#include "util.hpp"

#include <stdplus/exception.hpp>
#include <stdplus/fd/gmock.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/numeric/endian.hpp>

#include <gtest/gtest.h>

namespace stdplus::fd
{

using testing::_;
using testing::Ge;
using testing::SizeIs;
using std::literals::string_view_literals::operator""sv;

TEST(Flags, Flags)
{
    FdFlags f = FdFlags(0).set(FdFlag::CloseOnExec).unset(FdFlag::CloseOnExec);
    f.set(FdFlag::CloseOnExec).unset(FdFlag::CloseOnExec);
    EXPECT_EQ(0, static_cast<int>(f));
}

TEST(Connect, Success)
{
    testing::StrictMock<FdMock> fd;
    SockAddrBuf buf;
    EXPECT_CALL(fd, connect(_)).WillOnce([&](std::span<const std::byte> addr) {
        std::copy(addr.begin(), addr.end(), reinterpret_cast<std::byte*>(&buf));
        buf.len = addr.size();
    });
    Sock6Addr addr(In6Addr{0xff}, 365, 0);
    connect(fd, addr);
    EXPECT_EQ(Sock6Addr::fromBuf(buf), addr);
}

TEST(ReadExact, Success)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(SizeIs(Ge(3)))).WillOnce(readSv("one"));
    }
    char buf[9];
    readExact(fd, buf);
    EXPECT_EQ(std::string_view(buf, sizeof(buf)), "alpha one");
}

TEST(ReadExact, NotEnoughEof)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    char buf[9];
    EXPECT_THROW(readExact(fd, buf), exception::Incomplete);
    EXPECT_THROW(readExact(fd, buf), exception::Eof);
}

TEST(ReadExact, NotEnoughBlock)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(_)).WillRepeatedly(readSv(""));
    }
    char buf[9];
    EXPECT_THROW(readExact(fd, buf), exception::Incomplete);
    EXPECT_THROW(readExact(fd, buf), exception::WouldBlock);
}

TEST(ReadExact, NotEnoughBlockInt)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\0\0\0"sv));
        EXPECT_CALL(fd, read(_)).WillRepeatedly(readSv(""));
    }
    int32_t i;
    EXPECT_THROW(readExact(fd, i), exception::Incomplete);
    EXPECT_THROW(readExact(fd, i), exception::WouldBlock);
}

TEST(ReadExact, EnoughInt)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\0\0\0\0"sv));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\x01\x02"sv));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\x03\x04"sv));
    }
    int32_t i;
    readExact(fd, i);
    EXPECT_EQ(i, 0);
    readExact(fd, i);
    EXPECT_EQ(ntoh(i), 0x01020304);
}

TEST(Read, Success)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("one"));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv(""));
    }
    char buf[15];
    auto res = read(fd, buf);
    EXPECT_EQ(std::string_view(res.begin(), res.end()), "alph");
    res = read(fd, buf);
    EXPECT_EQ(std::string_view(res.begin(), res.end()), "one");
    EXPECT_TRUE(read(fd, buf).empty());
}

TEST(Read, NotEnoughInt)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\0\0\0"sv));
        EXPECT_CALL(fd, read(_)).WillRepeatedly(readSv(""));
    }
    std::array<int32_t, 1> i;
    EXPECT_THROW(read(fd, i), exception::Incomplete);
    EXPECT_TRUE(read(fd, i).empty());
}

TEST(Read, NotEnoughEofInt)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\0\0\0"sv));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<int32_t, 1> i;
    EXPECT_THROW(read(fd, i), exception::Incomplete);
    EXPECT_THROW(read(fd, i), exception::Eof);
}

TEST(Read, EnoughInt)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\0\0\0\0"sv));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\x01\x02"sv));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("\x03\x04"sv));
    }
    std::array<int32_t, 1> i;
    read(fd, i);
    EXPECT_EQ(i[0], 0);
    read(fd, i);
    EXPECT_EQ(stdplus::ntoh(i[0]), 0x01020304);
}

} // namespace stdplus::fd
