#include <stdplus/hash/tuple.hpp>

#include <string>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(HashTuple, Basic)
{
    std::tuple<std::string, int> a{"bacon", 2};
    std::hash<decltype(a)>{}(a);
}

} // namespace stdplus
