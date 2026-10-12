#pragma once
#include <array>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace stdplus
{

template <typename CharT, std::size_t ObjSize, typename Alloc>
class BasicStrBuf;

namespace detail
{

template <typename DstCharT, typename CharT>
constexpr std::size_t selfOffset(
    [[maybe_unused]] const DstCharT* old_data,
    [[maybe_unused]] std::size_t old_size,
    [[maybe_unused]] std::basic_string_view<CharT> s) noexcept
{
    if constexpr (!std::is_same_v<DstCharT, CharT>)
    {
        // A view of another character type is never rebased
        return std::size_t(-1);
    }
    else
    {
        if (s.empty())
        {
            return std::size_t(-1);
        }
        if (std::is_constant_evaluated())
        {
            for (std::size_t i = 0; i < old_size; ++i)
            {
                if (s.data() == old_data + i)
                {
                    return i;
                }
            }
            return std::size_t(-1);
        }
        if (std::less_equal<const CharT*>{}(old_data, s.data()) &&
            std::less<const CharT*>{}(s.data(), old_data + old_size))
        {
            return static_cast<std::size_t>(s.data() - old_data);
        }
        return std::size_t(-1);
    }
}

template <typename DstCharT, typename CharT>
constexpr std::basic_string_view<CharT> rebaseSv(
    [[maybe_unused]] const DstCharT* new_data,
    [[maybe_unused]] std::size_t offset,
    std::basic_string_view<CharT> s) noexcept
{
    if constexpr (std::is_same_v<DstCharT, CharT>)
    {
        if (offset != std::size_t(-1))
        {
            return {new_data + offset, s.size()};
        }
    }
    return s;
}

template <typename CharT, typename Traits, typename Alloc, typename... CharTs>
constexpr void strAppend(std::basic_string<CharT, Traits, Alloc>& dst,
                         std::basic_string_view<CharTs>... strs)
{
    [[maybe_unused]] const auto offsets =
        std::array<std::size_t, sizeof...(strs)>{
            selfOffset(dst.data(), dst.size(), strs)...};
    dst.reserve((dst.size() + ... + strs.size()));
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (dst.append(rebaseSv(dst.data(), offsets[Is], strs)), ...);
    }(std::index_sequence_for<CharTs...>{});
}
template <typename CharT, std::size_t ObjSize, typename Alloc,
          typename... CharTs>
constexpr void strAppend(stdplus::BasicStrBuf<CharT, ObjSize, Alloc>& dst,
                         std::basic_string_view<CharTs>... strs)
{
    [[maybe_unused]] const auto offsets =
        std::array<std::size_t, sizeof...(strs)>{
            selfOffset(dst.data(), dst.size(), strs)...};
    [[maybe_unused]] auto out = dst.append((strs.size() + ... + 0));
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ((out = std::copy(rebaseSv(dst.data(), offsets[Is], strs).begin(),
                          rebaseSv(dst.data(), offsets[Is], strs).end(), out)),
         ...);
    }(std::index_sequence_for<CharTs...>{});
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
