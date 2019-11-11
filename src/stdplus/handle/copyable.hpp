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
      public:
        using MHandleF = typename Managed<T, As...>::template HandleF<Drop>;

        /** @brief Creates a handle referencing the object
         *
         *  @param[in] maybeV - Optional object being managed
         */
        template <typename... Vs>
        constexpr explicit HandleF(const std::optional<T>& maybeV, Vs&&... vs) :
            MHandleF(std::nullopt, std::forward<Vs>(vs)...)
        {
            reset(maybeV);
        }
        template <typename... Vs>
        constexpr explicit HandleF(const T& maybeV, Vs&&... vs) :
            MHandleF(std::nullopt, std::forward<Vs>(vs)...)
        {
            reset(maybeV);
        }

        /** @brief Creates a handle owning the object
         *
         *  @param[in] maybeV - Maybe the object being managed
         */
        template <typename... Vs>
        constexpr explicit HandleF(std::optional<T>&& maybeV, Vs&&... vs) :
            MHandleF(std::move(maybeV), std::forward<Vs>(vs)...)
        {
        }
        template <typename... Vs>
        constexpr explicit HandleF(T&& maybeV, Vs&&... vs) :
            MHandleF(std::move(maybeV), std::forward<Vs>(vs)...)
        {
        }

        constexpr HandleF(const HandleF& other) :
            MHandleF(std::nullopt, other.as)
        {
            reset(other.maybe_value());
        }

        constexpr HandleF(HandleF&& other) noexcept(
            std::is_nothrow_move_constructible_v<MHandleF>) :
            MHandleF(std::move(other))
        {
        }

        constexpr HandleF& operator=(const HandleF& other)
        {
            if (this != &other)
            {
                reset();
                this->as = other.as;
                reset(other.maybe_value());
            }
            return *this;
        }

        constexpr HandleF& operator=(HandleF&& other)
        {
            MHandleF::operator=(std::move(other));
            return *this;
        }

        using MHandleF::reset;

        /** @brief Resets the managed value to a new value
         *         Takes a new reference on the value
         *
         *  @param[in] maybeV - Maybe the new value
         */
        constexpr void reset(const std::optional<T>& maybeV)
        {
            if (maybeV)
            {
                reset(doRef(*maybeV, std::index_sequence_for<As...>()));
            }
            else
            {
                reset(std::nullopt);
            }
        }
        constexpr void reset(const T& maybeV)
        {
            reset(doRef(maybeV, std::index_sequence_for<As...>()));
        }

      private:
        template <size_t... Indices>
        T doRef(const T& v, std::index_sequence<Indices...>)
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
