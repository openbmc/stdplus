#include <stdplus/exception.hpp>

// These will only be used if the compiler doesn't support them
#if !__has_builtin(__builtin_LINE)
int __builtin_LINE()
{
    return -1;
}
const char* __builtin_FILE()
{
    return "<unknown>";
}
const char* __builtin_FUNCTION()
{
    return "<unknown>";
}
#endif

namespace stdplus
{
namespace exception
{

Incomplete::Incomplete(const char* what) :
    std::system_error(std::make_error_code(std::errc::illegal_byte_sequence),
                      what)
{}

Incomplete::Incomplete(const std::string& what) :
    std::system_error(std::make_error_code(std::errc::illegal_byte_sequence),
                      what)
{}

WouldBlock::WouldBlock(const char* what) :
    std::system_error(std::make_error_code(std::errc::operation_would_block),
                      what)
{}

WouldBlock::WouldBlock(const std::string& what) :
    std::system_error(std::make_error_code(std::errc::operation_would_block),
                      what)
{}

Eof::Eof(const char* what) :
    std::system_error(std::make_error_code(std::errc::no_message_available),
                      what)
{}

Eof::Eof(const std::string& what) :
    std::system_error(std::make_error_code(std::errc::no_message_available),
                      what)
{}

} // namespace exception
} // namespace stdplus
