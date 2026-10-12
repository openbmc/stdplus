#pragma once
#include <stdplus/fd/intf.hpp>

#include <array>
#include <functional>
#include <span>
#include <string>

namespace stdplus
{
namespace fd
{

class LineReader
{
  public:
    static constexpr size_t buf_size = 4096;

    LineReader(const Fd& fd);
    LineReader(const LineReader&) = delete;
    LineReader(LineReader&&) = delete;
    LineReader& operator=(const LineReader&) = delete;
    LineReader& operator=(LineReader&&) = delete;

    const std::string* readLine();

  private:
    std::reference_wrapper<const Fd> fd;
    std::array<char, buf_size> buf;
    std::span<char> buf_data;
    std::string line;
    bool line_complete = false, hit_eof = false;
};

} // namespace fd
} // namespace stdplus
