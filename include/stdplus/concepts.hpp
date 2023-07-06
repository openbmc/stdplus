#pragma once
#include <concepts>
#include <type_traits>

namespace stdplus
{

template <typename T, typename U>
concept WeaklyEqualityComparableWith = requires(
    const std::remove_reference_t<T>& t, const std::remove_reference_t<U>& u) {
                                           {
                                               t == u
                                           } -> std::convertible_to<bool>;
                                           {
                                               t != u
                                           } -> std::convertible_to<bool>;
                                           {
                                               u == t
                                           } -> std::convertible_to<bool>;
                                           {
                                               u != t
                                           } -> std::convertible_to<bool>;
                                       };

}
