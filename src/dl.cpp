#include <dlfcn.h>

#include <stdplus/dl.hpp>

#include <format>

namespace stdplus
{

Dl::Dl(const char* file, DlOpenFlags flags) :
    handle(open(file, static_cast<int>(flags)))
{}

struct link_map* Dl::linkMap()
{
    struct link_map* ret;
    info(RTLD_DI_LINKMAP, &ret);
    return ret;
}

void Dl::info(int request, void* info)
{
    if (::dlinfo(*handle, request, info) != 0)
    {
        const char* err = dlerror();
        throw std::runtime_error(
            std::format("dlinfo: {}", err ? err : "unknown error"));
    }
}

void* Dl::open(const char* file, int flags)
{
    void* ret = ::dlopen(file, flags);
    if (ret == nullptr)
    {
        const char* err = dlerror();
        throw std::runtime_error(
            std::format("dlopen `{}`: {}", file ? file : "<nullptr>",
                        err ? err : "unknown error"));
    }
    return ret;
}

void Dl::close(void*&& handle) noexcept
{
    ::dlclose(handle);
}

} // namespace stdplus
