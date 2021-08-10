#include <liburing.h>

#include <linux/io_uring.h>
#include <sys/eventfd.h>

#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/io_uring.hpp>
#include <stdplus/util/cexec.hpp>

#include <algorithm>

namespace stdplus
{

IoUring::IoUring(size_t queue_size)
{
    CHECK_RET(io_uring_queue_init(queue_size, &ring, 0), "io_uring_queue_init");
}

IoUring::IoUring(size_t queue_size, io_uring_params* params)
{
    CHECK_RET(io_uring_queue_init_params(queue_size, &ring, params),
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

stdplus::ManagedFd& IoUring::getEventFd()
{
    if (event_fd)
    {
        return *event_fd;
    }
    stdplus::ManagedFd efd(CHECK_RET(eventfd(0, EFD_NONBLOCK), "eventfd"));
    CHECK_RET(io_uring_register_eventfd(&ring, efd.get()),
              "io_uring_register_eventfd");
    return *(event_fd = std::move(efd));
}

void IoUring::processEvents()
{
    auto& efd = getEventFd();
    std::byte b[8];
    while (!stdplus::fd::read(efd, b).empty())
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

} // namespace stdplus
