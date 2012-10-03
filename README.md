xstd::optional
=============

Copyright 2011 Daniel Kr&uuml;gler.
Distributed under the Boost Software License, Version 1.0. (See
accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)

Alternative to boost::optional for C++11, supporting move-only types and optimizations
by detecting type-traits.

Notes from Brian (of http://hostilefork.com) who published the code to GitHub:

	"As of 2012, boost::optional is behind the times...no support for move-only types and
	there are other issues.  Trying to patch it is difficult, because boost has a lot of
	legacy concerns and cannot dive into c++11.  So if a codebase is purely c++11 and
	needs a "smarter" optional, the best thing may be  to throw it out and start all over.

    Daniel did this, and was willing to share his code under a boost license.  His
    desire was to share his work if it was useful--but *not* to compete with the existing
    efforts to build std::optional for the next standard.  That work is documented and
    discussed extensively on Google Groups:

    http://kojot.sggw.waw.pl/~akrzemi1/optional/tr2.optional.proposal.html

    Yet the present state of the reference implementation was not working for my use
    cases.  Daniel's code worked better for me, and since there was some back-and-forth
    regarding issues I encountered--we began tracking and exchanging our modifications
    through version control (Git + GitHub).  Yet keep in mind the goal is not to become
    popular, merely to inform the design of the next standard through practice.

    Many thanks to Daniel for his code and his time!"
