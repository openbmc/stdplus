#include <fmt/format.h>
#include <sys/stat.h>

#include <stdplus/fd/atomic.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>

#include <cstdlib>
#include <filesystem>
#include <system_error>
#include <utility>

namespace stdplus
{
namespace fd
{

static std::string makeTmpName(const std::filesystem::path& filename)
{
    auto name = filename.filename();
    auto path = filename.parent_path() /
                fmt::format(".{}.XXXXXX", name.native());
    return path.native();
}

static int mktemp(std::string& tmpl)
{
    mode_t old = umask(0077);
    int fd = mkstemp(tmpl.data());
    umask(old);
    return CHECK_ERRNO(fd, [&](int error) {
        throw std::system_error(error, std::generic_category(),
                                fmt::format("mkstemp({})", tmpl));
    });
}

AtomicWriter::AtomicWriter(const std::filesystem::path& filename, int mode,
                           std::string_view tmpl) :
    filename(filename),
    mode(mode),
    tmpname(!tmpl.empty() ? std::string(tmpl) : makeTmpName(filename)),
    fd(mktemp(tmpname))
{}

AtomicWriter::AtomicWriter(AtomicWriter&& other) :
    filename(std::move(other.filename)), mode(other.mode),
    tmpname(std::move(other.tmpname)), fd(std::move(other.fd))
{
    // We don't want to cleanup twice
    other.tmpname.clear();
}

AtomicWriter& AtomicWriter::operator=(AtomicWriter&& other)
{
    if (this != &other)
    {
        filename = std::move(other.filename);
        mode = other.mode;
        tmpname = std::move(other.tmpname);
        fd = std::move(other.fd);

        // We don't want to cleanup twice
        other.tmpname.clear();
    }
    return *this;
}

AtomicWriter::~AtomicWriter()
{
    cleanup();
}

void AtomicWriter::commit(bool allow_copy)
{
    try
    {
        CHECK_ERRNO(fsync(get()), "fsync");
        CHECK_ERRNO(fchmod(get(), mode), "fchmod");
        // We want the file to be closed before renaming it
        {
            auto ifd = std::move(fd);
        }
        try
        {
            std::filesystem::rename(tmpname, filename);
            tmpname.clear();
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            if (!allow_copy || e.code() != std::errc::cross_device_link)
            {
                throw;
            }
            std::filesystem::copy(tmpname, filename);
        }
    }
    catch (...)
    {
        // We do this to ensure that any crashes that might happen before the
        // destructor don't cause us to leave stale files.
        cleanup();
        throw;
    }
}

int AtomicWriter::get() const
{
    return fd.get();
}

void AtomicWriter::cleanup() noexcept
{
    if (!tmpname.empty())
    {
        // Ensure the FD is closed prior to removing the file
        {
            auto ifd = std::move(fd);
        }
        std::error_code ec;
        std::filesystem::remove(tmpname, ec);
        tmpname.clear();
    }
}

} // namespace fd
} // namespace stdplus
