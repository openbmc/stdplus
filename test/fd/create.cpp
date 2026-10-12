#include <sys/socket.h>

#include <stdplus/fd/create.hpp>

#include <system_error>

#include <gtest/gtest.h>

namespace stdplus::fd
{

TEST(Create, Open)
{
    auto fd = open("/dev/null",
                   OpenFlags(OpenAccess::ReadOnly).set(OpenFlag::CloseOnExec));
    EXPECT_GE(fd.get(), 0);
    EXPECT_NE(static_cast<int>(fd.fcntlGetfd()) &
                  static_cast<int>(FdFlag::CloseOnExec),
              0);
    EXPECT_THROW(open("/stdplus/nonexistent/path", OpenAccess::ReadOnly),
                 std::system_error);
}

TEST(Create, Socket)
{
    static_assert(static_cast<int>(SocketFlag::CloseOnExec) == SOCK_CLOEXEC);
    static_assert(static_cast<int>(SocketFlag::NonBlock) == SOCK_NONBLOCK);
    auto fd =
        socket(SocketDomain::UNIX,
               SocketFlags(SocketType::Datagram).set(SocketFlag::CloseOnExec),
               SocketProto::IP);
    EXPECT_GE(fd.get(), 0);
    EXPECT_NE(static_cast<int>(fd.fcntlGetfd()) &
                  static_cast<int>(FdFlag::CloseOnExec),
              0);
    EXPECT_THROW(socket(static_cast<SocketDomain>(-1), SocketType::Stream,
                        SocketProto::IP),
                 std::system_error);
}

} // namespace stdplus::fd
