#include <stdplus/exception.hpp>

namespace stdplus
{
namespace exception
{

WouldBlock::WouldBlock(const char* what) :
    std::system_error(std::make_error_code(std::errc::operation_would_block),
                      what)
{
}

WouldBlock::WouldBlock(const std::string& what) :
    std::system_error(std::make_error_code(std::errc::operation_would_block),
                      what)
{
}

Eof::Eof(const char* what) :
    std::system_error(std::make_error_code(std::errc::no_message_available),
                      what)
{
}

Eof::Eof(const std::string& what) :
    std::system_error(std::make_error_code(std::errc::no_message_available),
                      what)
{
}

} // namespace exception
} // namespace stdplus
