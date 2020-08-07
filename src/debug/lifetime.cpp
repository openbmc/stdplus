#include <stdplus/debug/lifetime.hpp>
#include <stdplus/print.hpp>

using std::literals::string_view_literals::operator""sv;

template <typename CharT>
struct std::formatter<std::source_location, CharT>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    constexpr auto format(const auto& v, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}:{}({})", v.file_name(), v.line(),
                              v.function_name());
    }
};

namespace stdplus::debug
{

static std::size_t next_id = 0;

Lifetime::Lifetime(std::source_location loc) : loc(loc), id(next_id++)
{
    stdplus::print(stderr, "Lifetime Construct {} {}\n", loc, id);
}

Lifetime::Lifetime(const Lifetime& other) : loc(other.loc), id(next_id++)
{
    stdplus::print(stderr, "Lifetime Copy {} {}->{}\n", loc, other.id, id);
}

Lifetime::Lifetime(Lifetime&& other) : loc(other.loc), id(next_id++)
{
    stdplus::print(stderr, "Lifetime Move {} {}->{}\n", loc, other.id, id);
}

Lifetime& Lifetime::operator=(const Lifetime& other)
{
    auto old_id = id;
    id = next_id++;
    stdplus::print(stderr, "Lifetime Copy {} {}->{} drop {}\n", loc, other.id,
                   id, old_id);
    return *this;
}

Lifetime& Lifetime::operator=(Lifetime&& other)
{
    auto old_id = id;
    id = next_id++;
    stdplus::print(stderr, "Lifetime Move {} {}->{} drop {}\n", loc, other.id,
                   id, old_id);
    return *this;
}

Lifetime::~Lifetime()
{
    stdplus::print(stderr, "Lifetime Destroy {} {}\n", loc, id);
}

} // namespace stdplus::debug
