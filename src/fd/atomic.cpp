#include <cstdlib>
#include <filesystem>
#include <fmt/format.h>
#include <stdplus/fd/atomic.hpp>
#include <stdplus/fd/create.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>
#include <sys/stat.h>
#include <system_error>
#include <utility>

namespace stdplus
{
namespace fd
{

namespace detail
{
bool tmpfile_support = true;
}

static std::string makeTmpName(const std::filesystem::path& filename)
{
    auto name = filename.filename();
    auto path =
        filename.parent_path() / fmt::format(".{}.XXXXXX", name.native());
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

static ManagedFd makeTmpFile(std::string& name, bool& exists)
{
    try
    {
        if (!detail::tmpfile_support)
        {
            throw std::system_error(
                std::make_error_code(std::errc::operation_not_supported), "");
        }
        auto parent = std::filesystem::path(name).parent_path();
        return open(parent.native(),
                    OpenFlags(OpenAccess::WriteOnly).set(OpenFlag::TmpFile),
                    0600);
    }
    catch (const std::system_error& e)
    {
        if (e.code() != std::errc::operation_not_supported)
        {
            throw;
        }
        auto fd = mktemp(name);
        exists = true;
        return ManagedFd{std::move(fd)};
    }
}

AtomicWriter::AtomicWriter(const std::filesystem::path& filename, int mode,
                           std::string_view tmpl) :
    filename(filename),
    mode(mode), tmpexists(false),
    tmpname(!tmpl.empty() ? std::string(tmpl) : makeTmpName(filename)),
    fd(makeTmpFile(tmpname, tmpexists))
{
}

AtomicWriter::AtomicWriter(AtomicWriter&& other) :
    filename(std::move(other.filename)), mode(other.mode),
    tmpexists(other.tmpexists), tmpname(std::move(other.tmpname)),
    fd(std::move(other.fd))
{
    // We don't want to cleanup twice
    other.tmpexists = false;
}

AtomicWriter& AtomicWriter::operator=(AtomicWriter&& other)
{
    if (this != &other)
    {
        filename = std::move(other.filename);
        mode = other.mode;
        tmpexists = other.tmpexists;
        tmpname = std::move(other.tmpname);
        fd = std::move(other.fd);

        // We don't want to cleanup twice
        other.tmpexists = false;
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
        // If we have an O_TMPFILE type file we need to give it a name
        if (!tmpexists)
        {
            // This is possibly racy since we can't link over an existing file.
            // Just retry a few times if the file already exists.
            size_t i = 0;
            while (true)
            {
                close(mktemp(tmpname));
                std::filesystem::remove(tmpname);
                auto name = std::filesystem::path(tmpname).filename();
                auto ret =
                    linkat(get(), "", AT_FDCWD, name.c_str(), AT_EMPTY_PATH);
                if (ret > 0)
                {
                    tmpexists = true;
                    break;
                }
                else if (errno != EEXIST)
                {
                    throw std::system_error(
                        errno, std::generic_category(),
                        fmt::format("linkat({}, , AT_FDCWD, {}, AT_EMPTY_PATH)",
                                    get(), name.native()));
                }
                else if (i++ > 10)
                {
                    throw std::runtime_error("Failed to name tmpfile");
                }
            }
        }
        CHECK_ERRNO(fsync(get()), "fsync");
        CHECK_ERRNO(fchmod(get(), mode), "fchmod");
        // We want the file to be closed before renaming it
        {
            auto ifd = std::move(fd);
        }
        try
        {
            std::filesystem::rename(tmpname, filename);
            tmpexists = false;
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

const std::string& AtomicWriter::getTmpname() const
{
    static const std::string empty;
    return tmpexists ? tmpname : empty;
}

int AtomicWriter::get() const
{
    return fd.get();
}

void AtomicWriter::cleanup() noexcept
{
    if (tmpexists)
    {
        // Ensure the FD is closed prior to removing the file
        {
            auto ifd = std::move(fd);
        }
        std::error_code ec;
        std::filesystem::remove(tmpname, ec);
        tmpexists = false;
    }
}

} // namespace fd
} // namespace stdplus
