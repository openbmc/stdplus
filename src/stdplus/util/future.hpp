#pragma once
#include <chrono>
#include <exception>
#include <function2/function2.hpp>
#include <future>
#include <optional>
#include <tuple>
#include <utility>
#include <variant>

namespace stdplus
{
namespace util
{

template <typename T>
class Future;

template <typename T>
class Promise
{
  public:
    Promise() noexcept : f(nullptr){};
    Promise(const Promise& other) = delete;
    Promise(Promise&& other) noexcept : f(other.f), gotFuture(other.gotFuture)
    {
        other.f = nullptr;
        update_future(this);
    }
    Promise& operator=(const Promise& other) = delete;
    Promise& operator=(Promise&& other) noexcept
    {
        f = other.f;
        gotFuture = other.gotFuture;
        other.f = nullptr;
        update_future(this);
        return *this;
    }
    ~Promise() noexcept
    {
        if (f != nullptr && !f->maybeV)
        {
            set_future(std::make_exception_ptr(
                std::logic_error("Promise dropped without set")));
        }
        update_future(nullptr);
    }

    Future<T> get_future()
    {
        if (f != nullptr)
        {
            throw std::logic_error("Called get_future twice");
        }
        Future<T> ret(*this);
        f = &ret;
        gotFuture = true;
        return ret;
    }

    void set_exception(std::exception_ptr p)
    {
        set_future(p);
    }

    void set_value(const T& value)
    {
        set_future(value);
    }

    void set_value(T&& value)
    {
        set_future(std::move(value));
    }

  private:
    friend class Future<T>;

    Future<T>* f;
    bool gotFuture = false;

    template <typename R>
    void set_future(R&& value)
    {
        if (!gotFuture)
        {
            throw std::logic_error("Tried to set future value before creation");
        }
        if (f != nullptr)
        {
            if (f->maybeV)
            {
                throw std::logic_error("Tried to set future value twice");
            }
            f->maybeV.emplace(std::forward<R>(value));
            if (f->wakeup != nullptr)
            {
                f->wakeup(*f);
            }
        }
    }

    void update_future(Promise* p) noexcept
    {
        if (f != nullptr)
        {
            f->p = p;
        }
    }
};

template <typename T>
class Future
{
  public:
    using Wakeup = fu2::unique_function<void(Future<T>&)>;

    Future(const Future& other) = delete;
    Future(Future&& other) noexcept :
        p(other.p), maybeV(std::move(other.maybeV)),
        wakeup(std::move(other.wakeup))
    {
        other.p = nullptr;
        update_promise(this);
    }
    Future& operator=(const Future& other) = delete;
    Future& operator=(Future&& other) noexcept
    {
        p = other.p;
        maybeV = std::move(other.maybeV);
        wakeup = std::move(other.wakeup);
        other.p = nullptr;
        update_promise(this);
        return *this;
    }
    ~Future() noexcept
    {
        update_promise(nullptr);
    }

    void set_wakeup(Wakeup&& wakeup)
    {
        this->wakeup = std::move(wakeup);
    }

    bool is_ready() const noexcept
    {
        return maybeV.has_value();
    }

    T& get()
    {
        auto& v = maybeV.value();
        if (std::holds_alternative<std::exception_ptr>(v))
        {
            std::rethrow_exception(std::get<std::exception_ptr>(v));
        }
        return std::get<T>(v);
    }

    const T& get() const
    {
        const auto& v = maybeV.value();
        if (std::holds_alternative<std::exception_ptr>(v))
        {
            std::rethrow_exception(std::get<std::exception_ptr>(v));
        }
        return std::get<T>(v);
    }

  private:
    friend class Promise<T>;

    Promise<T>* p;
    std::optional<std::variant<T, std::exception_ptr>> maybeV;
    Wakeup wakeup;

    Future(Promise<T>& p) noexcept : p(&p){};
    void update_promise(Future* f) noexcept
    {
        if (p != nullptr)
        {
            p->f = f;
        }
    }
};

/** @brief Determines if a future is ready to be used without blocking
 *
 *  @param[in] future - The future we are checking
 *  @return True if ready to read
 */
template <typename T>
bool isReady(const std::future<T>& future)
{
    // TODO(wak): Replace wait_for() with is_ready() when stabilized
    using namespace std::chrono_literals;
    return future.wait_for(0s) == std::future_status::ready;
}

/** @brief Determines if a future is ready to be used without blocking
 *
 *  @param[in] future - The future we are checking
 *  @return True if ready to read
 */
template <typename T>
bool isReady(const Future<T>& future)
{
    return future.is_ready();
}

/** @brief Determines if all futures in an iterable container are ready
 *         to be used without blocking
 *
 *  @param[in] futures - The futures we are checking
 *  @return True if all are ready to read
 */
template <typename C, typename = typename C::const_iterator>
bool isReady(const C& futures)
{
    for (const auto& future : futures)
    {
        if (!isReady(future))
        {
            return false;
        }
    }
    return true;
}

/** @brief Determines if all futures are ready to be used without blocking
 *
 *  @param[in] futures... - The futures we are checking
 *  @return True if all are ready to read
 */
template <typename... Futures>
bool allReady(const Futures&... futures)
{
    return (... && isReady(futures));
}

/** @brief Determines if all futures in a tuple are ready to be used
 *         without blocking
 *
 *  @param[in] futures - The futures we are checking
 *  @return True if all are ready to read
 */
template <typename... Futures>
bool isReady(const std::tuple<Futures...>& futures)
{
    return std::apply(allReady<Futures...>, futures);
}

} // namespace util
} // namespace stdplus
