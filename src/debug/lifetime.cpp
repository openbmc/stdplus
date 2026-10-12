#include <stdplus/debug/lifetime.hpp>
#include <stdplus/print.hpp>

#include <atomic>
#include <format>

namespace stdplus::debug
{
namespace
{
struct Loc
{
    std::source_location v;
};
} // namespace
} // namespace stdplus::debug

template <typename CharT>
struct std::formatter<stdplus::debug::Loc, CharT>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    constexpr auto format(const stdplus::debug::Loc& l,
                          FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}:{}({})", l.v.file_name(),
                              l.v.line(), l.v.function_name());
    }
};

namespace stdplus::debug
{

static std::atomic<std::size_t> next_id = 0;

Lifetime::Lifetime(std::source_location loc) : loc(loc), id(next_id++)
{
    stdplus::print(stderr, "Lifetime Construct {} {}\n", Loc{loc}, id);
}

Lifetime::Lifetime(const Lifetime& other) : loc(other.loc), id(next_id++)
{
    stdplus::print(stderr, "Lifetime Copy {} {}->{}\n", Loc{loc}, other.id, id);
}

Lifetime::Lifetime(Lifetime&& other) : loc(other.loc), id(next_id++)
{
    stdplus::print(stderr, "Lifetime Move {} {}->{}\n", Loc{loc}, other.id, id);
}

Lifetime& Lifetime::operator=(const Lifetime& other)
{
    auto old_id = id;
    id = next_id++;
    stdplus::print(stderr, "Lifetime Copy {} {}->{} drop {}\n", Loc{loc},
                   other.id, id, old_id);
    return *this;
}

Lifetime& Lifetime::operator=(Lifetime&& other)
{
    auto old_id = id;
    id = next_id++;
    stdplus::print(stderr, "Lifetime Move {} {}->{} drop {}\n", Loc{loc},
                   other.id, id, old_id);
    return *this;
}

Lifetime::~Lifetime()
{
    stdplus::print(stderr, "Lifetime Destroy {} {}\n", Loc{loc}, id);
}

} // namespace stdplus::debug
