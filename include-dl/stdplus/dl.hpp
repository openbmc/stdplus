#pragma once
#include <dlfcn.h>
#include <link.h>

#include <stdplus/flags.hpp>
#include <stdplus/handle/managed.hpp>

namespace stdplus
{

enum class DlOpenType : int
{
    Lazy = RTLD_LAZY,
    Now = RTLD_NOW,
};

enum class DlOpenFlag : int
{
    Global = RTLD_GLOBAL,
    Local = RTLD_LOCAL,
    NoDelete = RTLD_NODELETE,
    NoLoad = RTLD_NOLOAD,
    DeepBind = RTLD_DEEPBIND,
};

class DlOpenFlags : public stdplus::BitFlags<DlOpenFlag>
{
  public:
    constexpr DlOpenFlags(DlOpenType type) :
        BitFlags<DlOpenFlag>(std::to_underlying(type))
    {}

    constexpr DlOpenFlags(BitFlags<DlOpenFlag> flags) :
        BitFlags<DlOpenFlag>(flags)
    {}
};

class Dl
{
  public:
    Dl(const char* filename, DlOpenFlags flags);

    struct link_map* linkMap();

  private:
    void info(int request, void* info);

    static void* open(const char* filename, int flags);
    static void close(void*&& handle);
    stdplus::Managed<void*>::Handle<close> handle;
};

} // namespace stdplus
