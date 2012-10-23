#ifndef TOOLS_HPP_INCLUDED
#define TOOLS_HPP_INCLUDED

// Copyright 2011-2012 Daniel Kr&uuml;gler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>

namespace xstd {
namespace details {
namespace tool_detect_traits {
// Declare but do not define
template<class> struct is_trivially_default_constructible;
template<class> struct has_trivial_default_constructor;
template<class> struct is_trivially_copy_constructible;
template<class> struct has_trivial_copy_constructor;
template<class> struct is_trivially_copy_assignable;
template<class> struct has_trivial_copy_assign;
template<class> struct is_trivially_destructible;
template<class> struct has_trivial_destructor;
}
}
}

namespace std {
// Minimalistic extension of namespace std
namespace xstd_tool_detect_traits_8680E938_DBC2_4ACF_896E_4E58D393C904 {

// Import names, but those from namespace std will be preferred:
using namespace xstd::details::tool_detect_traits;

struct trivially_default_constructible
{
  template<class T, bool = is_trivially_default_constructible<T>::value>
  static is_trivially_default_constructible<T> test(int);

  template<class T, bool = has_trivial_default_constructor<T>::value>
  static has_trivial_default_constructor<T> test(...);
};

struct trivially_copy_constructible
{
  template<class T, bool = is_trivially_copy_constructible<T>::value>
  static is_trivially_copy_constructible<T> test(int);

  template<class T, bool = has_trivial_copy_constructor<T>::value>
  static has_trivial_copy_constructor<T> test(...);
};

struct trivially_copy_assignable
{
  template<class T, bool = is_trivially_copy_assignable<T>::value>
  static is_trivially_copy_assignable<T> test(int);

  template<class T, bool = has_trivial_copy_assign<T>::value>
  static has_trivial_copy_assign<T> test(...);
};

struct trivially_destructible
{
  template<class T, bool = is_trivially_destructible<T>::value>
  static is_trivially_destructible<T> test(int);

  template<class T, bool = has_trivial_destructor<T>::value>
  static has_trivial_destructor<T> test(...);
};

}
}

namespace xstd {

template<class...>
struct and_;

template<>
struct and_<> : std::true_type
{
};

template<class P1, class... Pn>
struct and_<P1, Pn...> : std::conditional<P1::value,
  and_<Pn...>, std::false_type>::type
{
};

template<class P>
struct not_ : std::integral_constant<bool, !P::value>
{
};

namespace details_traits = std::xstd_tool_detect_traits_8680E938_DBC2_4ACF_896E_4E58D393C904;

template<class T>
using is_trivially_default_constructible = decltype(details_traits::trivially_default_constructible::test<T>(0));

template<class T>
using is_trivially_copy_constructible = decltype(details_traits::trivially_copy_constructible::test<T>(0));

template<class T>
using is_trivially_copy_assignable = decltype(details_traits::trivially_copy_assignable::test<T>(0));

template<class T>
using is_trivially_destructible = decltype(details_traits::trivially_destructible::test<T>(0));

}

#endif // TOOLS_HPP_INCLUDED
