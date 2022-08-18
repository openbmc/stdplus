#include <liburing.h>

#include <sys/eventfd.h>

#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/io_uring.hpp>
#include <stdplus/util/cexec.hpp>

#include <algorithm>

namespace stdplus
{

__kernel_timespec chronoToKTS(std::chrono::nanoseconds t) noexcept
{
    __kernel_timespec ts;
    ts.tv_sec = std::chrono::floor<std::chrono::seconds>(t).count();
    ts.tv_nsec = (t % std::chrono::seconds(1)).count();
    return ts;
}

IoUring::IoUring(size_t queue_size, int flags)
{
    CHECK_RET(io_uring_queue_init(queue_size, &ring, flags),
              "io_uring_queue_init");
}

IoUring::IoUring(size_t queue_size, io_uring_params& params)
{
    CHECK_RET(io_uring_queue_init_params(queue_size, &ring, &params),
              "io_uring_queue_init_params");
}

IoUring::~IoUring()
{
    io_uring_queue_exit(&ring);
    io_uring_cqe cqe{};
    cqe.res = -ECANCELED;
    for (auto h : handlers)
    {
        h->handleCQE(cqe);
    }
}

IoUring::FileHandle::FileHandle(unsigned slot, IoUring& ring) :
    slot(slot, &ring)
{
}

void IoUring::FileHandle::drop(unsigned&& slot, IoUring*& ring)
{
    ring->updateFile(slot, -1);
}

void IoUring::reserveFiles(size_t num)
{
    auto new_size = std::max<size_t>(7, num + filesAllocated);
    if (files.size() < new_size)
    {
        io_uring_unregister_files(&ring);
        files.resize(new_size, -1);
        CHECK_RET(io_uring_register_files(&ring, files.data(), files.size()),
                  "io_uring_register_files");
    }
}

[[nodiscard]] IoUring::FileHandle IoUring::registerFile(int fd)
{
    unsigned slot = 0;
    for (; slot < files.size(); slot++)
    {
        if (files[slot] == -1)
        {
            updateFile(slot, fd);
            return FileHandle(slot, *this);
        }
    }

    io_uring_unregister_files(&ring);
    files.resize(std::max<size_t>(7, files.size() * 2 + 1), -1);
    setFile(slot, fd);
    CHECK_RET(io_uring_register_files(&ring, files.data(), files.size()),
              "io_uring_register_files");
    return FileHandle(slot, *this);
}

io_uring_sqe& IoUring::getSQE()
{
    return *CHECK_ERRNO(io_uring_get_sqe(&ring), "io_uring_get_sqe");
}

void IoUring::setHandler(io_uring_sqe& sqe, CQEHandler* h) noexcept
{
    auto oldh = reinterpret_cast<CQEHandler*>(sqe.user_data);
    if (oldh == h)
    {
        return;
    }
    io_uring_cqe cqe{};
    cqe.res = -ECANCELED;
    dropHandler(oldh, cqe);
    if (h != nullptr)
    {
        handlers.push_back(h);
    }
    io_uring_sqe_set_data(&sqe, h);
}

void IoUring::cancelHandler(CQEHandler& h)
{
    io_uring_prep_cancel(&getSQE(), &h, 0);
    submit();
}

void IoUring::submit()
{
    CHECK_RET(io_uring_submit(&ring), "io_uring_submit");
}

void IoUring::process() noexcept
{
    io_uring_cqe* cqe;
    while (io_uring_peek_cqe(&ring, &cqe) == 0)
    {
        auto h = reinterpret_cast<CQEHandler*>(cqe->user_data);
        dropHandler(h, *cqe);
        io_uring_cqe_seen(&ring, cqe);
    }
}

void IoUring::wait()
{
    io_uring_cqe* cqe;
    CHECK_RET(io_uring_wait_cqe(&ring, &cqe), "io_uring_wait_cqe");
}

void IoUring::wait(std::chrono::nanoseconds timeout)
{
    io_uring_cqe* cqe;
    auto kts = chronoToKTS(timeout);
    CHECK_RET(io_uring_wait_cqe_timeout(&ring, &cqe, &kts),
              "io_uring_wait_cqe_timeout");
}

ManagedFd& IoUring::getEventFd()
{
    if (event_fd)
    {
        return *event_fd;
    }
    ManagedFd efd(CHECK_RET(eventfd(0, EFD_NONBLOCK), "eventfd"));
    CHECK_RET(io_uring_register_eventfd(&ring, efd.get()),
              "io_uring_register_eventfd");
    return *(event_fd = std::move(efd));
}

void IoUring::processEvents()
{
    auto& efd = getEventFd();
    std::byte b[8];
    while (!fd::read(efd, b).empty())
    {
        process();
    }
}

void IoUring::dropHandler(CQEHandler* h, io_uring_cqe& cqe) noexcept
{
    if (h == nullptr)
    {
        return;
    }
    auto it = std::find(handlers.begin(), handlers.end(), h);
    if (it != handlers.end() - 1)
    {
        std::swap(*it, handlers.back());
    }
    handlers.pop_back();
    h->handleCQE(cqe);
}

void IoUring::setFile(unsigned slot, int fd) noexcept
{
    if (files[slot] == -1 && files[slot] != fd)
    {
        filesAllocated++;
    }
    else if (fd == -1 && files[slot] != fd)
    {
        filesAllocated--;
    }
    files[slot] = fd;
}

void IoUring::updateFile(unsigned slot, int fd)
{
    setFile(slot, fd);
    CHECK_RET(io_uring_register_files_update(&ring, slot, &files[slot], 1),
              "io_uring_register_files_update");
}

} // namespace stdplus
