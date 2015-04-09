#ifndef IS_SWAPPABLE_HPP_INCLUDED
#define IS_SWAPPABLE_HPP_INCLUDED

// Copyright 2011 Daniel Kr&uuml;gler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <utility>
#include <type_traits>

namespace xstd {
namespace details {
namespace private_std_swap_area {

using std::swap;

struct do_is_swappable
{
  template<class T, class =
    decltype(swap(std::declval<T&>(), std::declval<T&>()))
  >
  static std::true_type test(int);

  template<class>
  static std::false_type test(...);
};

struct do_is_nothrow_swappable
{
  template<class T>
  static auto test(int) -> std::integral_constant<bool,
    noexcept(swap(std::declval<T&>(), std::declval<T&>()))
  >;

  template<class>
  static std::false_type test(...);
};

struct do_is_swappable_with
{
  template<class T, class U
  , class =
    decltype(swap(std::declval<T>(), std::declval<U>()))
  , class =
    decltype(swap(std::declval<U>(), std::declval<T>()))
  >
  static std::true_type test(int);

  template<class, class>
  static std::false_type test(...);
};

struct do_is_nothrow_swappable_with
{
  template<class T, class U>
  static auto test(int) -> std::integral_constant<bool,
    noexcept(swap(std::declval<T>(), std::declval<U>()))
    &&
    noexcept(swap(std::declval<U>(), std::declval<T>()))
  >;

  template<class, class>
  static std::false_type test(...);
};

} // ns private_std_swap_area

template<class T>
struct is_swappable_impl : decltype(
  private_std_swap_area::do_is_swappable::test<T>(0)
)
{};

template<class T>
struct is_nothrow_swappable_impl : decltype(
  private_std_swap_area::do_is_nothrow_swappable::test<T>(0)
)
{};

template<class T, class U>
struct is_swappable_with_impl : decltype(
  private_std_swap_area::do_is_swappable_with::test<T, U>(0)
)
{};

template<class T, class U>
struct is_nothrow_swappable_with_impl : decltype(
  private_std_swap_area::do_is_nothrow_swappable_with::test<T, U>(0)
)
{};

} // ns details

template<class T>
struct is_swappable : details::is_swappable_impl<T>
{
};

template<class T>
struct is_nothrow_swappable : details::is_nothrow_swappable_impl<T>
{
};

template<class T, class U>
struct is_swappable_with : details::is_swappable_with_impl<T, U>
{
};

template<class T, class U>
struct is_nothrow_swappable_with : details::is_nothrow_swappable_with_impl<T, U>
{
};

} // ns xstd

#endif // IS_SWAPPABLE_HPP_INCLUDED
