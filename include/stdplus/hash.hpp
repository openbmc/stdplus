#pragma once
#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace std
{
template <class Key>
struct hash;
}

namespace stdplus
{

template <class Key>
struct hash;

namespace detail
{

constexpr std::size_t updateSeed(std::size_t seed, std::size_t v) noexcept
{
    return seed ^ (v + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

template <typename T>
constexpr std::size_t hashMultiS(std::size_t seed, const T& t) noexcept(
    noexcept(hash<T>{}(std::declval<const T&>())))
{
    return updateSeed(seed, hash<T>{}(t));
}

template <typename T, typename... Ts>
constexpr std::size_t
    hashMultiS(std::size_t seed, const T& t, const Ts&... ts) noexcept(
        (noexcept(hashMultiS(0, std::declval<const T&>())) && ... &&
         noexcept(hashMultiS(0, std::declval<const Ts&>()))))
{
    return hashMultiS(hashMultiS(seed, t), ts...);
}

} // namespace detail

constexpr std::size_t hashMulti() noexcept
{
    return 0;
}

template <typename T>
constexpr std::size_t hashMulti(const T& t) noexcept(
    noexcept(hash<T>{}(std::declval<const T&>())))
{
    return hash<T>{}(t);
}

template <typename T, typename... Ts>
constexpr std::size_t hashMulti(const T& t, const Ts&... ts) noexcept(
    noexcept(hashMulti(std::declval<const T&>())) &&
    noexcept(detail::hashMultiS(0, std::declval<const Ts&>()...)))
{
    return detail::hashMultiS(hashMulti(t), ts...);
}

namespace detail
{

template <typename T>
constexpr std::size_t hashArr(
    std::size_t seed, const T* ts,
    std::size_t n) noexcept(noexcept(hashMulti(std::declval<const T&>())))
{
    if (n == 1)
    {
        return updateSeed(seed, hashMulti(*ts));
    }
    return hashArr(updateSeed(seed, hashMulti(*ts)), ts + 1, n - 1);
}

template <typename T>
constexpr std::size_t hashArr(const T* ts, std::size_t n) noexcept(
    noexcept(hashMulti(std::declval<const T&>())))
{
    if (n == 0)
    {
        return 0;
    }
    if (n == 1)
    {
        return hashMulti(*ts);
    }
    return hashArr(hashMulti(*ts), ts + 1, n - 1);
}

} // namespace detail

template <class Key>
struct hash : std::hash<std::remove_cv_t<Key>>
{};

template <typename T, std::size_t N>
struct hash<std::array<T, N>>
{
    constexpr std::size_t operator()(const std::array<T, N>& a) const
        noexcept(noexcept(hashMulti(std::declval<const T&>())))
    {
        return detail::hashArr(a.data(), N);
    }
};

template <typename T, std::size_t N>
struct hash<T[N]>
{
    constexpr std::size_t operator()(const T (&a)[N]) const
        noexcept(noexcept(hashMulti(std::declval<const T&>())))
    {
        return detail::hashArr(a, N);
    }
};

template <typename... Ts>
struct hash<std::tuple<Ts...>>
{
    constexpr std::size_t operator()(const std::tuple<Ts...>& ts) const
        noexcept(noexcept(hashMulti(std::declval<const Ts&>()...)))
    {
        return std::apply([](const auto&... vs) { return hashMulti(vs...); },
                          ts);
    }
};

} // namespace stdplus
