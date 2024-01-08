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

TEST(ReadAll, SuccessEmpty)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    EXPECT_EQ(readAll<std::string>(fd), "");
}

TEST(ReadAll, Success)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(SizeIs(Ge(3)))).WillOnce(readSv("one"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    EXPECT_EQ(readAll<std::string>(fd), "alpha one");
}

TEST(ReadAll, Block)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(_)).WillRepeatedly(readSv(""));
    }
    EXPECT_THROW(readAll<std::string>(fd), exception::WouldBlock);
}

TEST(ReadAll, NotEnough)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(5))))
            .WillOnce(readSv("\x01\x02\x03\x04\x05"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x06"));
        EXPECT_CALL(fd, read(_)).WillRepeatedly(readSv(""));
    }
    EXPECT_THROW(readAll<std::vector<int32_t>>(fd), exception::Incomplete);
}

TEST(ReadAll, NotEnoughEof)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(5))))
            .WillOnce(readSv("\x01\x02\x03\x04\x05"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x06"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    EXPECT_THROW(readAll<std::vector<int32_t>>(fd), exception::Incomplete);
}

TEST(ReadAll, EnoughInt)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(5))))
            .WillOnce(readSv("\x01\x02\x03\x04\x05"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x06"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("\x07\x08"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    EXPECT_EQ(readAll<std::vector<int32_t>>(fd),
              (std::vector{stdplus::hton(int32_t{0x01020304}),
                           stdplus::hton(int32_t{0x05060708})}));
}

TEST(ReadAllFixed, SuccessEmpty)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<char, 16> buf;
    auto ret = readAllFixed(fd, buf);
    EXPECT_EQ(std::string_view(ret.begin(), ret.end()), "");
}

TEST(ReadAllFixed, Success)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(SizeIs(Ge(3)))).WillOnce(readSv("one"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<char, 16> buf;
    auto ret = readAllFixed(fd, buf);
    EXPECT_EQ(std::string_view(ret.begin(), ret.end()), "alpha one");
}

TEST(ReadAllFixed, Block)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(_)).WillRepeatedly(readSv(""));
    }
    std::array<char, 16> buf;
    EXPECT_THROW(readAllFixed(fd, buf), exception::WouldBlock);
}

TEST(ReadAllFixed, NotEnough)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(5))))
            .WillOnce(readSv("\x01\x02\x03\x04\x05"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x06"));
        EXPECT_CALL(fd, read(_)).WillRepeatedly(readSv(""));
    }
    std::array<int32_t, 2> buf;
    EXPECT_THROW(readAllFixed(fd, buf), exception::Incomplete);
}

TEST(ReadAllFixed, NotEnoughEof)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(5))))
            .WillOnce(readSv("\x01\x02\x03\x04\x05"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x06"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<int32_t, 2> buf;
    EXPECT_THROW(readAllFixed(fd, buf), exception::Incomplete);
}

TEST(ReadAllFixed, EnoughInt)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(5))))
            .WillOnce(readSv("\x01\x02\x03\x04\x05"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x06"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("\x07\x08"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<int32_t, 2> buf;
    EXPECT_THAT(readAllFixed(fd, buf),
                testing::ElementsAre(stdplus::hton(int32_t{0x01020304}),
                                     stdplus::hton(int32_t{0x05060708})));
}

TEST(ReadAllFixed, TooMuch)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(5))))
            .WillOnce(readSv("\x01\x02\x03\x04\x05"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x06"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("\x07\x08"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("\x09"));
    }
    std::array<int32_t, 2> buf;
    EXPECT_THROW(readAllFixed(fd, buf), std::system_error);
}

TEST(ReadAllExact, FailEmpty)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<char, 16> buf;
    EXPECT_THROW(readAllExact(fd, buf), std::system_error);
}

TEST(ReadAllExact, FailPartial)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(SizeIs(Ge(3)))).WillOnce(readSv("one"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<char, 16> buf;
    EXPECT_THROW(readAllExact(fd, buf), std::system_error);
}

TEST(ReadAllExact, Success)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(SizeIs(Ge(10)))).WillOnce(readSv("one tenner"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(exception::Eof("test")));
    }
    std::array<char, 16> buf;
    readAllExact(fd, buf);
    EXPECT_EQ(std::string_view(buf.begin(), buf.end()), "alpha one tenner");
}

TEST(ReadAllExact, TooMuch)
{
    testing::StrictMock<FdMock> fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(SizeIs(Ge(4)))).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(SizeIs(Ge(2)))).WillOnce(readSv("a "));
        EXPECT_CALL(fd, read(SizeIs(Ge(10)))).WillOnce(readSv("one tenner"));
        EXPECT_CALL(fd, read(SizeIs(Ge(1)))).WillOnce(readSv("r"));
    }
    std::array<char, 16> buf;
    EXPECT_THROW(readAllExact(fd, buf), std::system_error);
}

} // namespace stdplus::fd
