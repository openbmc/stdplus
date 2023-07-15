#pragma once
#include <cstdint>
#include <utility>

namespace stdplus
{

namespace detail
{

class fview_store
{
  private:
    uintptr_t fun;
    void* obj;

  public:
    inline fview_store(void* fun, void* obj) :
        fun(reinterpret_cast<uintptr_t>(fun) | 1), obj(obj)
    {}
    inline fview_store(void* fun) : fun(reinterpret_cast<uintptr_t>(fun)) {}

    inline void* getFun() const
    {
        return reinterpret_cast<void*>(fun & ~1);
    }
    constexpr bool hasObj() const
    {
        return fun & 1;
    }
    constexpr void* getObj() const
    {
        return obj;
    }
};

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

template <typename R, bool C, bool Nx, typename... Args>
struct function_view_base
{
    template <typename F>
    static inline std::enable_if_t<!C && !std::is_void_v<F>, void*>
        getmem(R (F::*m)(Args...)) noexcept
    {
        return reinterpret_cast<void*>(m);
    }

    template <typename F>
    static inline void* getmem(R (F::*m)(Args...) const) noexcept
    {
        union
        {
            R (F::*mp)(Args...) const;
            void* p;
        };
        mp = m;
        return p;
    }

    struct erasure
    {};

  protected:
    fview_store s;

    template <bool Nx2>
    inline function_view_base(R (*f)(Args...) noexcept(Nx2)) noexcept :
        s(reinterpret_cast<void*>(f))
    {
        static_assert((alignof(decltype(f)) & 1) == 0);
    }

    template <std::invocable<Args...> F,
              typename F_ = std::remove_reference_t<F>>
        requires std::same_as<std::invoke_result_t<F, Args...>, R> &&
                 std::is_reference_v<F>
    inline function_view_base(F&& f) noexcept :
        s(getmem<F_>(&F_::operator()), std::addressof(f))
    {}

    inline function_view_base(fview_store s) noexcept : s(s) {}

  public:
    inline R operator()(Args... args) const noexcept(Nx)
    {
        if (!s.hasObj())
        {
            return reinterpret_cast<R (*)(Args...)>(s.getFun())(
                std::forward<Args>(args)...);
        }
        union
        {
            void* p;
            R (erasure::*m)(Args...);
        };
        m = nullptr;
        p = s.getFun();
        return (reinterpret_cast<erasure*>(s.getObj())->*m)(
            std::forward<Args>(args)...);
    }
};

} // namespace detail

template <typename...>
struct function_view;

template <typename R, typename... Args, bool Nx>
struct function_view<R(Args...) noexcept(Nx)> :
    detail::function_view_base<R, /*const=*/false, Nx, Args...>
{
    using _Base = detail::function_view_base<R, /*const=*/false, Nx, Args...>;

    template <bool Nx2>
    inline function_view(function_view<R(Args...) noexcept(Nx2)> other) noexcept
        :
        _Base(other.s)
    {}

    template <bool Nx2>
    inline function_view(
        function_view<R(Args...) const noexcept(Nx2)> other) noexcept :
        _Base(other.s)
    {}

    template <bool Nx2>
    inline function_view&
        operator=(function_view<R(Args...) noexcept(Nx2)> other) noexcept
    {
        this->s = other.s;
        return *this;
    }

    template <bool Nx2>
    inline function_view&
        operator=(function_view<R(Args...) const noexcept(Nx2)> other) noexcept
    {
        this->s = other.s;
        return *this;
    }

    template <std::invocable<Args...> F>
        requires std::same_as<std::invoke_result_t<F, Args...>, R>
    inline function_view(F&& f) noexcept : _Base(std::forward<F>(f))
    {}
};

template <typename R, typename... Args, bool Nx>
struct function_view<R(Args...) const noexcept(Nx)> :
    detail::function_view_base<R, /*const=*/true, Nx, Args...>
{
    using _Base = detail::function_view_base<R, /*const=*/true, Nx, Args...>;

    template <bool Nx2>
    inline function_view(
        function_view<R(Args...) const noexcept(Nx2)> other) noexcept :
        _Base(other.s)
    {}

    template <bool Nx2>
    inline function_view&
        operator=(function_view<R(Args...) const noexcept(Nx2)> other) noexcept
    {
        this->s = other.s;
        return *this;
    }

    template <std::invocable<Args...> F>
        requires std::same_as<std::invoke_result_t<F, Args...>, R>
    inline function_view(F&& f) noexcept : _Base(std::forward<F>(f))
    {}

    friend class function_view<R(Args...) noexcept(Nx)>;
};

template <typename R, typename... Args, bool Nx>
function_view(R (*)(Args...) noexcept(Nx))
    -> function_view<R(Args...) noexcept(Nx)>;

template <typename F>
function_view(F) -> function_view<
    typename detail::FViewGuide<decltype(&F::operator())>::type>;

} // namespace stdplus
