#include <stdplus/str/maps.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(StringMaps, Basic)
{
    string_umap<int> a = {{"hi", 2}};
    EXPECT_EQ(2, a.find(std::string_view("hi"))->second);
}

} // namespace stdplus
