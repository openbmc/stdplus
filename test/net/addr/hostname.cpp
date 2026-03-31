#include <stdplus/net/addr/hostname.hpp>

#include <string>

#include <gtest/gtest.h>

using namespace stdplus;

// Test valid FQDNs (multi-label hostnames)
TEST(Hostname, ValidFQDN)
{
    EXPECT_NO_THROW(Hostname("example.com"));
    EXPECT_NO_THROW(Hostname("sub.example.com"));
    EXPECT_NO_THROW(Hostname("deep.sub.example.com"));
    EXPECT_NO_THROW(Hostname("my-server.example.com"));
    EXPECT_NO_THROW(Hostname("server1.example.com"));
    EXPECT_NO_THROW(Hostname("a.b.c.d.e.f.g.example.com"));
}

// Test valid single-label hostnames (local hostnames)
TEST(Hostname, ValidLocalHostname)
{
    EXPECT_NO_THROW(Hostname("localhost"));
    EXPECT_NO_THROW(Hostname("server1"));
    EXPECT_NO_THROW(Hostname("myhost"));
    EXPECT_NO_THROW(Hostname("web-server"));
    EXPECT_NO_THROW(Hostname("ntp"));
    EXPECT_NO_THROW(Hostname("timeserver"));
}

// Test trailing dot support (DNS root)
TEST(Hostname, TrailingDot)
{
    EXPECT_NO_THROW(Hostname("example.com."));
    EXPECT_NO_THROW(Hostname("sub.example.com."));

    // Verify trailing dot is removed in stored value
    Hostname h("example.com.");
    EXPECT_EQ(h.getHostname(), "example.com");
}

// Test invalid hostnames that start or end with hyphen
TEST(Hostname, InvalidStartEnd)
{
    EXPECT_THROW(Hostname("-example.com"), std::exception);
    EXPECT_THROW(Hostname("sub.-example.com"), std::exception);
    EXPECT_THROW(Hostname("example-.com"), std::exception);
    EXPECT_THROW(Hostname("example.com-"), std::exception);
    EXPECT_THROW(Hostname(".example.com"), std::exception);
    EXPECT_THROW(Hostname("-localhost"), std::exception);
    EXPECT_THROW(Hostname("localhost-"), std::exception);
}

// Test invalid characters
TEST(Hostname, InvalidCharacters)
{
    EXPECT_THROW(Hostname("my_server.com"), std::exception);
    EXPECT_THROW(Hostname("my server.com"), std::exception);
    EXPECT_THROW(Hostname("my@server.com"), std::exception);
    EXPECT_THROW(Hostname("my#server.com"), std::exception);
    EXPECT_THROW(Hostname("my$server.com"), std::exception);
}

// Test consecutive dots (empty labels)
TEST(Hostname, ConsecutiveDots)
{
    EXPECT_THROW(Hostname("example..com"), std::exception);
    EXPECT_THROW(Hostname("sub..example.com"), std::exception);
}

// Test length limits (RFC 1035) - Zero allocation version
TEST(Hostname, LengthLimits)
{
    // Test max label length (63 characters) - VALID
    EXPECT_NO_THROW((Hostname{
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.com"}));

    // Test label too long (64 characters) - INVALID
    EXPECT_THROW(
        (Hostname{
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.com"}),
        std::exception);

    // Test: 250-char label violates RFC 1035 - INVALID
    EXPECT_THROW(
        (Hostname{
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaa.co"}),
        std::exception);
}
// Test empty string
TEST(Hostname, EmptyString)
{
    EXPECT_THROW(Hostname(""), std::exception);
}

// Test all-numeric hostnames (should be rejected)
TEST(Hostname, AllNumeric)
{
    EXPECT_THROW(Hostname("123456"), std::exception);
    EXPECT_THROW(Hostname("192.168.1"), std::exception);
    EXPECT_THROW(Hostname("256.1.1.1"), std::exception);
}

// Test comparison operators (using spaceship operator)
TEST(Hostname, Comparison)
{
    Hostname h1("example.com");
    Hostname h2("example.com");
    Hostname h3("test.com");
    Hostname h4("aaa.com");

    // Equality
    EXPECT_EQ(h1, h2);
    EXPECT_NE(h1, h3);

    // Ordering (lexicographic)
    EXPECT_LT(h4, h1); // "aaa" < "example"
    EXPECT_LE(h4, h1);
    EXPECT_LE(h1, h2); // equal
    EXPECT_GT(h3, h1); // "test" > "example"
    EXPECT_GE(h3, h1);
    EXPECT_GE(h1, h2); // equal
}

// Test ToStr and FromStr conversions
TEST(Hostname, ToFromStr)
{
    Hostname h("example.com");

    // ToStr requires buffer-based conversion
    char buf[253];
    auto end = ToStr<Hostname>{}(buf, h);
    std::string str(buf, end - buf);
    EXPECT_EQ(str, "example.com");

    // FromStr requires string_view
    Hostname h2 = FromStr<Hostname>{}(std::string_view("test.example.com"));
    EXPECT_EQ(h2.getHostname(), "test.example.com");
}

// Test numeric labels (valid when mixed with alphabetic)
TEST(Hostname, NumericLabels)
{
    EXPECT_NO_THROW(Hostname("192.example.com"));
    EXPECT_NO_THROW(Hostname("server1.example.com"));
    EXPECT_NO_THROW(Hostname("123.456.example.com"));
}

// Test mixed case (DNS is case-insensitive)
TEST(Hostname, MixedCase)
{
    EXPECT_NO_THROW(Hostname("Example.COM"));
    EXPECT_NO_THROW(Hostname("MyServer.Example.Com"));
}
