#include <cstdlib>
#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/util/cexec.hpp>
#include <sys/stat.h>

namespace stdplus
{
namespace fd
{

FormatBuffer::FormatBuffer(stdplus::Fd& fd, size_t max) : fd(fd), max(max)
{
}

FormatBuffer::~FormatBuffer() noexcept(false)
{
    flush();
}

void FormatBuffer::flush()
{
    if (buf.size() > 0)
    {
        stdplus::fd::writeExact(fd, buf);
        buf.clear();
    }
}

void FormatBuffer::writeIfNeeded()
{
    if (buf.size() >= max)
    {
        flush();
    }
}

FormatToFile::FormatToFile(std::string_view tmpl) :
    tmpname(tmpl),
    fd(CHECK_ERRNO(mkstemp(tmpname.data()),
                   [&](int error) {
                       auto msg = fmt::format("mkstemp({})", tmpname);
                       tmpname.clear();
                       throw std::system_error(error, std::generic_category(),
                                               msg);
                   })),
    buf(fd)
{
}

FormatToFile::~FormatToFile()
{
    if (!tmpname.empty())
    {
        std::error_code ec;
        std::filesystem::remove(tmpname, ec);
    }
}

void FormatToFile::commit(const std::filesystem::path& out, int mode)
{
    {
        buf.flush();
        auto ifd = std::move(fd);
    }
    CHECK_ERRNO(chmod(tmpname.c_str(), mode), [&](int error) {
        throw std::system_error(error, std::generic_category(),
                                fmt::format("chmod({}, {})", tmpname, mode));
    });
    std::filesystem::rename(tmpname, out);
    tmpname.clear();
}

} // namespace fd
} // namespace stdplus
