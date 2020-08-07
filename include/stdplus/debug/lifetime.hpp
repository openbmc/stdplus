#pragma once
#include <source_location>

namespace stdplus::debug
{

class Lifetime
{
  public:
    Lifetime(std::source_location loc = std::source_location::current());
    Lifetime(const Lifetime& other);
    Lifetime(Lifetime&& other);
    Lifetime& operator=(const Lifetime& other);
    Lifetime& operator=(Lifetime&& other);
    ~Lifetime();

  private:
    std::source_location loc;
    std::size_t id;
};

} // namespace stdplus::debug
