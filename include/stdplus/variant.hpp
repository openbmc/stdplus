#include <stdplus/concepts.hpp>

#include <concepts>
#include <type_traits>
#include <utility>
#include <variant>

namespace stdplus
{
namespace detail
{

template <template <typename, typename> typename Veq, typename...>
struct CanVeq;

template <template <typename, typename> typename Veq, typename T>
struct CanVeq<Veq, T>
{
    static constexpr inline bool value = false;
};

template <template <typename, typename> typename Veq, typename T, typename V,
          typename... Vs>
struct CanVeq<Veq, T, V, Vs...>
{
    static constexpr inline bool value = Veq<T, V>::value ||
                                         CanVeq<Veq, T, Vs...>::value;
};

template <typename T1, typename T2>
struct VeqBase
{
    static constexpr inline bool value = false;

    constexpr bool operator()(const T1&, const T2&) const noexcept
    {
        return false;
    }
};

template <typename T1, typename T2>
struct VeqFuzzy : VeqBase<T1, T2>
{};

template <typename T1, WeaklyEqualityComparableWith<T1> T2>
struct VeqFuzzy<T1, T2>
{
    static constexpr inline bool value = true;

    constexpr bool operator()(const T1& lhs, const T2& rhs) const noexcept
    {
        return lhs == rhs;
    }
};

template <typename T1, typename T2>
struct VeqStrict : VeqBase<T1, T2>
{};

template <typename T1, std::same_as<T1> T2>
struct VeqStrict<T1, T2>
{
    static constexpr inline bool value = true;

    constexpr bool operator()(const T1& lhs, const T2& rhs) const noexcept
    {
        return lhs == rhs;
    }
};

} // namespace detail

template <template <typename, typename> typename Veq, typename... Vs,
          typename T,
          std::enable_if_t<detail::CanVeq<Veq, T, Vs...>::value, bool> = true>
constexpr bool variantEq(const std::variant<Vs...>& vs, const T& t) noexcept
{
    return std::visit(
        [&t](const auto& v) {
        return Veq<std::remove_cvref_t<decltype(v)>, T>{}(v, t);
        },
        vs);
}

template <template <typename, typename> typename Veq, typename... Vs,
          typename... Vs2>
constexpr bool variantEq(const std::variant<Vs...>& vs,
                         const std::variant<Vs2...>& vs2) noexcept
{
    return std::visit([&vs](const auto& v) { return variantEq<Veq>(vs, v); },
                      vs2);
}

template <typename... Vs>
constexpr bool variantEqFuzzy(const std::variant<Vs...>& vs,
                              const auto& t) noexcept
{
    return variantEq<detail::VeqFuzzy>(vs, t);
}

template <typename... Vs>
constexpr bool variantEqStrict(const std::variant<Vs...>& vs,
                               const auto& t) noexcept
{
    return variantEq<detail::VeqStrict>(vs, t);
}

} // namespace stdplus
