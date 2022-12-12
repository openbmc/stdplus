#pragma once
#include <cstddef>
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
constexpr std::size_t
    hashMultiS(std::size_t seed,
               const T& t) noexcept(noexcept(hash<T>{}(std::declval<T>())))
{
    return updateSeed(seed, hash<T>{}(t));
}

template <typename T, typename... Ts>
constexpr std::size_t
    hashMultiS(std::size_t seed, const T& t, const Ts&... ts) noexcept(
        (noexcept(hashMultiS(0, std::declval<T>())) &&
         ...&& noexcept(hashMultiS(0, std::declval<Ts>()))))
{
    return hashMultiS(hashMultiS(seed, t), ts...);
}

} // namespace detail

constexpr std::size_t hashMulti() noexcept
{
    return 0;
}

template <typename T>
constexpr std::size_t
    hashMulti(const T& t) noexcept(noexcept(hash<T>{}(std::declval<T>())))
{
    return hash<T>{}(t);
}

template <typename T, typename... Ts>
constexpr std::size_t hashMulti(const T& t, const Ts&... ts) noexcept(
    noexcept(hashMulti(std::declval<T>())) && noexcept(
        detail::hashMultiS(0, std::declval<Ts>()...)))
{
    return detail::hashMultiS(hashMulti(t), ts...);
}

template <class Key>
struct hash : std::hash<Key>
{
};

} // namespace stdplus
