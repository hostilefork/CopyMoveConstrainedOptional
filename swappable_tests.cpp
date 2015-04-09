// Copyright 2011 Daniel Kr&uuml;gler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "is_swappable.hpp"

//------------------
namespace ns1 {
  struct SwapThrow {};
  void swap(SwapThrow&, SwapThrow&);
}

namespace ns2 {
  struct SwapThrow {
    SwapThrow(const SwapThrow&) noexcept(false);
    SwapThrow& operator=(const SwapThrow&) noexcept(false);
  };
}

namespace ns3 {
  struct SwapNoThrow {
    SwapNoThrow(const SwapNoThrow&) noexcept(false);
    SwapNoThrow& operator =(const SwapNoThrow&) noexcept(false);
  };
  void swap(SwapNoThrow&, SwapNoThrow&) noexcept;
}

namespace ns4 {
  struct SwapNoThrow {};
}

namespace ns5 {
  struct SwapThrow {
    SwapThrow(SwapThrow&&) noexcept;
    SwapThrow& operator=(const SwapThrow&) noexcept(false);
  };
}

namespace ns6 {
  struct NoSwap1 {};

  void swap(NoSwap1&, NoSwap1&) = delete;

  struct NoSwap2 {
    friend void swap(NoSwap2&, NoSwap2&) = delete;
  };

  struct NoSwap3 {};

  void swap(NoSwap3&&, NoSwap3&&){}

  struct NoSwap4 {};

  void swap(const NoSwap4&, const NoSwap4&) = delete;
}
//------------------

static_assert(xstd::is_swappable<int>::value, "Error");
static_assert(!xstd::is_swappable<void>::value, "Error");
static_assert(!xstd::is_swappable<const volatile void>::value, "Error");
static_assert(!xstd::is_swappable<void () const>::value, "Error");
static_assert(!xstd::is_swappable<ns6::NoSwap1>::value, "Error");
static_assert(!xstd::is_swappable<ns6::NoSwap2>::value, "Error");
static_assert(xstd::is_swappable<ns6::NoSwap3>::value, "Error");
static_assert(xstd::is_swappable<ns6::NoSwap4>::value, "Error");

static_assert(xstd::is_nothrow_swappable<int>::value, "Error");
static_assert(!xstd::is_nothrow_swappable_with<int, int>::value, "Error");
static_assert(xstd::is_nothrow_swappable_with<int&, int&>::value, "Error");
static_assert(!xstd::is_nothrow_swappable_with<const int&, int&>::value, "Error");
static_assert(!xstd::is_nothrow_swappable_with<const int&, const int&>::value, "Error");
static_assert(!xstd::is_nothrow_swappable_with<int&, double&>::value, "Error");
static_assert(!xstd::is_nothrow_swappable_with<void, void>::value, "Error");
static_assert(!xstd::is_nothrow_swappable_with<const void, volatile void>::value, "Error");
static_assert(!xstd::is_nothrow_swappable_with<void() const, void()>::value, "Error");

static_assert(xstd::is_nothrow_swappable<int[3]>::value, "Error");

static_assert(!xstd::is_nothrow_swappable<ns1::SwapThrow>::value, "Error");
static_assert(!xstd::is_nothrow_swappable<ns2::SwapThrow>::value, "Error");
static_assert(xstd::is_nothrow_swappable<ns3::SwapNoThrow>::value, "Error");
static_assert(xstd::is_nothrow_swappable<ns4::SwapNoThrow>::value, "Error");
static_assert(!xstd::is_nothrow_swappable<ns5::SwapThrow>::value, "Error");

namespace u {
  struct X;
  struct Y;
  struct Z;
}

template<class T>
void do_swap(T& t) noexcept(xstd::is_nothrow_swappable<T>::value);

namespace u {
  struct X { };
  struct Y { Y(const Y&); };
  struct Z { Z(Z&&); Z& operator=(Z&&); };
  void swap(Z&, Z&) noexcept;
}

static_assert(noexcept(do_swap(std::declval<u::X&>())), "Error");
static_assert(!noexcept(do_swap(std::declval<u::Y&>())), "Error");
static_assert(noexcept(do_swap(std::declval<u::Z&>())), "Error");

namespace m {
  struct Some {
    friend void swap(Some&, Some&){}
  };

  struct Proxy {
    friend void swap(Some&, Proxy) noexcept {}
  };
  void swap(Proxy, Some&) noexcept;
}

static_assert(!xstd::is_nothrow_swappable<m::Some>::value, "Error");
static_assert(xstd::is_nothrow_swappable_with<m::Some&, m::Proxy>::value, "Error");
static_assert(xstd::is_nothrow_swappable_with<m::Proxy, m::Some&>::value, "Error");

void the_test()
{
  using std::swap;
  ns6::NoSwap3 a3, b3;
  swap(a3, b3);
  ns6::NoSwap4 a4, b4;
  swap(a4, b4);
}



