#pragma once
#include <functional>
#include <type_traits>
#include <utility>

namespace stdplus
{
namespace detail
{

template <typename T, bool F = std::is_fundamental_v<T>>
struct PinWrap
{
    using type = T;
};

template <typename T>
struct PinWrap<T, true>
{
    using type = struct
    {
        T v;
        constexpr operator T&()
        {
            return v;
        }
        constexpr operator const T&() const
        {
            return v;
        }
    };
};

} // namespace detail

/** @brief Lightweight class wrapper that removes move operations from a class
 *         in order to guarantee the contents stay pinned to a specific location
 *         in memory.
 */
template <typename T, typename CT = detail::PinWrap<T>::type>
struct Pinned : CT
{
    using type = T;

    Pinned(Pinned&&) = delete;
    Pinned& operator=(Pinned&&) = delete;

    template <typename... Args>
    constexpr Pinned(Args&&... args) noexcept(
        noexcept(CT(std::declval<Args>()...))) :
        CT(std::forward<Args>(args)...)
    {}

    template <typename Arg>
    constexpr Pinned& operator=(Arg&& arg) noexcept(
        noexcept(std::declval<CT>() = std::declval<Arg>()))
    {
        static_cast<CT&>(*this) = std::forward<Arg>(arg);
        return *this;
    }
};

template <typename T>
Pinned(T&& t) -> Pinned<std::remove_cvref_t<T>>;

template <typename T>
struct PinnedRef : std::reference_wrapper<T>
{
    using type = T;
    using wrapper = std::reference_wrapper<T>;

    template <typename U,
              std::enable_if_t<
                  !std::is_move_constructible_v<std::remove_cvref_t<U>> &&
                      !std::is_move_assignable_v<std::remove_cvref_t<U>>,
                  bool> = true>
    constexpr PinnedRef(U& u) noexcept : wrapper(u)
    {}

    template <typename U>
    constexpr PinnedRef(Pinned<U>& u) noexcept : wrapper(u)
    {}
    template <typename U,
              std::enable_if_t<!std::is_same_v<U, void> && std::is_const_v<T>,
                               bool> = true>
    constexpr PinnedRef(const Pinned<U>& u) noexcept : wrapper(u)
    {}

    template <typename U>
    constexpr PinnedRef(PinnedRef<U> u) noexcept : wrapper(u)
    {}
};

template <typename T>
PinnedRef(T&& t) -> PinnedRef<std::remove_reference_t<T>>;

template <typename T>
PinnedRef(Pinned<T>& t) -> PinnedRef<T>;

template <typename T>
PinnedRef(const Pinned<T>& t) -> PinnedRef<const T>;

} // namespace stdplus

namespace std
{
template <class T>
typename stdplus::Pinned<T>&& move(stdplus::Pinned<T>& t) noexcept = delete;
}
