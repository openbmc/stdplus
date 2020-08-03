#pragma once
#include <utility>

namespace stdplus
{

template <typename Int, typename Flag = Int>
class BitFlags
{
  public:
    inline explicit BitFlags(Int val = 0) : val(val)
    {
    }

    inline BitFlags& set(Flag flag) &
    {
        val |= static_cast<Int>(flag);
        return *this;
    }
    inline BitFlags&& set(Flag flag) &&
    {
        val |= static_cast<Int>(flag);
        return std::move(*this);
    }

    inline BitFlags& unset(Flag flag) &
    {
        val &= ~static_cast<Int>(flag);
        return *this;
    }
    inline BitFlags&& unset(Flag flag) &&
    {
        val &= ~static_cast<Int>(flag);
        return std::move(*this);
    }

    explicit inline operator Int() const
    {
        return val;
    }

  private:
    Int val;
};

} // namespace stdplus
