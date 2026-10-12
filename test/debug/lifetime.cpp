#include <stdplus/debug/lifetime.hpp>

#include <cstddef>
#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace stdplus::debug
{

// Lifetime only reports to stderr. Capture it with gtest's
// testing::internal::CaptureStderr(), which is not part of the documented
// gtest API but is widely used and stable across gtest releases.
static std::vector<std::string> captureLines(std::string out)
{
    std::vector<std::string> ret;
    std::size_t pos;
    while ((pos = out.find('\n')) != out.npos)
    {
        ret.push_back(out.substr(0, pos));
        out.erase(0, pos + 1);
    }
    return ret;
}

static std::size_t lastId(const std::string& line)
{
    return std::stoul(line.substr(line.rfind(' ') + 1));
}

TEST(Lifetime, Operations)
{
    testing::internal::CaptureStderr();
    {
        Lifetime a;
        Lifetime b(a);
        Lifetime c(std::move(a));
        b = c;
        a = std::move(b);
    }
    const auto lines = captureLines(testing::internal::GetCapturedStderr());
    ASSERT_EQ(8u, lines.size());
    const auto id = lastId(lines[0]);
    const auto check =
        [&](std::size_t i, std::string_view op, std::string suffix) {
            EXPECT_TRUE(lines[i].starts_with(std::format("Lifetime {} ", op)))
                << lines[i];
            EXPECT_TRUE(lines[i].ends_with(suffix)) << lines[i];
        };
    check(0, "Construct", std::format(" {}", id));
    check(1, "Copy", std::format(" {}->{}", id, id + 1));
    check(2, "Move", std::format(" {}->{}", id, id + 2));
    check(3, "Copy", std::format(" {}->{} drop {}", id + 2, id + 3, id + 1));
    check(4, "Move", std::format(" {}->{} drop {}", id + 3, id + 4, id));
    check(5, "Destroy", std::format(" {}", id + 2));
    check(6, "Destroy", std::format(" {}", id + 3));
    check(7, "Destroy", std::format(" {}", id + 4));
}

TEST(Lifetime, Assignment)
{
    Lifetime a;
    const auto bloc = std::source_location::current();
    Lifetime b(bloc);
    testing::internal::CaptureStderr();
    auto& self = a;
    a = self;
    a = std::move(self);
    a = b;
    const auto lines = captureLines(testing::internal::GetCapturedStderr());
    ASSERT_EQ(3u, lines.size());
    // Self-assignment must log the original id as both source and dropped id
    for (std::size_t i = 0; i < 2; ++i)
    {
        const auto& l = lines[i];
        const auto drop = l.rfind(" drop ");
        const auto arrow = l.rfind("->", drop);
        const auto src = l.rfind(' ', arrow) + 1;
        EXPECT_EQ(l.substr(src, arrow - src), l.substr(drop + 6)) << l;
    }
    // Assignment must take over the source location of the assigned object
    EXPECT_NE(
        std::string::npos,
        lines[2].find(std::format("{}:{}(", bloc.file_name(), bloc.line())))
        << lines[2];
}

} // namespace stdplus::debug
