#include "util.hpp"

#include <stdplus/exception.hpp>
#include <stdplus/fd/gmock.hpp>
#include <stdplus/fd/line.hpp>

#include <string>

#include <gtest/gtest.h>

namespace stdplus
{
namespace fd
{

TEST(LineReader, Empty)
{
    auto fd = makeMemfd("");
    LineReader reader(fd);
    EXPECT_EQ("", *reader.readLine());
    EXPECT_THROW(reader.readLine(), exception::Eof);
    EXPECT_THROW(reader.readLine(), exception::Eof);
}

TEST(LineReader, SingleLine)
{
    auto fd = makeMemfd("\n");
    LineReader reader(fd);
    EXPECT_EQ("", *reader.readLine());
    EXPECT_EQ("", *reader.readLine());
    EXPECT_THROW(reader.readLine(), exception::Eof);
    EXPECT_THROW(reader.readLine(), exception::Eof);
}

TEST(LineReader, SomeData)
{
    auto fd = makeMemfd("A\nbcd\n\ne");
    LineReader reader(fd);
    EXPECT_EQ("A", *reader.readLine());
    EXPECT_EQ("bcd", *reader.readLine());
    EXPECT_EQ("", *reader.readLine());
    EXPECT_EQ("e", *reader.readLine());
    EXPECT_THROW(reader.readLine(), exception::Eof);
    EXPECT_THROW(reader.readLine(), exception::Eof);
}

TEST(LineReader, LargerThanBuf)
{
    std::string big(LineReader::buf_size + 10, 'a');
    auto fd = makeMemfd(std::string("alpha\n") + big + "\ndef");
    LineReader reader(fd);
    EXPECT_EQ("alpha", *reader.readLine());
    EXPECT_EQ(big, *reader.readLine());
    EXPECT_EQ("def", *reader.readLine());
    EXPECT_THROW(reader.readLine(), exception::Eof);
    EXPECT_THROW(reader.readLine(), exception::Eof);
}

using testing::_;

TEST(LineReader, Nonblock)
{
    FdMock fd;
    {
        testing::InSequence seq;
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("alph"));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv({}));
        EXPECT_CALL(fd, read(_)).WillOnce(readSv("a"));
        EXPECT_CALL(fd, read(_))
            .WillRepeatedly(testing::Throw(stdplus::exception::Eof("test")));
    }

    LineReader reader(fd);
    EXPECT_EQ(nullptr, reader.readLine());
    EXPECT_EQ("alpha", *reader.readLine());
    EXPECT_THROW(reader.readLine(), exception::Eof);
    EXPECT_THROW(reader.readLine(), exception::Eof);
}

} // namespace fd
} // namespace stdplus
