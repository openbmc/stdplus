#pragma once
#include <string>
#include <string_view>
#include <utility>

namespace stdplus
{
namespace detail
{

template <typename CharT, typename Traits, typename Alloc, typename... CharTs>
constexpr void strAppend(std::basic_string<CharT, Traits, Alloc>& dst,
                         std::basic_string_view<CharTs>... strs)
{
    dst.reserve((dst.size() + ... + strs.size()));
    (dst.append(strs), ...);
}

template <typename CharT>
struct DedSV : std::basic_string_view<CharT>
{
    DedSV(const CharT* p) noexcept : std::basic_string_view<CharT>(p) {}
    template <typename Traits, typename Alloc>
    DedSV(const std::basic_string<CharT, Traits, Alloc>& s) noexcept :
        std::basic_string_view<CharT>(s)
    {}
    DedSV(std::basic_string_view<CharT> sv) noexcept :
        std::basic_string_view<CharT>(sv)
    {}
};

DedSV(const auto& C)
    -> DedSV<std::decay_t<std::remove_pointer_t<decltype(std::data(C))>>>;

} // namespace detail

inline namespace util
{

/** @brief Appends multiple strings to the end of the destination string
 *         in the most optimal way for the given inputs.
 *
 *  @param[in, out] dst - The string being appended to
 *  @param[in] ...strs  - An arbitrary number of strings to concatenate
 */
constexpr void strAppend(auto& dst, const auto&... strs)
{
    detail::strAppend(dst, detail::DedSV(strs)...);
}

/** @brief Concatenates multiple strings together in the most optimal
 *         way for the given inputs.
 *
 *  @param[in] ...strs - An arbitrary number of strings to concatenate
 *  @return The concatenated result string
 */
template <typename CharT = char,
          typename Traits = std::basic_string<CharT>::traits_type,
          typename Alloc = std::basic_string<CharT>::allocator_type>
constexpr auto strCat(const auto&... strs)
{
    std::basic_string<CharT, Traits, Alloc> ret;
    strAppend(ret, strs...);
    return ret;
}
template <typename CharT, typename Traits, typename Alloc>
constexpr auto strCat(std::basic_string<CharT, Traits, Alloc>&& in,
                      const auto&... strs)
{
    auto ret = std::move(in);
    strAppend(ret, strs...);
    return ret;
}

} // namespace util
} // namespace stdplus
