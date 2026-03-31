#include <stdplus/net/addr/hostname.hpp>

#include <gtest/gtest.h>

using namespace stdplus;

TEST(HostnameTest, ValidHostnames)
{
    EXPECT_NO_THROW(Hostname("example.com"));
    EXPECT_NO_THROW(Hostname("pool.ntp.org"));
    EXPECT_NO_THROW(Hostname("time.google.com"));
    EXPECT_NO_THROW(Hostname("example.com."));
}

TEST(HostnameTest, InvalidHostnames)
{
    EXPECT_THROW(Hostname(""), std::invalid_argument);
    EXPECT_THROW(Hostname(".example.com"), std::invalid_argument);
    EXPECT_THROW(Hostname("123456"), std::invalid_argument);
    EXPECT_THROW(Hostname("256.1.1.1"), std::invalid_argument);
}

TEST(HostnameTest, IsValid)
{
    EXPECT_TRUE(Hostname::isValid("example.com"));
    EXPECT_TRUE(Hostname::isValid("pool.ntp.org"));
    EXPECT_FALSE(Hostname::isValid(""));
    EXPECT_FALSE(Hostname::isValid("123456"));
}
