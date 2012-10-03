#ifndef OPTIONAL_HPP_INCLUDED
#define OPTIONAL_HPP_INCLUDED

// Copyright 2011 Daniel Kr&uuml;gler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <type_traits>
#include <utility>
#include <new>
#include <cstddef> // std::nullptr_t

#include "tools.hpp"
#include "is_swappable.hpp"

namespace xstd {

template<class>
class optional;

namespace details {

enum class member_policy
{
  none = 0,
  copy = 1 << 0,
  move = 1 << 1,
};

constexpr member_policy operator&(member_policy x, member_policy y)
{
  using int_type = std::underlying_type<member_policy>::type;
  return static_cast<member_policy>(static_cast<int_type>(x) & static_cast<int_type>(y));
}

constexpr member_policy operator|(member_policy x, member_policy y)
{
  using int_type = std::underlying_type<member_policy>::type;
  return static_cast<member_policy>(static_cast<int_type>(x) | static_cast<int_type>(y));
}

constexpr member_policy operator^(member_policy x, member_policy y)
{
  using int_type = std::underlying_type<member_policy>::type;
  return static_cast<member_policy>(static_cast<int_type>(x) ^ static_cast<int_type>(y));
}

constexpr member_policy operator~(member_policy x)
{
  using int_type = std::underlying_type<member_policy>::type;
  return static_cast<member_policy>(~static_cast<int_type>(x));
}

template<member_policy, class T, class D>
struct ctor_base;

template<class T, class D>
struct ctor_base<member_policy::none, T, D>
{
  constexpr ctor_base() noexcept = default;
  ctor_base(const ctor_base&) = delete;
  ctor_base(ctor_base&&) = delete;
  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

template<class T, class D>
struct ctor_base<member_policy::move | member_policy::copy, T, D>
{
  constexpr ctor_base() noexcept = default;

  ctor_base(const ctor_base& rhs) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    static_cast<D&>(*this).copy_construct(static_cast<const D&>(rhs));
  }

  ctor_base(ctor_base&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    static_cast<D&>(*this).move_construct(std::move(static_cast<D&&>(rhs)));
  }

  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

template<class T, class D>
struct ctor_base<member_policy::copy, T, D>
{
  constexpr ctor_base() noexcept = default;

  ctor_base(const ctor_base& rhs) noexcept(std::is_nothrow_copy_constructible<T>::value)
  {
    static_cast<D&>(*this).copy_construct(static_cast<const D&>(rhs));
  }

  ctor_base(ctor_base&&) = delete;

  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

template<class T, class D>
struct ctor_base<member_policy::move, T, D>
{
  constexpr ctor_base() noexcept = default;

  ctor_base(const ctor_base&) = delete;

  ctor_base(ctor_base&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    static_cast<D&>(*this).move_construct(std::move(static_cast<D&&>(rhs)));
  }

  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

template<member_policy, class T, class D>
struct assign_base;

template<class T, class D>
struct assign_base<member_policy::none, T, D>
{
  constexpr assign_base() noexcept = default;
  assign_base(const assign_base&) noexcept = default;
  assign_base(assign_base&&) noexcept = default;
  assign_base& operator=(const assign_base&) = delete;
  assign_base& operator=(assign_base&&) = delete;
};

template<class T, class D>
struct assign_base<member_policy::move | member_policy::copy, T, D>
{
  constexpr assign_base() noexcept = default;
  assign_base(const assign_base&) noexcept = default;
  assign_base(assign_base&&) noexcept = default;

  assign_base& operator=(const assign_base& rhs) noexcept(
     and_<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_assignable<T>>::value
  )
  {
    static_cast<D&>(*this).copy_assign(static_cast<const D&>(rhs));
    return *this;
  }

  assign_base& operator=(assign_base&& rhs) noexcept(
     and_<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_assignable<T>>::value
  )
  {
    static_cast<D&>(*this).move_assign(static_cast<D&&>(rhs));
    return *this;
  }
};

template<class T, class D>
struct assign_base<member_policy::copy, T, D>
{
  constexpr assign_base() noexcept = default;
  assign_base(const assign_base&) noexcept = default;
  assign_base(assign_base&&) noexcept = default;

  assign_base& operator=(const assign_base& rhs) noexcept(
     and_<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_assignable<T>>::value
  )
  {
    static_cast<D&>(*this).copy_assign(static_cast<const D&>(rhs));
    return *this;
  }

  assign_base& operator=(assign_base&&) = delete;
};

template<class T, class D>
struct assign_base<member_policy::move, T, D>
{
  constexpr assign_base() noexcept = default;
  assign_base(const assign_base&) noexcept = default;
  assign_base(assign_base&&) noexcept = default;

  assign_base& operator=(const assign_base&) = delete;

  assign_base& operator=(assign_base&& rhs) noexcept(
     and_<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_assignable<T>>::value
  )
  {
    static_cast<D&>(*this).move_assign(static_cast<D&&>(rhs));
    return *this;
  }
};

template<class T, class D>
using select_ctor_base = ctor_base<
    (std::is_copy_constructible<T>::value ? member_policy::copy : member_policy::none) |
    (std::is_move_constructible<T>::value ? member_policy::move : member_policy::none),
    T, D
>;

template<class T, class D>
using select_assign_base = assign_base<
    (and_<std::is_copy_constructible<T>, std::is_copy_assignable<T>>::value ? member_policy::copy : member_policy::none) |
    (and_<std::is_move_constructible<T>, std::is_move_assignable<T>>::value ? member_policy::move : member_policy::none),
    T, D
>;

struct no_init_t {};

constexpr no_init_t no_init{};

template<class T>
union optional_storage
{
  unsigned char for_value_init;
  typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type actual_data;
};

template<class T>
union literal_optional_storage
{
  unsigned char for_value_init;
  typename std::remove_cv<T>::type actual_data;
  constexpr literal_optional_storage(no_init_t) : for_value_init() {}
  // generation of default copy constructor of raw data requires we have handling
  // for literal_optional_storage(literal_optional_storage&) - a catch-all forwarding
  // constructor cannot differentiate between initializing the actual_data member
  // and that case.
  template<typename FirstArg, class... RestArgs>
  constexpr literal_optional_storage(typename std::enable_if<(sizeof...(RestArgs) > 0) and not std::is_convertible<literal_optional_storage, FirstArg>::value, FirstArg>::type firstarg, RestArgs&&... restargs) :
      actual_data(firstarg, std::forward<RestArgs>(restargs)...) {}
  constexpr literal_optional_storage(literal_optional_storage const & other) : actual_data(other.actual_data) {}
};

// http://stackoverflow.com/questions/12702103/writing-code-that-works-when-has-trivial-destructor-is-defined-instead-of-is
#ifdef TRIVIAL_DESTRUCTOR_TYPE_TRAIT_NOT_STANDARD
template<class T>
using is_trivially_destructible = std::has_trivial_destructor<T>;
#else
template<class T>
using is_trivially_destructible = std::is_trivially_destructible<T>;
#endif

static_assert(is_trivially_destructible<int>::value, "");
static_assert(is_trivially_destructible<optional_storage<int>>::value, "");

template<class T, bool = is_trivially_destructible<T>::value>
class optional_data
{
  using raw_type = optional_storage<T>;

  bool is_init_;
  raw_type raw_data_;

public:
  constexpr optional_data() noexcept : is_init_(false), raw_data_{} {}

  optional_data(const optional_data&) noexcept : is_init_(false) {}

  optional_data(optional_data&&) noexcept : is_init_(false) {}

  optional_data& operator=(const optional_data&) noexcept {
    return *this;
  }

  optional_data& operator=(optional_data&&) noexcept {
    return *this;
  }

  // NB: Actually the explicit exception-specification is not
  // required, but the current gcc snap-shot behaves incorrect
  // in the derived class, if not doing so.
  ~optional_data() noexcept
  {
    this->destroy();
  }

  auto is_initialized() const noexcept -> bool
  {
    return this->is_init_;
  }

  auto raw_memory() noexcept -> void*
  {
    return &this->raw_data_;
  }

  auto raw_memory() const noexcept -> const void*
  {
    return &this->raw_data_;
  }

  auto data() noexcept -> T&
  {
    return *static_cast<T*>(raw_memory());
  }

  auto data() const noexcept -> const T&
  {
    return *static_cast<const T*>(raw_memory());
  }

  template<class... U>
  void construct(U&&... u)
  {
    assert(!this->is_initialized());
    ::new (this->raw_memory()) T(std::forward<U>(u)...);
    this->is_init_ = true;
  }

  template<class U>
  void assign(U&& u)
  {
    if (this->is_initialized())
    {
      this->data() = std::forward<U>(u);
    }
    else
    {
      this->construct(std::forward<U>(u));
    }
  }

  void destroy() noexcept
  {
    if (this->is_init_)
    {
      this->data().~T();
      this->is_init_ = false;
    }
  }
};

// Partial specialization for trivially-destructible types. It also
// is a trivially-destructible type:
template<class T>
class optional_data<T, true>
{
  using raw_type = literal_optional_storage<T>;

  bool is_init_;
  raw_type raw_data_;

public:
  constexpr optional_data() noexcept : is_init_(false), raw_data_(no_init) {}

  constexpr optional_data(const optional_data&) : is_init_(false), raw_data_(no_init) {}

  constexpr optional_data(optional_data&&) : is_init_(false), raw_data_(no_init) {}

  optional_data& operator=(const optional_data&) = default;

  optional_data& operator=(optional_data&&) = default;

  ~optional_data() noexcept = default;

  constexpr auto is_initialized() const noexcept -> bool
  {
    return this->is_init_;
  }

  auto data() noexcept -> T&
  {
    return raw_data_.actual_data;
  }

  constexpr auto data() const noexcept -> const T&
  {
    return raw_data_.actual_data;
  }

  template<class... U>
  void construct(U&&... u)
  {
    assert(!this->is_initialized());
    ::new (static_cast<void*>(&raw_data_.actual_data)) T(std::forward<U>(u)...);
    this->is_init_ = true;
  }

  template<class U>
  void assign(U&& u)
  {
    if (this->is_initialized())
    {
      this->data() = std::forward<U>(u);
    }
    else
    {
      this->construct(std::forward<U>(u));
    }
  }

  void destroy() noexcept
  {
    this->is_init_ = false;
  }
};

template<class>
struct is_optional_impl : std::false_type
{
};

template<class T>
struct is_optional_impl<optional<T>> : std::true_type
{
};

template<class T>
struct is_optional : is_optional_impl<typename std::remove_cv<
  typename std::remove_reference<T>::type>::type
>
{
};

template<class T, class U>
using is_equivalent = std::is_same<
  typename std::remove_cv<typename std::remove_reference<T>::type>::type,
  typename std::remove_cv<typename std::remove_reference<U>::type>::type
>;

} // ns details

struct inplace_t {};

constexpr inplace_t inplace{};

struct none_t {};

constexpr none_t none{};

template<class T>
class optional : private details::optional_data<T>,
                 private details::select_ctor_base<T, optional<T>>,
                 private details::select_assign_base<T, optional<T>>
{
  using copy_type = details::select_ctor_base<T, optional>;
  using assign_type = details::select_assign_base<T, optional>;
  friend copy_type;
  friend assign_type;
  template<class>
  friend class optional;

  void copy_construct(const optional& rhs)
  {
    if (rhs.is_initialized())
    {
      this->construct(rhs.data());
    }
    else
    {
      assert(!this->is_initialized());
    }
  }

  void move_construct(optional&& rhs)
  {
    if (rhs.is_initialized())
    {
      this->construct(std::move(rhs.data()));
    }
    else
    {
      assert(!this->is_initialized());
    }
  }

  void copy_assign(const optional& rhs)
  {
    if (rhs.is_initialized())
    {
      this->assign(rhs.data());
    }
    else
    {
      this->destroy();
    }
  }

  void move_assign(optional&& rhs)
  {
    if (rhs.is_initialized())
    {
      this->assign(std::move(rhs.data()));
    }
    else
    {
      this->destroy();
    }
  }

public:
  using value_type = T;

  constexpr optional() noexcept = default;

  constexpr optional(none_t) noexcept {}

  template<class U,
    typename std::enable_if<
      and_<
        not_<details::is_equivalent<U, optional<T>>>,
        std::is_convertible<U, T>,
        std::is_constructible<T, U>
      >::value,
      bool
    >::type = false
  >
  optional(U&& u) noexcept(std::is_nothrow_constructible<T, U>::value)
  {
    this->construct(std::forward<U>(u));
  }

  template<class U,
    typename std::enable_if<
      and_<
        not_<details::is_equivalent<U, optional<T>>>,
        not_<std::is_convertible<U, T>>,
        std::is_constructible<T, U>
      >::value,
      bool
    >::type = false
  >
  explicit optional(U&& u) noexcept(std::is_nothrow_constructible<T, U>::value)
  {
    this->construct(std::forward<U>(u));
  }

  template<class... U,
    typename std::enable_if<
      std::is_constructible<T, U...>::value,
      bool
    >::type = false
  >
  optional(inplace_t, U&&... u) noexcept(std::is_nothrow_constructible<T, U...>::value)
  {
    this->construct(std::forward<U>(u)...);
  }

  template<class U,
    typename std::enable_if<
      and_<
        std::is_convertible<const U&, T>,
        std::is_constructible<T, const U&>
      >::value,
      bool
    >::type = false
  >
  optional(const optional<U>& u) noexcept(std::is_nothrow_constructible<T, const U&>::value)
  {
    this->construct(u.data());
  }

  template<class U,
    typename std::enable_if<
      and_<
        not_<std::is_convertible<const U&, T>>,
        std::is_constructible<T, const U&>
      >::value,
      bool
    >::type = false
  >
  explicit optional(const optional<U>& u) noexcept(std::is_nothrow_constructible<T, const U&>::value)
  {
    this->construct(u.data());
  }

  template<class U,
    typename std::enable_if<
      and_<
        std::is_convertible<U&&, T>,
        std::is_constructible<T, U&&>
      >::value,
      bool
    >::type = false
  >
  optional(optional<U>&& u) noexcept(std::is_nothrow_constructible<T, U>::value)
  {
    this->construct(std::move(u.data()));
  }

  template<class U,
    typename std::enable_if<
      and_<
        not_<std::is_convertible<U&&, T>>,
        std::is_constructible<T, U&&>
      >::value,
      bool
    >::type = false
  >
  explicit optional(optional<U>&& u) noexcept(std::is_nothrow_constructible<T, U>::value)
  {
    this->construct(std::move(u.data()));
  }

  optional& operator=(none_t) noexcept
  {
    this->destroy();
    return *this;
  }

  template<class U,
    typename std::enable_if<
      and_<
       not_<details::is_equivalent<U, optional<T>>>,
       std::is_constructible<T, U>,
        std::is_assignable<T&, U>
      >::value,
      bool
    >::type = false
  >
  optional& operator=(U&& u) noexcept(and_<std::is_nothrow_constructible<T, U>,
                                      std::is_nothrow_assignable<T&, U>>::value)
  {
    this->assign(std::forward<U>(u));
    return *this;
  }

  template<class U,
    typename std::enable_if<
      and_<
        std::is_constructible<T, const U&>,
        std::is_assignable<T&, const U&>
      >::value,
      bool
    >::type = false
  >
  optional& operator=(const optional<U>& u) noexcept(
    and_<std::is_nothrow_constructible<T, const U&>,
         std::is_nothrow_assignable<T&, const U&>>::value
  )
  {
    this->assign(u.data());
    return *this;
  }

  template<class U,
    typename std::enable_if<
      and_<
        std::is_constructible<T, U&&>,
        std::is_assignable<T&, U&&>
      >::value,
      bool
    >::type = false
  >
  optional& operator=(optional<U>&& u) noexcept(
    and_<std::is_nothrow_constructible<T, U&&>,
         std::is_nothrow_assignable<T&, U&&>>::value
  )
  {
    this->assign(std::move(u.data()));
    return *this;
  }

  template<class... U,
    typename std::enable_if<
      std::is_constructible<T, U...>::value,
      bool
    >::type = false
  >
  optional& emplace(U&&... u) noexcept(std::is_nothrow_constructible<T, U...>::value)
  {
    this->destroy();
    this->construct(std::forward<U>(u)...);
    return *this;
  }

  auto operator*() noexcept -> T&
  {
    assert(this->is_initialized());
    return this->data();
  }

  auto operator*() const noexcept -> const T&
  {
    assert(this->is_initialized());
    return this->data();
  }

  constexpr explicit operator bool() const noexcept
  {
    return this->is_initialized();
  }

    // if a move-only type is contained inside of an optional, the optional becomes
    // move-only.  However, it is still possible to dereference and get the contained
    // movable type...then move it out from under the optional without it being
    // "notified".  the extract method lets you perform a dereference-and-move while
    // setting the optional to none.  How to handle this situation should be reviewed.
    inline T extract() noexcept {
        assert(this->is_initialized());
        T result (std::move(this->data()));
        this->destroy();
        return result;
    }

  void swap(optional& rhs) noexcept(
    and_<is_nothrow_swappable<T>,
         std::is_nothrow_move_assignable<T>>::value
  )
  {
    using std::swap;
    if (this->is_initialized()) {
      if (rhs.is_initialized()) {
        swap(this->data(), rhs.data());
      } else {
        rhs.assign(std::move(this->data()));
        this->destroy();
      }
    } else {
      if (rhs.is_initialized()) {
        this->assign(std::move(rhs.data()));
        rhs.destroy();
      }
    }
  }

};

template<class T>
inline void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template<class T1, class T2>
inline bool operator==(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(*x == *y))
{
  if (x) {
    return y && (*x == *y);
  } else {
    return !y;
  }
}

template<class T1, class T2>
inline bool operator!=(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(x == y))
{
  return !(x == y);
}

template<class T1, class T2>
inline bool operator<(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(*x < *y))
{
  if (y) {
    return !x || (*x < *y);
  } else {
    return false;
  }
}

template<class T1, class T2>
inline bool operator<=(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(y < x))
{
  return !(y < x);
}

template<class T1, class T2>
inline bool operator>(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(y < x))
{
  return (y < x);
}

template<class T1, class T2>
inline bool operator>=(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(x < y))
{
  return !(x < y);
}

template<class T>
inline bool operator==(const optional<T>& x, none_t) noexcept
{
  return !x;
}

template<class T>
inline bool operator==(none_t, const optional<T>& x) noexcept
{
  return !x;
}

template<class T>
inline bool operator!=(const optional<T>& x, none_t) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
inline bool operator!=(none_t, const optional<T>& x) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
inline bool operator<(const optional<T>& x, none_t) noexcept
{
  return false;
}

template<class T>
inline bool operator<(none_t, const optional<T>& x) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
inline bool operator<=(const optional<T>& x, none_t) noexcept
{
  return !x;
}

template<class T>
inline bool operator<=(none_t, const optional<T>& x) noexcept
{
  return true;
}

template<class T>
inline bool operator>(const optional<T>& x, none_t) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
inline bool operator>(none_t, const optional<T>& x) noexcept
{
  return false;
}

template<class T>
inline bool operator>=(const optional<T>& x, none_t) noexcept
{
  return true;
}

template<class T>
inline bool operator>=(none_t, const optional<T>& x) noexcept
{
  return !x;
}

} // ns xstd

#endif // OPTIONAL_HPP_INCLUDED
