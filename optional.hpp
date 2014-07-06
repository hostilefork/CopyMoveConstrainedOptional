#ifndef OPTIONAL_HPP_INCLUDED
#define OPTIONAL_HPP_INCLUDED

// Copyright 2011-2014 Daniel Kr&uuml;gler.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <type_traits>
#include <utility>
#include <new>
#include <stdexcept>

#include "tools.hpp"
#include "is_swappable.hpp"

namespace xstd {

// workaround: std utility functions aren't constexpr yet
// (taken from std::experimental::optional)
template <class T> 
constexpr T&& forward(typename std::remove_reference<T>::type& t) noexcept
{
  return static_cast<T&&>(t);
}

template <class T> 
constexpr T&& forward(typename std::remove_reference<T>::type&& t) noexcept
{
  static_assert(!std::is_lvalue_reference<T>::value, "This overload doesn't accept lvalue references");
  return static_cast<T&&>(t);
}

template <class T> 
constexpr typename std::remove_reference<T>::type&& move(T&& t) noexcept
{
  return static_cast<typename std::remove_reference<T>::type&&>(t);
}

template<class>
class optional;

struct inplace_t {};

constexpr inplace_t inplace{};

struct nullopt_t {};

constexpr nullopt_t nullopt{};

class bad_optional_access : public std::logic_error 
{
public: 
  bad_optional_access() : std::logic_error("invalid access to empty optional") {}
};

namespace details {

// Enum used as bitmask to flag copy/move capabilities of a type:
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

// All instantiations of ctor_base are empty, trivial types that have
// specific move/copy constructors deleted
template<member_policy>
struct ctor_base;

template<>
struct ctor_base<member_policy::none>
{
  constexpr ctor_base() noexcept = default;
  constexpr ctor_base(const ctor_base&) = delete;
  constexpr ctor_base(ctor_base&&) = delete;
  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

template<>
struct ctor_base<member_policy::move | member_policy::copy>
{
  constexpr ctor_base() noexcept = default;
  constexpr ctor_base(const ctor_base& rhs) noexcept = default;
  constexpr ctor_base(ctor_base&& rhs) noexcept = default;
  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

template<>
struct ctor_base<member_policy::copy>
{
  constexpr ctor_base() noexcept = default;
  constexpr ctor_base(const ctor_base& rhs) noexcept = default;
  constexpr ctor_base(ctor_base&&) = delete;
  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

template<>
struct ctor_base<member_policy::move>
{
  constexpr ctor_base() noexcept = default;
  constexpr ctor_base(const ctor_base&) = delete;
  constexpr ctor_base(ctor_base&& rhs) noexcept = default;
  ctor_base& operator=(const ctor_base&) noexcept = default;
  ctor_base& operator=(ctor_base&&) noexcept = default;
};

// All instantiations of assign_base are empty, trivial types that have
// specific move/copy assignment operators deleted
template<member_policy>
struct assign_base;

template<>
struct assign_base<member_policy::none>
{
  constexpr assign_base() noexcept = default;
  constexpr assign_base(const assign_base&) noexcept = default;
  constexpr assign_base(assign_base&&) noexcept = default;
  assign_base& operator=(const assign_base&) = delete;
  assign_base& operator=(assign_base&&) = delete;
};

template<>
struct assign_base<member_policy::move | member_policy::copy>
{
  constexpr assign_base() noexcept = default;
  constexpr assign_base(const assign_base&) noexcept = default;
  constexpr assign_base(assign_base&&) noexcept = default;
  assign_base& operator=(const assign_base& rhs) noexcept = default;
  assign_base& operator=(assign_base&& rhs) noexcept = default;
};

template<>
struct assign_base<member_policy::copy>
{
  constexpr assign_base() noexcept = default;
  constexpr assign_base(const assign_base&) noexcept = default;
  constexpr assign_base(assign_base&&) noexcept = default;
  assign_base& operator=(const assign_base& rhs) noexcept = default;
  assign_base& operator=(assign_base&&) = delete;
};

template<>
struct assign_base<member_policy::move>
{
  constexpr assign_base() noexcept = default;
  constexpr assign_base(const assign_base&) noexcept = default;
  constexpr assign_base(assign_base&&) noexcept = default;
  assign_base& operator=(const assign_base&) = delete;
  assign_base& operator=(assign_base&& rhs) noexcept = default;
};

template<class T>
using select_ctor_base = ctor_base<
  (std::is_copy_constructible<T>::value ? member_policy::copy : member_policy::none) |
  (std::is_move_constructible<T>::value ? member_policy::move : member_policy::none)>;

template<class T>
using select_assign_base = assign_base<
  (and_<std::is_copy_constructible<T>, std::is_copy_assignable<T>>::value ? member_policy::copy : member_policy::none) |
  (and_<std::is_move_constructible<T>, std::is_move_assignable<T>>::value ? member_policy::move : member_policy::none)>;


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

// Ideally we would use the official is_trivially_copyable trait here:
template<class T, bool = and_<
  is_trivially_copy_constructible<T>,
  is_trivially_copy_assignable<typename std::remove_cv<T>::type>,
  is_trivially_destructible<T>
 >::value
>
class optional_data;

// Partial specialization for types that are not trivial copyable
template<class T>
class optional_data<T, false>
{
  bool is_init_;
  union {
    unsigned char for_value_init_;
    typename std::remove_cv<T>::type value_;
  };

public:
  constexpr auto is_initialized() const noexcept -> bool
  {
    return this->is_init_;
  }

  auto raw_memory() noexcept -> void*
  {
    return &this->value_;
  }

  constexpr auto raw_memory() const noexcept -> const void*
  {
    return &this->value_;
  }

  auto data() noexcept -> T&
  {
    return this->value_;
  }

  constexpr auto data() const noexcept -> const T&
  {
    return this->value_;
  }

  template<class... U>
  void construct(U&&... u)
  {
    assert(!this->is_init_);
    ::new (this->raw_memory()) T(std::forward<U>(u)...);
    this->is_init_ = true;
  }

  template<class U>
  void assign(U&& u)
  {
    if (this->is_init_)
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
/*   static_assert(std::has_nothrow_destructor<T>::value,
                  "T's destructor shall not throw exceptions"); */
    if (this->is_init_)
    {
      this->data().~T();
      this->is_init_ = false;
    }
  }

  constexpr optional_data() noexcept : is_init_(false), for_value_init_() {}

  template<class... Args>
  constexpr optional_data(inplace_t, Args&&... args) : is_init_(true),
    value_(xstd::forward<Args>(args)...)
  {}

  optional_data(const optional_data& rhs) noexcept(std::is_nothrow_copy_constructible<T>::value)
  : is_init_(false)
  {
    if (rhs.is_initialized())
      this->construct(rhs.data());
  }

  optional_data(optional_data&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
  : is_init_(false)
  {
    if (rhs.is_initialized())
      this->construct(std::move(rhs.data()));
  }

  optional_data& operator=(const optional_data& rhs) noexcept(
    and_<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_assignable<T>>::value
  )
  {
    if (rhs.is_initialized())
      this->assign(rhs.data());
    else
      this->destroy();
    return *this;
  }

  optional_data& operator=(optional_data&& rhs) noexcept(
    and_<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_assignable<T>>::value
  )
  {
    if (rhs.is_initialized())
      this->assign(std::move(rhs.data()));
    else
      this->destroy();
    return *this;
  }

  // NB: Actually the explicit exception-specification is not
  // required, but the current gcc snap-shot behaves incorrect
  // in the derived class, if not doing so.
  ~optional_data() noexcept
  {
    this->destroy();
  }
};

// Partial specialization ideally for trivial copyable types. It would
// also be a trivial copyable type, once we can detect trivial special
// move members:
template<class T>
class optional_data<T, true>
{
  bool is_init_;
  // For the moment we cannot use an anonymous union because of
  // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=54922
  // (Just recently fixed)
  union data_t {
    unsigned char for_value_init_;
    typename std::remove_cv<T>::type value_;
    constexpr data_t() noexcept : for_value_init_() {}
    template<class... Args>
    constexpr data_t(inplace_t, Args&&... args)
    : value_(xstd::forward<Args>(args)...) {}
  } data_;

public:
  constexpr auto is_initialized() const noexcept -> bool
  {
    return this->is_init_;
  }

  auto raw_memory() noexcept -> void*
  {
    return &this->data_.value_;
  }

  constexpr auto raw_memory() const noexcept -> const void*
  {
    return &this->data_.value_;
  }

  auto data() noexcept -> T&
  {
    return this->data_.value_;
  }

  constexpr auto data() const noexcept -> const T&
  {
    return this->data_.value_;
  }

  template<class... U>
  void construct(U&&... u)
  {
    assert(!this->is_init_);
    ::new (this->raw_memory()) T(std::forward<U>(u)...);
    this->is_init_ = true;
  }

  template<class U>
  void assign(U&& u)
  {
    if (this->is_init_)
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

  constexpr optional_data() noexcept : is_init_(false) {}

  template<class... Args>
  constexpr optional_data(inplace_t, Args&&... args) : is_init_(true),
    data_(inplace_t(), xstd::forward<Args>(args)...)
  {}

  optional_data(const optional_data& rhs) = default;

  // Once we can detect trivial move constructors, we could default
  // this member
  optional_data(optional_data&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
  : is_init_(false)
  {
    if (rhs.is_initialized())
      this->construct(std::move(rhs.data()));
  }

  optional_data& operator=(const optional_data& rhs) = default;

  // Once we can detect trivial move assignment operators, we could default
  // this member
  optional_data& operator=(optional_data&& rhs) noexcept(
    and_<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_assignable<T>>::value
  )
  {
    if (rhs.is_initialized())
      this->assign(std::move(rhs.data()));
    else
      this->destroy();
    return *this;
  }

  ~optional_data() noexcept = default;
};

}

template<class T>
class optional : private details::select_ctor_base<T>,
                 private details::select_assign_base<T>,
                 private details::optional_data<T>
{
  using data_type = details::optional_data<T>;
  
  using vt_wo_cv = typename std::remove_cv<T>::type;

  template<class>
  friend class optional;

public:
  static_assert(std::is_object<T>::value, "T shall be an object type");
  
  static_assert(and_<not_<std::is_same<vt_wo_cv, nullopt_t>>, 
                     not_<std::is_same<vt_wo_cv, inplace_t>>>::value, 
                "Invalid T");

  using value_type = T;

  constexpr optional() noexcept = default;

  constexpr optional(nullopt_t) noexcept {}

  template<class U,
    typename std::enable_if<
      and_<
        not_<details::is_equivalent<U, optional>>,
        std::is_convertible<U, T>,
        std::is_constructible<T, U>
      >::value,
      bool
    >::type = false
  >
  constexpr optional(U&& u) noexcept(std::is_nothrow_constructible<T, U>::value)
  : data_type(inplace_t(), xstd::forward<U>(u))
  {
  }

  template<class U,
    typename std::enable_if<
      and_<
        not_<details::is_equivalent<U, optional>>,
        not_<std::is_convertible<U, T>>,
        std::is_constructible<T, U>
      >::value,
      bool
    >::type = false
  >
  explicit constexpr optional(U&& u) noexcept(std::is_nothrow_constructible<T, U>::value)
  : data_type(inplace_t(), xstd::forward<U>(u))
  {
  }

  template<class... U,
    typename std::enable_if<
      std::is_constructible<T, U...>::value,
      bool
    >::type = false
  >
  constexpr optional(inplace_t, U&&... u) noexcept(std::is_nothrow_constructible<T, U...>::value)
  : data_type(inplace_t(), xstd::forward<U>(u)...)
  {
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
    if (u.is_initialized())
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
    if (u.is_initialized())
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
    if (u.is_initialized())
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
    if (u.is_initialized())
      this->construct(std::move(u.data()));
  }

  optional& operator=(nullopt_t) noexcept
  {
    this->destroy();
    return *this;
  }

  template<class U,
    typename std::enable_if<
      and_<
       not_<details::is_equivalent<U, optional>>,
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
    if (u.is_initialized())
      this->assign(u.data());
    else
      this->destroy();
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
    if (u.is_initialized())
      this->assign(std::move(u.data()));
    else
      this->destroy();
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

  constexpr auto operator*() const noexcept -> const T&
  {
    return (assert(this->is_initialized()), this->data());
  }

  auto value() -> T&
  {
    return this->is_initialized() ? this->data() : throw bad_optional_access();
  }

  constexpr auto value() const -> const T&
  {
    return this->is_initialized() ? this->data() : throw bad_optional_access();
  }

  constexpr explicit operator bool() const noexcept
  {
    return this->is_initialized();
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

  /**
   * Move-constructs the T contents of *this into a temporary of type (cv-unqualified) T,
   * clears its own state, and returns the temporary value.
   * The operation has the same exception-safety as T's move-construction.
   * @pre *this == true
   * @post !*this
  */
  typename std::remove_cv<T>::type
  extract() noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    assert(this->is_initialized());
    typename std::remove_cv<T>::type result(std::move(this->data()));
    this->destroy();
    return result;
  }

  /**
   * @param[out] dst Lvalue as destination for a possible move-assignment
   * @return If !*this returns false, else move-assigns the T content of *this
   *         into dst, clears its own content state and returns true.
   * @post !*this
   * The operation has the same exception-safety as T's move-assignment to an
   * lvalue of U.
  */
  template<class U,
    typename std::enable_if<
      std::is_assignable<U&, T&&>::value,
      bool
    >::type = false
  >
  bool extract_to(U& dst) noexcept(std::is_nothrow_assignable<U&, T&&>::value)
  {
    if (!this->is_initialized())
      return false;
    dst = std::move(this->data());
    this->destroy();
    return true;
  }

};

template<class T>
inline void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
{
  x.swap(y);
}

template<class T1, class T2>
constexpr bool operator==(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(*x == *y))
{
  return x ? (y && (*x == *y)) : !y;
}

template<class T1, class T2>
constexpr bool operator!=(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(x == y))
{
  return !(x == y);
}

template<class T1, class T2>
constexpr bool operator<(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(*x < *y))
{
  return y ? !x || (*x < *y) : false;
}

template<class T1, class T2>
constexpr bool operator<=(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(y < x))
{
  return !(y < x);
}

template<class T1, class T2>
constexpr bool operator>(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(y < x))
{
  return (y < x);
}

template<class T1, class T2>
constexpr bool operator>=(const optional<T1>& x, const optional<T2>& y) noexcept(noexcept(x < y))
{
  return !(x < y);
}

template<class T>
constexpr bool operator==(const optional<T>& x, nullopt_t) noexcept
{
  return !x;
}

template<class T>
constexpr bool operator==(nullopt_t, const optional<T>& x) noexcept
{
  return !x;
}

template<class T>
constexpr bool operator!=(const optional<T>& x, nullopt_t) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
constexpr bool operator!=(nullopt_t, const optional<T>& x) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
constexpr bool operator<(const optional<T>& x, nullopt_t) noexcept
{
  return false;
}

template<class T>
constexpr bool operator<(nullopt_t, const optional<T>& x) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
constexpr bool operator<=(const optional<T>& x, nullopt_t) noexcept
{
  return !x;
}

template<class T>
constexpr bool operator<=(nullopt_t, const optional<T>& x) noexcept
{
  return true;
}

template<class T>
constexpr bool operator>(const optional<T>& x, nullopt_t) noexcept
{
  return static_cast<bool>(x);
}

template<class T>
constexpr bool operator>(nullopt_t, const optional<T>& x) noexcept
{
  return false;
}

template<class T>
constexpr bool operator>=(const optional<T>& x, nullopt_t) noexcept
{
  return true;
}

template<class T>
constexpr bool operator>=(nullopt_t, const optional<T>& x) noexcept
{
  return !x;
}

// make_optional from std::experimental::optional

template <class T>
constexpr optional<typename std::decay<T>::type> make_optional(T&& v)
{
  return optional<typename std::decay<T>::type>(xstd::forward<T>(v));
}

} // ns xstd

#endif // OPTIONAL_HPP_INCLUDED
