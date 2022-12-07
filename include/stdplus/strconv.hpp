#pragma once
#include <array>
#include <fmt/core.h>
#include <string_view>
#include <type_traits>

namespace stdplus
{

template <typename T, typename CharT = char, bool enable = true>
struct ToStr
{
    static inline constexpr size_t buf_size = 0;

    constexpr CharT* operator()(CharT* buf, T v) const noexcept
    {
        static_assert(std::is_same_v<T, void>);
    }
};

template <typename T, typename CharT = char,
          template <typename, typename> class ToStrT = ToStr>
struct ToStrBuf
{
  public:
    constexpr std::basic_string_view<CharT> operator()(const T& v) noexcept(
        noexcept(std::declval<ToStrT<T, CharT>>()(std::declval<CharT*>(),
                                                  std::declval<T>())))
    {
        return {buf.data(), ToStrT<T, CharT>{}(buf.data(), v)};
    }

  private:
    std::array<CharT, ToStrT<T, CharT>::buf_size> buf;
};

template <typename T, typename CharT,
          template <typename, typename> class ToStrT = ToStr>
struct Format
{
  private:
    fmt::formatter<std::basic_string_view<CharT>> formatter;

  public:
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(auto v, FormatContext& ctx) const
    {
        return formatter.format(ToStrBuf<T, CharT, ToStrT>{}(v), ctx);
    }
};

} // namespace stdplus
