#pragma once
#include <optional>
#include <stdplus/handle/managed.hpp>
#include <type_traits>
#include <utility>

namespace stdplus
{

/** @brief Similar to the Managed Handle, but also allows for copying
 *         and performs an operation during that copy.
 */
template <typename T, typename... As>
struct Copyable
{
    template <typename Drop, typename Ref>
    class HandleF : public Managed<T, As...>::template HandleF<Drop>
    {
      protected:
        static constexpr bool ref_noexcept =
            noexcept(Ref()(std::declval<T>(), std::declval<As&>()...));

      public:
        using MHandleF = typename Managed<T, As...>::template HandleF<Drop>;

        /** @brief Creates a handle referencing the object
         *
         *  @param[in] maybeV - Optional object being managed
         */
        template <typename... Vs>
        constexpr explicit HandleF(const std::optional<T>& maybeV, Vs&&... vs) noexcept(
            noexcept(MHandleF(std::nullopt, std::declval<Vs>()...)) && noexcept(
                std::declval<HandleF>().reset(
                    std::declval<const std::optional<T>&>()))) :
            MHandleF(std::nullopt, std::forward<Vs>(vs)...)
        {
            reset(maybeV);
        }
        template <typename... Vs>
        constexpr explicit HandleF(const T& maybeV, Vs&&... vs) noexcept(
            noexcept(MHandleF(std::nullopt, std::declval<Vs>()...)) && noexcept(
                std::declval<HandleF>().reset(std::declval<const T&>()))) :
            MHandleF(std::nullopt, std::forward<Vs>(vs)...)
        {
            reset(maybeV);
        }

        /** @brief Creates a handle owning the object
         *
         *  @param[in] maybeV - Maybe the object being managed
         */
        template <typename... Vs>
        constexpr explicit HandleF(
            std::optional<T>&& maybeV,
            Vs&&... vs) noexcept(noexcept(MHandleF(std::
                                                       declval<std::optional<
                                                           T>&&>(),
                                                   std::declval<Vs>()...))) :
            MHandleF(std::move(maybeV), std::forward<Vs>(vs)...)
        {
        }
        template <typename... Vs>
        constexpr explicit HandleF(T&& maybeV, Vs&&... vs) noexcept(
            noexcept(MHandleF(std::declval<T&&>(), std::declval<Vs>()...))) :
            MHandleF(std::move(maybeV), std::forward<Vs>(vs)...)
        {
        }

        constexpr HandleF(const HandleF& other) noexcept(noexcept(MHandleF(
            std::nullopt,
            std::declval<const std::tuple<
                As...>&>())) && noexcept(std::declval<HandleF>()
                                             .reset(std::declval<
                                                    const std::optional<
                                                        T>&>()))) :
            MHandleF(std::nullopt, other.as)
        {
            reset(other.maybe_value());
        }

        constexpr HandleF(HandleF&& other) noexcept(
            std::is_nothrow_move_constructible_v<MHandleF>) :
            MHandleF(std::move(other))
        {
        }

        constexpr HandleF& operator=(const HandleF& other) noexcept(
            noexcept(std::declval<HandleF>().reset()) &&
            std::is_nothrow_copy_constructible_v<std::tuple<As...>>&& noexcept(
                std::declval<HandleF>().reset(
                    std::declval<const std::optional<T>&>())))
        {
            if (this != &other)
            {
                reset();
                this->as = other.as;
                reset(other.maybe_value());
            }
            return *this;
        }

        constexpr HandleF& operator=(HandleF&& other) noexcept(
            std::is_nothrow_move_assignable_v<MHandleF>)
        {
            MHandleF::operator=(std::move(other));
            return *this;
        }

        using MHandleF::reset;
        constexpr void reset(const std::optional<T>& maybeV) noexcept(
            ref_noexcept&& noexcept(std::declval<HandleF>().reset(
                std::declval<T>())) && noexcept(std::declval<HandleF>()
                                                    .reset()))
        {
            if (maybeV)
            {
                reset(doRef(*maybeV, std::index_sequence_for<As...>()));
            }
            else
            {
                reset();
            }
        }
        constexpr void reset(const T& maybeV) noexcept(ref_noexcept&& noexcept(
            std::declval<HandleF>().reset(std::declval<T>())))
        {
            reset(doRef(maybeV, std::index_sequence_for<As...>()));
        }

      private:
        template <size_t... Indices>
        T doRef(const T& v,
                std::index_sequence<Indices...>) noexcept(ref_noexcept)
        {
            return Ref()(v, std::get<Indices>(this->as)...);
        }
    };

    template <T (*ref)(const T&, As&...)>
    struct Reffer
    {
        T operator()(const T& t, As&... as) noexcept(noexcept(ref))
        {
            return ref(t, as...);
        }
    };

    template <void (*drop)(T&&, As&...), T (*ref)(const T&, As&...)>
    using Handle = HandleF<typename Managed<T, As...>::template Dropper<drop>,
                           Reffer<ref>>;
};

} // namespace stdplus
