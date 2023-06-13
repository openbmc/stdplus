#pragma once
#include <cstdint>
#include <limits>
#include <memory>
#include <string_view>

namespace stdplus
{

namespace detail
{

template <typename CharT, std::size_t BufLen, auto Endian = std::endian::native>
union StrBufStore;

template <typename CharT, std::size_t BufLen>
union StrBufStore<CharT, BufLen, std::endian::little>
{
    struct Inl
    {
        CharT ptr[BufLen];
        std::make_unsigned_t<CharT> len;
        static_assert(BufLen <= std::numeric_limits<decltype(len)>::max() / 2);
    } inl;
    struct Dyn
    {
        std::uint8_t
            rsvd[offsetof(Inl, len) - sizeof(CharT*) - sizeof(std::size_t) * 2];
        CharT* ptr;
        std::size_t cap;
        std::size_t len;
    } dyn;
    static_assert(offsetof(Inl, len) + sizeof(Inl::len) ==
                  offsetof(Dyn, len) + sizeof(Dyn::len));

    constexpr StrBufStore() noexcept
    {
        if (std::is_constant_evaluated())
        {
            std::construct_at(&dyn);
        }
    }
    StrBufStore(StrBufStore&&) = delete;
    StrBufStore(const StrBufStore&) = delete;
    StrBufStore& operator=(StrBufStore&&) = delete;
    StrBufStore& operator=(const StrBufStore&) = delete;

    static inline constexpr auto dyn_mask = std::size_t{1}
                                            << ((sizeof(std::size_t) << 3) - 1);
    static inline constexpr auto inl_mask = decltype(Inl::len){1}
                                            << ((sizeof(Inl::len) << 3) - 1);

    constexpr bool isDyn() const noexcept
    {
        if (std::is_constant_evaluated())
        {
            return true;
        }
        return inl.len & inl_mask;
    }

    constexpr std::size_t dynLen() const noexcept
    {
        return dyn.len & ~dyn_mask;
    }

    constexpr void dynLen(std::size_t len) noexcept
    {
        dyn.len = len | dyn_mask;
    }

    constexpr auto inlLen() const noexcept
    {
        return inl.len;
    }

    constexpr void inlLen(decltype(Inl::len) len) noexcept
    {
        inl.len = len;
    }
};

template <typename CharT, std::size_t ObjSize, typename Allocator>
struct StrBufAS : Allocator
{
    static inline constexpr std::size_t buf_len =
        (ObjSize -
         sizeof(std::declval<detail::StrBufStore<CharT, 63>>().inl.len) -
         (std::is_empty_v<Allocator> ? 0 : sizeof(Allocator))) /
        sizeof(CharT);
    using Store = detail::StrBufStore<CharT, buf_len>;
    static_assert(sizeof(std::declval<Store>().inl) >=
                  sizeof(std::declval<Store>().dyn));

    constexpr CharT* allocate(std::size_t n)
    {
        auto ptr = static_cast<Allocator&>(*this).allocate(n);
        if (std::is_constant_evaluated())
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                std::construct_at(ptr + i);
            }
        }
        return ptr;
    }

    constexpr void zeroInit() noexcept
    {
        if (std::is_constant_evaluated())
        {
            store.dyn.ptr = allocate(buf_len);
            store.dyn.cap = buf_len;
            store.dynLen(0);
        }
        else
        {
            store.inlLen(0);
        }
    }

    constexpr StrBufAS(Allocator&& a) noexcept : Allocator(std::move(a))
    {
        zeroInit();
    }

    constexpr StrBufAS& operator=(Allocator&& a) noexcept
    {
        *static_cast<Allocator*>(this) = a;
        return *this;
    }

    Store store;
};

} // namespace detail

template <typename CharT, std::size_t ObjSize = 128,
          typename Allocator = std::allocator<CharT>>
class BasicStrBuf
{
  private:
    detail::StrBufAS<CharT, ObjSize, Allocator> as;

    template <bool assign>
    constexpr void inlcopy(const BasicStrBuf& other) noexcept
    {
        const auto optr = other.as.store.inl.ptr;
        const auto olen = other.as.store.inlLen();
        if (assign || std::is_constant_evaluated())
        {
            if (as.store.isDyn())
            {
                as.store.dynLen(olen);
                std::copy(optr, optr + olen, as.store.dyn.ptr);
                return;
            }
        }
        as.store.inl.len = other.as.store.inl.len;
        std::copy(optr, optr + olen, as.store.inl.ptr);
    }

    template <bool assign>
    constexpr void move(BasicStrBuf&& other) noexcept
    {
        if (!other.as.store.isDyn())
        {
            inlcopy<assign>(other);
            other.as.zeroInit();
            return;
        }
        if (assign || std::is_constant_evaluated())
        {
            if (as.store.isDyn())
            {
                as.deallocate(as.store.dyn.ptr, as.store.dyn.cap);
            }
        }
        as.store.dyn = other.as.store.dyn;
        other.as.zeroInit();
    }

    template <bool assign>
    constexpr void copy(const BasicStrBuf& other)
    {
        if (!other.as.store.isDyn())
        {
            inlcopy<assign>(other);
            return;
        }
        const auto optr = other.as.store.dyn.ptr;
        const std::size_t olen = other.as.store.dynLen();
        if (assign || std::is_constant_evaluated())
        {
            if (as.store.isDyn())
            {
                if (olen <= as.store.dyn.cap)
                {
                    as.store.dyn.len = other.as.store.dyn.len;
                    std::copy(optr, optr + olen, as.store.dyn.ptr);
                    return;
                }
                else
                {
                    as.deallocate(as.store.dyn.ptr, as.store.dyn.cap);
                }
            }
        }
        if (olen <= as.buf_len)
        {
            std::copy(optr, optr + olen, as.store.inl.ptr);
            as.store.inlLen(olen);
            return;
        }
        as.store.dyn.cap = other.as.store.dyn.cap;
        as.store.dyn.ptr = as.allocate(as.store.dyn.cap);
        as.store.dyn.len = other.as.store.dyn.len;
        std::copy(optr, optr + olen, as.store.dyn.ptr);
    }

  public:
    using value_type = CharT;

    constexpr BasicStrBuf() noexcept : as({}) {}

    constexpr BasicStrBuf(BasicStrBuf&& other) noexcept :
        as(static_cast<Allocator&&>(other.as))
    {
        move</*assign=*/false>(std::move(other));
    }

    constexpr BasicStrBuf(const BasicStrBuf& other) :
        as(std::allocator_traits<Allocator>::
               select_on_container_copy_construction(
                   static_cast<const Allocator&>(other.as)))
    {
        copy</*assign=*/false>(other);
    }

    constexpr BasicStrBuf& operator=(BasicStrBuf&& other) noexcept
    {
        if (this != &other)
        {
            if constexpr (typename std::allocator_traits<Allocator>::
                              propagate_on_container_move_assignment())
            {
                as = static_cast<Allocator&&>(other.as);
            }
            move</*assign=*/true>(std::move(other));
        }
        return *this;
    }

    constexpr BasicStrBuf& operator=(const BasicStrBuf& other)
    {
        if (this != &other)
        {
            if constexpr (typename std::allocator_traits<Allocator>::
                              propagate_on_container_copy_assignment())
            {
                as = static_cast<const Allocator&>(other.as);
            }
            copy</*assign=*/true>(other);
        }
        return *this;
    }

    constexpr ~BasicStrBuf()
    {
        if (as.store.isDyn())
        {
            as.deallocate(as.store.dyn.ptr, as.store.dyn.cap);
        }
    }

    constexpr std::size_t size() const noexcept
    {
        return as.store.isDyn() ? as.store.dynLen() : as.store.inlLen();
    }

    constexpr operator std::basic_string_view<CharT>() const noexcept
    {
        return std::basic_string_view<CharT>(begin(), size());
    }

    constexpr CharT* append(std::size_t amt)
    {
        if (!as.store.isDyn())
        {
            const std::size_t oldlen = as.store.inlLen();
            const std::size_t newlen = oldlen + amt;
            if (newlen <= as.buf_len)
            {
                as.store.inlLen(newlen);
                return as.store.inl.ptr + oldlen;
            }
            const std::size_t newcap = newlen + (newlen >> 1);
            const auto ptr = as.allocate(newcap);
            std::copy(as.store.inl.ptr, as.store.inl.ptr + oldlen, ptr);

            as.store.dyn.ptr = ptr;
            as.store.dyn.cap = newcap;
            as.store.dynLen(newlen);
            return ptr + oldlen;
        }
        const std::size_t oldlen = as.store.dynLen();
        const std::size_t newlen = oldlen + amt;
        if (newlen > as.store.dyn.cap)
        {
            const std::size_t newcap = newlen + (newlen >> 1);
            const auto ptr = as.allocate(newcap);
            std::copy(as.store.dyn.ptr, as.store.dyn.ptr + oldlen, ptr);
            as.deallocate(as.store.dyn.ptr, as.store.dyn.cap);

            as.store.dyn.ptr = ptr;
            as.store.dyn.cap = newcap;
        }
        as.store.dynLen(newlen);
        return as.store.dyn.ptr + oldlen;
    }

    constexpr void shrink(std::size_t amt) noexcept
    {
        if (as.store.isDyn())
        {
            as.store.dyn.len -= amt;
        }
        else
        {
            as.store.inl.len -= amt;
        }
    }

    constexpr void reset() noexcept
    {
        if (as.store.isDyn())
        {
            as.store.dynLen(0);
        }
        else
        {
            as.store.inlLen(0);
        }
    }

    constexpr CharT* begin() noexcept
    {
        return as.store.isDyn() ? as.store.dyn.ptr : as.store.inl.ptr;
    }

    constexpr const CharT* begin() const noexcept
    {
        return as.store.isDyn() ? as.store.dyn.ptr : as.store.inl.ptr;
    }

    constexpr CharT* end() noexcept
    {
        return begin() + size();
    }

    constexpr const CharT* end() const noexcept
    {
        return begin() + size();
    }

    constexpr const CharT* cbegin() const noexcept
    {
        return begin();
    }

    constexpr const CharT* cend() const noexcept
    {
        return end();
    }

    constexpr bool
        operator==(std::basic_string_view<CharT> other) const noexcept
    {
        return std::basic_string_view<CharT>{*this} == other;
    }
};

static_assert(sizeof(BasicStrBuf<char, 128>) == 128);
static_assert(sizeof(BasicStrBuf<wchar_t, 256>) == 256);

using StrBuf = BasicStrBuf<char>;
using WStrBuf = BasicStrBuf<wchar_t>;

} // namespace stdplus
