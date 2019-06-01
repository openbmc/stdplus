#pragma once
#include <exception>
#include <stdplus/future/future.hpp>
#include <utility>

namespace stdplus
{
namespace future
{

template <typename T>
class Future;

/** @brief A promise implementation that is meant to closely follow the API
 *         of std::promise where possible, but intends to provide a usable
 *         interface for single threaded event loops. See future.hpp for more
 *         info.
 */
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

    /** @brief Generates a new future object corresponding to this promise.
     *
     *  @throws std::logic_error - If the future was already retrieved.
     *  @return The new future
     */
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

    /** @brief Sets an arbitrary exception in the future, checking that the
     *         future was already retrieved and that it doesn't have a value
     *         already. This ignores futures that have been destroyed early.
     *         Wakes up the future holder.
     *
     *  @param[in] p - The exception to set on the future
     *  @throws std::logic_error - If the future was not retrieved or value set
     */
    void set_exception(std::exception_ptr p)
    {
        set_future(p);
    }

    /** @brief Sets an arbitrary value in the future, checking that the future
     *         was already retrieved and that it doesn't have a value already.
     *         This ignores futures that have been destroyed early. Wakes up
     *         the future holder.
     *
     *  @param[in] value - The value to set on the future
     *  @throws std::logic_error - If the future was not retrieved or value set
     */
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

    /** @brief A reference to the coupled future or null if the future has
     *         expired.
     */
    Future<T>* f;
    /** @brief Used to validate the corresponding future was retrieved. */
    bool gotFuture = false;

    /** @brief Sets an arbitrary value or exception in the future, checking
     *         that the future was already retrieved and that it doesn't have
     *         a value already. This ignores futures that have been destroyed
     *         early. Wakes up the future holder.
     *
     *  @param[in] value - The value or exception
     *  @throws std::logic_error - If the future was not retrieved or value set
     */
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

    /** @brief Updates the reference to our promise inside the corresponding
     *         future. Used to deal with the promise moving.
     *
     *  @param[in] p - The promise reference to be set in the future
     */
    void update_future(Promise* p) noexcept
    {
        if (f != nullptr)
        {
            f->p = p;
        }
    }
};

} // namespace future
} // namespace stdplus
