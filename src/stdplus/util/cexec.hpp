#pragma once
#include <functional>
#include <system_error>
#include <type_traits>
#include <utility>

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
inline std:system_error makeSystemError(int error, const char* msg)
{
    return std::system_error(error, std::generic_category(), msg);
}

/** @brief   Wraps common c style error handling for exception throwing
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
template <auto (*makeError)(int, const char*) = makeSystemError,
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

/** @brief   Wraps common c style error handling for exception throwing
 *           See callCheckErrno() for details. The difference is this returns
 *           -r instead of errno.
 *
 *  @param[in] msg     - The error message displayed when errno is set.
 *  @param[in] func    - The wrapped function we invoke
 *  @param[in] args... - The arguments passed to the function
 *  @throws std::system_error for an error case.
 *  @return A successful return value based on the function type
 */
template <auto (*makeError)(int, const char*) = makeSystemError,
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

} // namespace util
} // namespace stdplus
