#pragma once
#include <stdplus/handle/managed.hpp>

#include <tuple>
#include <utility>

namespace stdplus
{

struct Cancelable
{
    virtual ~Cancelable() = default;
    virtual void cancel() noexcept = 0;
};

namespace detail
{

struct CancelableF
{
    inline void operator()(Cancelable* c) noexcept
    {
        c->cancel();
    }
};

using CancelHandle = Managed<Cancelable*>::HandleF<CancelableF>;

} // namespace detail

struct Cancel : detail::CancelHandle
{
    Cancel() : detail::CancelHandle(std::nullopt) {}
    template <typename T>
    explicit Cancel(T&& t) : detail::CancelHandle(t)
    {}
};

namespace detail
{

struct fAny
{};
struct fPtr : fAny
{};

template <typename>
struct validator
{
    typedef int type;
};

template <typename F,
          typename validator<decltype(std::declval<F>() == nullptr)>::type = 0>
inline bool fPop(const F& f, fPtr)
{
    return !(f == nullptr);
}

template <typename F>
inline bool fPop(const F&, fAny)
{
    return true;
}

template <typename F>
inline bool fPop(const F& f)
{
    return fPop(f, fPtr());
}

} // namespace detail

template <typename F, typename... DefaultArgs>
class AlwaysCallOnce
{
  public:
    template <typename Fi, typename... Vs>
    explicit AlwaysCallOnce(Fi&& fi, Vs&&... default_args) :
        f(std::forward<Fi>(fi)), default_args(std::forward<Vs>(default_args)...)
    {}
    AlwaysCallOnce(const AlwaysCallOnce&) = delete;
    AlwaysCallOnce(AlwaysCallOnce&& other) noexcept :
        f(std::move(other.f)), default_args(std::move(other.default_args)),
        called(std::exchange(other.called, true))
    {}
    AlwaysCallOnce& operator=(const AlwaysCallOnce&) = delete;
    AlwaysCallOnce& operator=(AlwaysCallOnce&& other) noexcept
    {
        finalCall();
        f = std::move(other.f);
        default_args = std::move(other.default_args);
        called = std::exchange(other.called, true);
        return *this;
    }
    ~AlwaysCallOnce()
    {
        finalCall();
    }

    template <typename... Args>
    auto operator()(Args&&... args) noexcept
    {
        called = true;
        return f(std::forward<Args>(args)...);
    }

  private:
    F f;
    std::tuple<DefaultArgs...> default_args;
    bool called = false;

    void finalCall() noexcept
    {
        if (!called && detail::fPop(f))
        {
            std::apply(f, default_args);
        }
    }
};

template <typename... Args>
inline auto alwaysCallOnce(Args&&... args)
{
    return AlwaysCallOnce<std::remove_cv_t<std::remove_reference_t<Args>>...>(
        std::forward<Args>(args)...);
}

} // namespace stdplus
