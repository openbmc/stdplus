#pragma once
#include <cstdint>
#include <type_traits>
#include <utility>

namespace stdplus
{

template <typename F>
struct function_view;

namespace detail
{

/** @brief Turn lambdas into a type erased function */
template <typename>
struct FViewGuide;

template <typename R, typename O, bool Nx, typename... As>
struct FViewGuide<R (O::*)(As...) noexcept(Nx)>
{
    using type = R(As...) noexcept(Nx);
};

template <typename R, typename O, bool Nx, typename... As>
struct FViewGuide<R (O::*)(As...) & noexcept(Nx)>
{
    using type = R(As...) noexcept(Nx);
};

template <typename R, typename O, bool Nx, typename... As>
struct FViewGuide<R (O::*)(As...) const noexcept(Nx)>
{
    using type = R(As...) const noexcept(Nx);
};

template <typename R, typename O, bool Nx, typename... As>
struct FViewGuide<R (O::*)(As...) const & noexcept(Nx)>
{
    using type = R(As...) const noexcept(Nx);
};

template <typename F>
struct IsFView : std::false_type
{};

template <typename... Args>
struct IsFView<function_view<Args...>> : std::true_type
{};

template <typename F>
concept NonFView = !IsFView<std::decay_t<F>>::value;

template <typename F>
concept NonFunc =
    !std::is_function_v<std::remove_pointer_t<std::remove_cvref_t<F>>> &&
    NonFView<F>;

template <typename R, bool C, bool Nx, typename... Args>
struct function_view_base
{
  protected:
    union
    {
        R (*fun)(Args...);
        R (*memfun)(void*, Args...);
    };
    static_assert(sizeof(fun) == sizeof(memfun));
    void* obj;

    template <bool C2, bool Nx2>
    constexpr function_view_base(
        const function_view_base<R, C2, Nx2, Args...>& other) noexcept :
        obj(other.obj)
    {
        if (obj == nullptr)
        {
            fun = other.fun;
        }
        else
        {
            memfun = other.memfun;
        }
    }

    template <bool C2, bool Nx2>
    constexpr void assign(
        const function_view_base<R, C2, Nx2, Args...>& other) noexcept
    {
        obj = other.obj;
        if (obj == nullptr)
        {
            fun = other.fun;
        }
        else
        {
            memfun = other.memfun;
        }
    }

    template <typename, bool, bool, typename...>
    friend struct function_view_base;

  public:
    constexpr function_view_base() noexcept : fun(nullptr), obj(nullptr) {};
    constexpr function_view_base(std::nullptr_t) noexcept :
        function_view_base() {};

    template <bool Nx2>
        requires(!Nx || Nx2)
    constexpr function_view_base(R (*f)(Args...) noexcept(Nx2)) noexcept :
        fun(f), obj(nullptr)
    {}

    template <NonFunc F>
        requires(!C && std::invocable<F&, Args...> &&
                 (!Nx || std::is_nothrow_invocable_v<F&, Args...>) &&
                 std::same_as<std::invoke_result_t<F&, Args...>, R>)
    constexpr function_view_base(F& f) noexcept :
        memfun([](void* v, Args... args) {
            return (*reinterpret_cast<F*>(v))(std::forward<Args>(args)...);
        }),
        obj(std::addressof(f))
    {}

    template <NonFunc F>
        requires(std::invocable<const F&, Args...> &&
                 (!Nx || std::is_nothrow_invocable_v<const F&, Args...>) &&
                 std::same_as<std::invoke_result_t<const F&, Args...>, R>)
    constexpr function_view_base(const F& f) noexcept :
        memfun([](void* v, Args... args) {
            return (*reinterpret_cast<const F*>(v))(
                std::forward<Args>(args)...);
        }),
        obj(const_cast<F*>(std::addressof(f)))
    {}

    constexpr R operator()(Args... args) const noexcept(Nx)
    {
        if (obj == nullptr)
        {
            return fun(std::forward<Args>(args)...);
        }
        return memfun(obj, std::forward<Args>(args)...);
    }

    constexpr explicit operator bool() const noexcept
    {
        return obj != nullptr || fun != nullptr;
    }
};

} // namespace detail

template <typename R, typename... Args, bool Nx>
struct function_view<R(Args...) noexcept(Nx)> :
    detail::function_view_base<R, /*const=*/false, Nx, Args...>
{
    using _Base = detail::function_view_base<R, /*const=*/false, Nx, Args...>;

    template <bool Nx2>
        requires(!Nx || Nx2)
    constexpr function_view(
        const function_view<R(Args...) noexcept(Nx2)>& other) noexcept :
        _Base(other)
    {}

    template <bool Nx2>
        requires(!Nx || Nx2)
    constexpr function_view(
        const function_view<R(Args...) const noexcept(Nx2)>& other) noexcept :
        _Base(other)
    {}

    template <bool Nx2>
        requires(!Nx || Nx2)
    constexpr function_view& operator=(
        const function_view<R(Args...) noexcept(Nx2)>& other) noexcept
    {
        this->assign(other);
        return *this;
    }

    template <bool Nx2>
        requires(!Nx || Nx2)
    constexpr function_view& operator=(
        const function_view<R(Args...) const noexcept(Nx2)>& other) noexcept
    {
        this->assign(other);
        return *this;
    }

    template <detail::NonFView F>
        requires std::constructible_from<_Base, F>
    constexpr function_view(F&& f) noexcept : _Base(std::forward<F>(f))
    {}
};

template <typename R, typename... Args, bool Nx>
struct function_view<R(Args...) const noexcept(Nx)> :
    detail::function_view_base<R, /*const=*/true, Nx, Args...>
{
    using _Base = detail::function_view_base<R, /*const=*/true, Nx, Args...>;

    template <bool Nx2>
        requires(!Nx || Nx2)
    constexpr function_view(
        const function_view<R(Args...) const noexcept(Nx2)>& other) noexcept :
        _Base(other)
    {}

    template <bool Nx2>
        requires(!Nx || Nx2)
    constexpr function_view& operator=(
        const function_view<R(Args...) const noexcept(Nx2)>& other) noexcept
    {
        this->assign(other);
        return *this;
    }

    template <detail::NonFView F>
        requires std::constructible_from<_Base, F>
    constexpr function_view(F&& f) noexcept : _Base(std::forward<F>(f))
    {}

    template <typename>
    friend struct function_view;
};

template <typename R, typename... Args, bool Nx>
function_view(R (*)(Args...) noexcept(Nx))
    -> function_view<R(Args...) const noexcept(Nx)>;

template <typename F>
function_view(F) -> function_view<
    typename detail::FViewGuide<decltype(&F::operator())>::type>;

} // namespace stdplus
