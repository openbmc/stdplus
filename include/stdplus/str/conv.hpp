#pragma once
#include <fmt/core.h>

#include <array>
#include <limits>
#include <string_view>
#include <type_traits>

namespace stdplus
{

template <typename F, typename = std::basic_string_view<char>>
struct ToStrIsStatic
{
    static constexpr inline bool value = false;
};

template <typename F>
struct ToStrIsStatic<F, decltype(std::declval<F>().template operator()<char>(
                            std::declval<typename F::type>()))>
{
    static constexpr inline bool value = true;
};

template <typename T, typename = void>
struct ToStr;

template <typename Self, typename T,
          std::size_t BufSize = std::numeric_limits<std::size_t>::max()>
struct ToStrStatic
{
    using type = T;
    static constexpr inline std::size_t bufSize = BufSize;

    template <typename CharT>
    static constexpr inline bool isNoexcept =
        noexcept(std::declval<Self>().template conv<CharT>(std::declval<T>()));

    template <typename CharT>
    constexpr std::basic_string_view<CharT> operator()(const T& v) const
        noexcept(isNoexcept<CharT>)
    {
        return static_cast<const Self*>(this)->template conv<CharT>(v);
    }

    template <typename CharT>
    constexpr std::enable_if_t<
        bufSize != std::numeric_limits<std::size_t>::max(), CharT*>
        operator()(CharT* buf, const T& v) const noexcept(isNoexcept<CharT>)
    {
        auto d = this->template operator()<CharT>(v);
        return std::copy(d.begin(), d.end(), buf);
    }

    template <typename CharT>
    constexpr void operator()(std::basic_string<CharT>& buf, const T& v) const
    {
        auto d = this->template operator()<CharT>(v);
        const auto oldsize = buf.size();
        buf.resize(oldsize + d.size());
        std::copy(d.begin(), d.end(), buf.begin() + oldsize);
    }
};

template <typename Self, typename T, std::size_t BufSize>
struct ToStrFixed
{
    using type = T;
    static constexpr inline std::size_t bufSize = BufSize;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, const T& v) const
        noexcept(noexcept(std::declval<Self>().conv(std::declval<CharT*>(),
                                                    std::declval<T>())))
    {
        return static_cast<const Self&>(*this).conv(buf, v);
    }

    template <typename CharT>
    constexpr void operator()(std::basic_string<CharT>& buf, const T& v) const
    {
        const auto oldsize = buf.size();
        buf.resize(oldsize + bufSize);
        buf.resize((*this)(buf.data() + oldsize, v) - buf.data());
    }
};

template <typename Self, typename T>
struct ToStrDynamic
{
    using type = T;
    static constexpr inline std::size_t bufSize =
        std::numeric_limits<std::size_t>::max();

    template <typename CharT>
    constexpr void operator()(std::basic_string<CharT>& buf, const T& v) const
    {
        static_cast<const Self&>(*this).conv(buf, v);
    }
};

template <typename T, typename CharT = char,
          template <typename, typename = void> class ToStrT = ToStr,
          typename = void>
struct ToStrFinal
{
  private:
    std::basic_string<CharT> buf;

  public:
    constexpr std::basic_string_view<CharT> operator()(const T& v)
    {
        buf.resize(0);
        ToStrT<T>{}(buf, v);
        return buf;
    }
};

template <typename T, typename CharT,
          template <typename, typename = void> class ToStrT>
struct ToStrFinal<T, CharT, ToStrT,
                  std::enable_if_t<ToStrIsStatic<ToStrT<T>>::value>>
{
    constexpr std::basic_string_view<CharT> operator()(const T& v) noexcept(
        noexcept(std::declval<ToStrT<T>>().template operator()<CharT>(
            std::declval<T>())))
    {
        return ToStrT<T>{}.template operator()<CharT>(v);
    }
};

template <typename T, typename CharT,
          template <typename, typename = void> class ToStrT>
struct ToStrFinal<T, CharT, ToStrT,
                  std::enable_if_t<!ToStrIsStatic<ToStrT<T>>::value &&
                                   ToStrT<T>::bufSize !=
                                       std::numeric_limits<std::size_t>::max()>>
{
  private:
    std::array<CharT, ToStrT<T>::bufSize> buf;

  public:
    constexpr std::basic_string_view<CharT> operator()(const T& v) noexcept(
        noexcept(std::declval<ToStrT<T>>()(std::declval<CharT*>(),
                                           std::declval<T>())))
    {
        return {buf.data(), ToStrT<T>{}(buf.data(), v)};
    }
};

template <typename T, typename CharT,
          template <typename, typename = void> class ToStrT = ToStr>
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
        return formatter.format(ToStrFinal<T, CharT, ToStrT>{}(v), ctx);
    }
};

} // namespace stdplus
