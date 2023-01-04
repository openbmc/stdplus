#include <stdplus/hash/array.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(HashTuple, Basic)
{
    std::array<std::string, 2> a{"bacon", "sound"};
    std::hash<decltype(a)>{}(a);
}

} // namespace stdplus
