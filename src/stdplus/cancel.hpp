#pragma once
#include <stdplus/handle/managed.hpp>

namespace stdplus
{

struct Cancelable
{
    virtual ~Cancelable() = default;
    virtual void cancel() noexcept = 0;
};
struct CancelableF
{
    inline void operator()(Cancelable* c) noexcept
    {
        c->cancel();
    }
};
using Cancel = stdplus::Managed<Cancelable*>::HandleF<CancelableF>;

} // namespace stdplus
