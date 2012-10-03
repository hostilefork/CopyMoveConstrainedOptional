#ifndef TOOLS_HPP_INCLUDED
#define TOOLS_HPP_INCLUDED

// Copyright 2011 Daniel Kr&uuml;gler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>

namespace xstd {

template<class...>
struct and_;

template<>
struct and_<> : std::true_type
{
};

template<class P>
struct and_<P> : P
{
};

template<class P1, class P2>
struct and_<P1, P2> : std::conditional<P1::value, P2, P1>::type
{
};

template<class P1, class P2, class P3, class... Pn>
struct and_<P1, P2, P3, Pn...> : std::conditional<P1::value, and_<P2, P3, Pn...>, P1>::type
{
};

template<class P>
struct not_ : std::integral_constant<bool, !P::value>
{
};

}

#endif // TOOLS_HPP_INCLUDED
