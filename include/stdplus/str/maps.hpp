#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
namespace stdplus
{

namespace detail
{
struct string_hash : public std::hash<std::string_view>
{
    using is_transparent = void;
};
} // namespace detail

template <typename V>
using string_umap =
    std::unordered_map<std::string, V, detail::string_hash, std::equal_to<>>;
using string_uset =
    std::unordered_set<std::string, detail::string_hash, std::equal_to<>>;

} // namespace stdplus
