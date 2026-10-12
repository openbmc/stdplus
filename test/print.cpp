#include <stdplus/print.hpp>

#include <cstdio>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(Print, Basic)
{
    auto file = std::tmpfile();
    prints(file, std::string_view{});
    print(file, "hello");
    print(file, "hi {}\n", 4);
    println(file, "ho\n");
    println(file, "ho {}", 16);
    EXPECT_EQ(0, std::fseek(file, 0, SEEK_SET));
    constexpr std::string_view expect = "hellohi 4\nho\n\nho 16\n";
    std::string buf(expect.size(), '\0');
    EXPECT_EQ(buf.size(),
              std::fread(buf.data(), sizeof(char), buf.size() + 1, file));
    EXPECT_EQ(buf, expect);
    EXPECT_EQ(0, std::fclose(file));

    print("hello");
    print("hi {}\n", 4);
    println("ho\n");
    println("ho {}", 16);

    auto ro = std::fopen("/dev/null", "r");
    ASSERT_NE(nullptr, ro);
    std::setbuf(ro, nullptr);
    EXPECT_THROW(print(ro, "hello"), std::system_error);
    EXPECT_EQ(0, std::fclose(ro));
}

} // namespace stdplus
