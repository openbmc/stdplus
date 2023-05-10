#include <dlfcn.h>
#include <fmt/format.h>

#include <stdplus/dl.hpp>

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
        throw std::runtime_error("dlinfo");
    };
}

void* Dl::open(const char* file, int flags)
{
    void* ret = ::dlopen(file, flags);
    if (ret == nullptr)
    {
        throw std::runtime_error(fmt::format(
            "dlopen `{}`: {}", file ? file : "<nullptr>", dlerror()));
    }
    return ret;
}

void Dl::close(void*&& handle)
{
    ::dlclose(handle);
}

} // namespace stdplus
