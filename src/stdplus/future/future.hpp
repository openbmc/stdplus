#pragma once
#include <exception>
#include <function2/function2.hpp>
#include <optional>
#include <utility>
#include <variant>

namespace stdplus
{
namespace future
{

template <typename T>
class Promise;

/** @brief A future implementation that is meant to closely follow the API
 *         of std::future where possible, but intends to provide a usable
 *         interface for single threaded event loops.
 *
 *  @detail This future is coupled to an equivalent promise, with all of the
 *          data stored by the future. Each future / promise pair is always
 *          unique, and can only have data set once. Unlike std::future, the
 *          data can be referenced multiple times out of this class so it
 *          doesn't need to be moved or copied. It is always safe to set the
 *          value on the promise side, even if the lifetime of the
 *          corresponding future has expired. None of the functions on the
 *          promise are valid until the get_future() is called, and is only
 *          allowed exactly once. An optional wakeup function can be specified
 *          that will be called exactly once, when the promise sets the future
 *          value.
 *
 *          Example:
 *            Promise<int> p;
 *            Future<int> f = p.get_future();
 *            assert(!f.is_ready());
 *            p.set_value(1);
 *            assert(f.is_ready());
 *            fprintf("%d\n", f.get());
 *            fprintf("%d\n", f.get());
 */
template <typename T>
class Future
{
  public:
    /** @brief The type of wakeup function we run when the promise is set */
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

    /** @brief Configures the wakeup function used to notify the waiter on
     *         this future. Typically an event loop will set this so that it
     *         can be woken to execute the task depending on the future.
     *
     *  @param[in] wakeup - The function to be executed.
     */
    void set_wakeup(Wakeup&& wakeup)
    {
        this->wakeup = std::move(wakeup);
    }

    /** @brief Polls the future to see if it has a value yet.
     *
     *  @return True if ready, false otherwise.
     */
    bool is_ready() const noexcept
    {
        return maybeV.has_value();
    }

    /** @brief Gets the value, or throws the exception stored in the future.
     *
     *  @throws std::bad_optional_access if the future is not ready
     *  @throws The exception stored by the future
     *  @return The value stored in the future
     */
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

    /** @brief A reference to the coupled promise or null if the promise has
     *         expired.
     */
    Promise<T>* p;
    /** @brief Maybe the value / exception for this future. */
    std::optional<std::variant<T, std::exception_ptr>> maybeV;
    /** @brief Maybe the wakeup function to instruct an event loop to process
     *         this future.
     */
    Wakeup wakeup;

    /** @brief Constructs a new future from a given promise */
    Future(Promise<T>& p) noexcept : p(&p){};

    /** @brief Updates the reference to our future inside the corresponding
     *         promise. Used to deal with the future moving.
     *
     *  @param[in] f - The future reference to be set in the promise
     */
    void update_promise(Future* f) noexcept
    {
        if (p != nullptr)
        {
            p->f = f;
        }
    }
};

} // namespace future
} // namespace stdplus
