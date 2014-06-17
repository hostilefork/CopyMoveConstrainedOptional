xstd::optional
=============

Copyright 2011 Daniel Kr&uuml;gler.
Distributed under the Boost Software License, Version 1.0. (See
accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)

Alternative to boost::optional for C++11, supporting move-only types and optimizations by detecting type-traits.

Notes from Brian (of http://hostilefork.com) who put this code on GitHub:

> In terms of compatibility with C++11, `boost::optional` lacked support for move-only types (among some other issues).  In 2012 there did not exist a viable reference implementation for a `std::optional` from the [group discussing it](http://kojot.sggw.waw.pl/~akrzemi1/optional/tr2.optional.proposal.html), so Daniel Kr&uuml;gler was willing to share his own implementation called `xstd::optional` under a boost license.  This was not intended to compete with the standards effort, but be a stopgap measure and to experiment with some particular implementation choices.

> As of 2014 there is a reference implementation which is being vetted and known as `std::experimental::optional`.  It is available in another GitHub repository as a single header file:

https://github.com/akrzemi1/Optional/

> Due to issues brought up during review, that specification is not slated to make it into C++14, but should be adopted in a later release.

> Generally speaking the `std::experimental::optional` managed to handle all the cases I was working with.  In order to bring the two into greater alignment, I updated `xstd::optional` to use `nullopt` instead of `none` to indicate an empty optional... and also added the `make_optional` helper function.
    
> I discussed a situation that `xstd::experimental::optional` was not able to handle with Andrzej Krzemie&#324;ski, where `xstd::optional` was able to work.  That specifically related to the type trait of copy constructibility not being inherited by optional from the type it is parameterized with:

https://github.com/akrzemi1/Optional/issues/13

> Andrzej felt that using a technique like Daniel's to provide this ability would complicate the code.  `std::vector` and other classes are subject to the same failure to answer the trait correctly, so it didn't seem like a good tradeoff to do it when the scheduled C++ [Concepts Lite](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3701.pdf) compiler feature would solve it generally.  There may be other differences, but that's the one I noticed.

> As a general heads-up, one should observe that moving the contents out of an optional for a move-only type will not register that optional as being a `nullopt`.  It will consider itself to be containing a value unless you explicitly assign it `nullopt`.  There was an `extract()` function in `xstd::optional` which addressed this, but that is not currently planned for inclusion in `std::experimental::optional`:

https://github.com/akrzemi1/Optional/issues/12

> Many thanks to Daniel and Andrzej for their code and time (and to everyone else working through the details...)
