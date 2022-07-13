#pragma once
#include <functional>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

/** @brief   Wraps common c style error handling for exception throwing
 *           This requires the callee to set errno on error.
 *  @details We often have a pattern in our code for checking errors and
 *           propagating up exceptions:
 *
 *           int c_call(const char* path);
 *
 *           int our_cpp_call(const char* path)
 *           {
 *               int r = c_call(path);
 *               if (r < 0)
 *               {
 *                   throw std::system_error(errno, std::generic_category(),
 *                                           "our msg");
 *               }
 *               return r;
 *           }
 *
 *           To make that more succinct, we can use CHECK_ERRNO:
 *
 *           int our_cpp_call(const char* path)
 *           {
 *               return CHECK_ERRNO(c_call(path), "our msg");
 *           }
 *
 *  @param[in] expr          - The expression returning an errno value
 *  @param[in] error_handler - Passed to the doError function
 *  @throws std::system_error (depends on error_handler) for an error case
 *  @return A successful return value based on the function type
 */
#define CHECK_ERRNO(expr, error_handler)                                       \
    [&](auto check_errno_ret) {                                                \
        if constexpr (::std::is_pointer_v<decltype(check_errno_ret)>)          \
        {                                                                      \
            if (check_errno_ret == nullptr)                                    \
                ::stdplus::util::doError(errno, (error_handler));              \
        }                                                                      \
        else if constexpr (::std::is_signed_v<decltype(check_errno_ret)>)      \
        {                                                                      \
            if (check_errno_ret < 0)                                           \
                ::stdplus::util::doError(errno, (error_handler));              \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            static_assert(::std::is_same_v<decltype(check_errno_ret), int>,    \
                          "Unimplemented check routine");                      \
        }                                                                      \
        return check_errno_ret;                                                \
    }((expr))

/** @brief   Wraps common c style error handling for exception throwing
 *           This requires the callee to provide error information in -r.
 *           See CHECK_ERRNO() for details.
 *
 *  @param[in] expr          - The expression returning an negative error value
 *  @param[in] error_handler - Passed to the doError function
 *  @throws std::system_error (depends on error_handler) for an error case
 *  @return A successful return value based on the function type
 */
#define CHECK_RET(expr, error_handler)                                         \
    [&](auto check_ret_ret) {                                                  \
        if constexpr (::std::is_signed_v<decltype(check_ret_ret)>)             \
        {                                                                      \
            if (check_ret_ret < 0)                                             \
                ::stdplus::util::doError(-check_ret_ret, (error_handler));     \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            static_assert(::std::is_same_v<decltype(check_ret_ret), int>,      \
                          "Unimplemented check routine");                      \
        }                                                                      \
        return check_ret_ret;                                                  \
    }((expr))

namespace stdplus
{
namespace util
{

/** @brief   Common pattern used by default for constructing a system exception
 *  @details Most libc or system calls will want to return a generic
 *           system_error when detecting an error in a call. This function
 *           creates that error from the errno and message.
 *
 *  @param[in] error -
 *  @param[in] msg   -
 *  @return The exception passed to a `throw` call.
 */
inline auto makeSystemError(int error, const char* msg)
{
    return std::system_error(error, std::generic_category(), msg);
}

/** @brief Turns errors into exceptions for the given error handler
 *
 *  @param[in] error - The numeric system error code
 *  @param[in] msg   - The string used to describe the error
 *  @throws A system exception with the given error and msg
 */
inline void doError(int error, const char* msg)
{
    throw makeSystemError(error, msg);
}
inline void doError(int error, const std::string& msg)
{
    throw makeSystemError(error, msg.c_str());
}

/** @brief Turns errors into exceptions for the given error handler
 *
 *  @param[in] error - The numeric system error code
 *  @param[in] fun   - the function used to throw the error
 */
template <typename Fun>
inline std::enable_if_t<std::is_invocable_v<Fun, int>, void> doError(int error,
                                                                     Fun&& fun)
{
    fun(error);
}

/** @brief   Wraps common c style error handling for exception throwing
 *           This requires the callee to set errno on error.
 *  @details We often have a pattern in our code for checking errors and
 *           propagating up exceptions:
 *
 *           int c_call(const char* path);
 *
 *           int our_cpp_call(const char* path)
 *           {
 *               int r = c_call(path);
 *               if (r < 0)
 *               {
 *                   throw std::system_error(errno, std::generic_category(),
 *                                           "our msg");
 *               }
 *               return r;
 *           }
 *
 *           To make that more succinct, we can use callCheckErrno:
 *
 *           int our_cpp_call(const char* path)
 *           {
 *               return callCheckErrno("our msg", c_call, path);
 *           }
 *
 *  @param[in] msg     - The error message displayed when errno is set.
 *  @param[in] func    - The wrapped function we invoke
 *  @param[in] args... - The arguments passed to the function
 *  @throws std::system_error for an error case.
 *  @return A successful return value based on the function type
 */
template <auto(*makeError)(int, const char*) = makeSystemError,
          typename... Args>
inline auto callCheckErrno(const char* msg, Args&&... args)
{
    using Ret = typename std::invoke_result<Args...>::type;

    if constexpr (std::is_integral_v<Ret> && std::is_signed_v<Ret>)
    {
        Ret r = std::invoke(std::forward<Args>(args)...);
        if (r < 0)
            throw makeError(errno, msg);
        return r;
    }
    else if constexpr (std::is_pointer_v<Ret>)
    {
        Ret r = std::invoke(std::forward<Args>(args)...);
        if (r == nullptr)
            throw makeError(errno, msg);
        return r;
    }
    else
    {
        static_assert(std::is_same_v<Ret, int>, "Unimplemented check routine");
    }
}
template <auto(*makeError)(int, const char*) = makeSystemError,
          typename... Args>
inline auto callCheckErrno(const std::string& msg, Args&&... args)
{
    return callCheckErrno(msg.c_str(), std::forward<Args>(args)...);
}

/** @brief   Wraps common c style error handling for exception throwing
 *           This requires the callee to provide error information in -r.
 *           See callCheckErrno() for details.
 *
 *  @param[in] msg     - The error message displayed when errno is set.
 *  @param[in] func    - The wrapped function we invoke
 *  @param[in] args... - The arguments passed to the function
 *  @throws std::system_error for an error case.
 *  @return A successful return value based on the function type
 */
template <auto(*makeError)(int, const char*) = makeSystemError,
          typename... Args>
inline auto callCheckRet(const char* msg, Args&&... args)
{
    using Ret = typename std::invoke_result<Args...>::type;

    if constexpr (std::is_integral_v<Ret> && std::is_signed_v<Ret>)
    {
        Ret r = std::invoke(std::forward<Args>(args)...);
        if (r < 0)
            throw makeError(-r, msg);
        return r;
    }
    else
    {
        static_assert(std::is_same_v<Ret, int>, "Unimplemented check routine");
    }
}
template <auto(*makeError)(int, const char*) = makeSystemError,
          typename... Args>
inline auto callCheckRet(const std::string& msg, Args&&... args)
{
    return callCheckRet(msg.c_str(), std::forward<Args>(args)...);
}

} // namespace util
} // namespace stdplus
