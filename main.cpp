#include "optional.hpp"

static_assert(xstd::is_trivially_destructible<int>::value, "");
static_assert(xstd::is_trivially_destructible<xstd::optional<int>>::value, "");

static_assert(std::is_copy_constructible<xstd::optional<int>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<int>>::value, "Error");
static_assert(std::is_copy_assignable<xstd::optional<int>>::value, "Error");
static_assert(std::is_move_assignable<xstd::optional<int>>::value, "Error");

static_assert(std::is_nothrow_copy_constructible<xstd::optional<int>>::value, "Error");
static_assert(std::is_nothrow_move_constructible<xstd::optional<int>>::value, "Error");
static_assert(std::is_nothrow_copy_assignable<xstd::optional<int>>::value, "Error");
static_assert(std::is_nothrow_move_assignable<xstd::optional<int>>::value, "Error");

struct cm
{
  cm(cm&&) = default;
  cm(const cm&) = default;
};

static_assert(std::is_copy_constructible<xstd::optional<cm>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<cm>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<cm>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<cm>>::value, "Error");

struct ncnm
{
  ncnm(ncnm&&) = delete;
  ncnm(const ncnm&) = delete;
};

static_assert(!std::is_copy_constructible<xstd::optional<ncnm>>::value, "Error");
static_assert(!std::is_move_constructible<xstd::optional<ncnm>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<ncnm>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<ncnm>>::value, "Error");

struct ncc
{
  ncc(ncc&&) = delete;
  ncc(const ncc&) = default;
};

static_assert(std::is_copy_constructible<xstd::optional<ncc>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<ncc>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<ncc>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<ncc>>::value, "Error");

struct nmc
{
  nmc(nmc&&) = default;
  nmc(const nmc&) = delete;
};

static_assert(!std::is_copy_constructible<xstd::optional<nmc>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<nmc>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<nmc>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<nmc>>::value, "Error");

struct t1 {
  t1(t1&&) = default;
  t1(const t1&) = default;
  t1& operator=(t1&&) = default;
  t1& operator=(const t1&) = default;
};

static_assert(std::is_copy_constructible<xstd::optional<t1>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<t1>>::value, "Error");
static_assert(std::is_copy_assignable<xstd::optional<t1>>::value, "Error");
static_assert(std::is_move_assignable<xstd::optional<t1>>::value, "Error");

static_assert(std::is_nothrow_copy_constructible<xstd::optional<t1>>::value, "Error");
static_assert(std::is_nothrow_move_constructible<xstd::optional<t1>>::value, "Error");
static_assert(std::is_nothrow_copy_assignable<xstd::optional<t1>>::value, "Error");
static_assert(std::is_nothrow_move_assignable<xstd::optional<t1>>::value, "Error");

struct t2 {
  t2(t2&&) = delete;
  t2(const t2&) = delete;
  t2& operator=(t2&&) = default;
  t2& operator=(const t2&) = default;
};

static_assert(!std::is_copy_constructible<xstd::optional<t2>>::value, "Error");
static_assert(!std::is_move_constructible<xstd::optional<t2>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<t2>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<t2>>::value, "Error");

struct t3 {
  t3(t3&&) = default;
  t3(const t3&) = delete;
  t3& operator=(t3&&) = default;
  t3& operator=(const t3&) = default;
};

static_assert(!std::is_copy_constructible<xstd::optional<t3>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<t3>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<t3>>::value, "Error");
static_assert(std::is_move_assignable<xstd::optional<t3>>::value, "Error");

struct t4 {
  t4(t4&&) = delete;
  t4(const t4&) = default;
  t4& operator=(t4&&) = default;
  t4& operator=(const t4&) = default;
};

static_assert(std::is_copy_constructible<t4>::value, "Error");
static_assert(!std::is_move_constructible<t4>::value, "Error");
static_assert(std::is_copy_assignable<t4>::value, "Error");
static_assert(std::is_move_assignable<t4>::value, "Error");

static_assert(std::is_copy_constructible<xstd::optional<t4>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<t4>>::value, "Error");
static_assert(std::is_copy_assignable<xstd::optional<t4>>::value, "Error");
static_assert(std::is_move_assignable<xstd::optional<t4>>::value, "Error");

struct t5 {
  t5(t5&&) = default;
  t5& operator=(t5&&) = default;
};

static_assert(!std::is_copy_constructible<xstd::optional<t5>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<t5>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<t5>>::value, "Error");
static_assert(std::is_move_assignable<xstd::optional<t5>>::value, "Error");

struct t6 {
  t6(const t6&) = default;
  t6& operator=(const t6&) = default;
};

static_assert(std::is_copy_constructible<xstd::optional<t6>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<t6>>::value, "Error");
static_assert(std::is_copy_assignable<xstd::optional<t6>>::value, "Error");
static_assert(std::is_move_assignable<xstd::optional<t6>>::value, "Error");

struct t7 {
  t7(t7&&) = delete;
  t7(const t7&) = delete;
  t7& operator=(t7&&) = delete;
  t7& operator=(const t7&) = delete;
};

static_assert(!std::is_copy_constructible<xstd::optional<t7>>::value, "Error");
static_assert(!std::is_move_constructible<xstd::optional<t7>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<t7>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<t7>>::value, "Error");

struct t8 {
  t8(t8&&) = default;
  t8(const t8&) = default;
  t8& operator=(t8&&) = delete;
  t8& operator=(const t8&) = delete;
};

static_assert(std::is_copy_constructible<xstd::optional<t8>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<t8>>::value, "Error");
static_assert(!std::is_copy_assignable<xstd::optional<t8>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<t8>>::value, "Error");

class Foo {
  int value;
public:
  Foo(int newValue) : value(newValue) { }
};

xstd::optional<Foo const> foo(Foo(1020));
xstd::optional<Foo const> foo2 = Foo(1020);

static_assert(std::is_copy_constructible<xstd::optional<Foo const>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<Foo const>>::value, "Error");

static_assert(!std::is_copy_assignable<xstd::optional<Foo const>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<Foo const>>::value, "Error");

static_assert(!std::is_assignable<xstd::optional<Foo const>&, int>::value, "Error");

static_assert(std::is_constructible<xstd::optional<Foo const>, Foo>::value, "Error");
static_assert(std::is_constructible<xstd::optional<Foo const>, int>::value, "Error");

static_assert(std::is_convertible<Foo, xstd::optional<Foo const>>::value, "Error");
static_assert(std::is_convertible<int, xstd::optional<Foo const>>::value, "Error");

static_assert(xstd::is_trivially_destructible<Foo const>::value, "Error");
static_assert(xstd::is_trivially_copy_constructible<Foo const>::value, "Error");
static_assert(xstd::is_trivially_copy_assignable<Foo>::value, "Error");
static_assert(xstd::is_trivially_destructible<xstd::optional<Foo const>>::value, "Error");
static_assert(xstd::is_trivially_copy_constructible<xstd::optional<Foo const>>::value, "Error");
static_assert(xstd::is_trivially_copy_assignable<xstd::optional<Foo>>::value, "Error");

class Bar {
  int value;
public:
  Bar(int newValue) : value(newValue) { }
  ~Bar(){}
};

xstd::optional<Bar const> bar(Bar(1020));
xstd::optional<Bar const> bar2 = Bar(1020);

static_assert(std::is_copy_constructible<xstd::optional<Bar const>>::value, "Error");
static_assert(std::is_move_constructible<xstd::optional<Bar const>>::value, "Error");

static_assert(!std::is_copy_assignable<xstd::optional<Bar const>>::value, "Error");
static_assert(!std::is_move_assignable<xstd::optional<Bar const>>::value, "Error");

static_assert(!std::is_assignable<xstd::optional<Bar const>&, int>::value, "Error");

static_assert(std::is_constructible<xstd::optional<Bar const>, Bar>::value, "Error");
static_assert(std::is_constructible<xstd::optional<Bar const>, int>::value, "Error");

static_assert(std::is_convertible<Bar, xstd::optional<Bar const>>::value, "Error");
static_assert(std::is_convertible<int, xstd::optional<Bar const>>::value, "Error");

static_assert(!std::is_trivially_destructible<xstd::optional<Bar const>>::value, "Error");

struct NonTrivTrivDTor {
  int membar;
  NonTrivTrivDTor() : membar() {}
  NonTrivTrivDTor(const NonTrivTrivDTor& rhs) : membar(rhs.membar) {}
  NonTrivTrivDTor(int v) : membar(v) {}
};

xstd::optional<NonTrivTrivDTor> nttd1;
xstd::optional<NonTrivTrivDTor> nttd2(nttd1);

#include <string>
#include <iostream>

struct IntroSpec {
  IntroSpec() {
    std::cout << "def-c'tor called" << std::endl;
  }

  IntroSpec(const IntroSpec&) {
    std::cout << "copy-c'tor called" << std::endl;
  }

  IntroSpec(IntroSpec&&) {
    std::cout << "move-c'tor called" << std::endl;
  }

  IntroSpec& operator=(const IntroSpec&) {
    std::cout << "copy-assign-op called" << std::endl;
    return *this;
  }

  IntroSpec& operator=(IntroSpec&&) {
    std::cout << "move-assign-op called" << std::endl;
    return *this;
  }

  ~IntroSpec() {
    std::cout << "d'tor called" << std::endl;
  }
};

struct ICv1 {
  operator bool() const { return true; }
};

struct ECv1 {
  explicit operator bool() const { return true; }
};

struct ICv2 {
  ICv2(bool){}
};

struct ECv2 {
  explicit ECv2(bool){}
};

struct IOptCv1 {
  IOptCv1(xstd::optional<int>){}
};

void test_conv_1() {
  xstd::optional<ICv1> oi1;
  xstd::optional<bool> ob1(oi1); // OK
  xstd::optional<bool> ob2 = oi1; // OK
  (void) ob2;

  xstd::optional<ECv1> oe1;
  xstd::optional<bool> ob3(oe1); // OK
  static_assert(std::is_constructible<xstd::optional<bool>, const xstd::optional<ECv1>&>::value, "Error");
  //xstd::optional<bool> ob4 = oe1; // Error
  static_assert(!std::is_convertible<const xstd::optional<ECv1>&, xstd::optional<bool>>::value, "Error");

  xstd::optional<ICv2> oi2(ob1); // OK
  xstd::optional<ICv2> oi3 = ob1; // OK
  (void) oi3;

  xstd::optional<ECv2> oe2(ob1); // OK
  static_assert(std::is_constructible<xstd::optional<ECv2>, const xstd::optional<bool>&>::value, "Error");
  //xstd::optional<ECv2> oe3 = ob1; // Error
  static_assert(!std::is_convertible<const xstd::optional<bool>&, xstd::optional<ECv2>>::value, "Error");

  xstd::optional<int> oint;
  xstd::optional<IOptCv1> oopti1(oint); // OK
  xstd::optional<IOptCv1> oopti2 = oint; // OK
  (void) oopti2;
}

void test_conv_2() {
  ICv1 icv1;
  xstd::optional<bool> ob1(icv1); // OK
  xstd::optional<bool> ob2 = icv1; // OK
  (void) ob2;

  ECv1 ecv1;
  xstd::optional<bool> ob3(ecv1); // OK
  static_assert(std::is_constructible<xstd::optional<bool>, const ECv1&>::value, "Error");
  //xstd::optional<bool> ob4 = ecv1; // Error
  static_assert(!std::is_convertible<const ECv1&, xstd::optional<bool>>::value, "Error");

  bool b{};
  xstd::optional<ICv2> oi2(b); // OK
  xstd::optional<ICv2> oi3 = b; // OK
  (void) oi3;

  xstd::optional<ECv2> oe2(b); // OK
  static_assert(std::is_constructible<xstd::optional<ECv2>, const bool&>::value, "Error");
  //xstd::optional<ECv2> oe3 = b; // Error
  static_assert(!std::is_convertible<const bool&, xstd::optional<ECv2>>::value, "Error");

  xstd::optional<int> oint;
  xstd::optional<IOptCv1> oopti1(oint); // OK
  xstd::optional<IOptCv1> oopti2 = oint; // OK
  (void) oopti2;
}

constexpr xstd::optional<int> const_oi{};
constexpr xstd::optional<int> const_oi_2{xstd::none};

static_assert(!const_oi && !const_oi_2, "");

#include <cassert>

int main()
{
  xstd::optional<cm> ocm1;
  xstd::optional<cm> ocm2 = ocm1;
  xstd::optional<cm> ocm3 = std::move(ocm1);
  (void) ocm2;
  (void) ocm3;

  xstd::optional<ncnm> oncm1;
  //xstd::optional<ncnm> oncm2 = oncm1;
  //xstd::optional<ncnm> oncm3 = std::move(oncm1);

  xstd::optional<ncc> oncc1;
  xstd::optional<ncc> oncc2 = oncc1;
  xstd::optional<ncc> oncc3 = std::move(oncc1);
  (void) oncc2;
  (void) oncc3;

  xstd::optional<nmc> onmc1;
  //xstd::optional<nmc> onmc2 = onmc1;
  xstd::optional<nmc> onmc3 = std::move(onmc1);
  (void) onmc3;

  xstd::optional<std::string> os1;
  xstd::optional<std::string> os2 = os1;
  xstd::optional<std::string> os3 = std::move(os1);

  xstd::optional<std::string> os12("This is a long sentence just to be sure that we get allocations");
  std::cout << "Original     : " << *os12 << std::endl;
  xstd::optional<std::string> os22 = os12;
  std::cout << "Copied       : " << *os22 << std::endl;
  xstd::optional<std::string> os32 = std::move(os12);
  std::cout << "Moved        : " << *os32 << std::endl;
  os12 = os22;
  std::cout << "Copy assigned: " << *os12 << std::endl;
  os12 = std::move(os32);
  std::cout << "Move assigned: " << *os12 << std::endl;

  std::cout << std::endl;

  {
      xstd::optional<IntroSpec> o1;
      xstd::optional<IntroSpec> o2(IntroSpec{});

      std::cout << std::endl;

      xstd::optional<IntroSpec> o3 = o2;
      xstd::optional<IntroSpec> o4 = std::move(o2);

      std::cout << std::endl;

      o3 = o4;
      o4 = std::move(o3);

      std::cout << std::endl;
  }

  {
      xstd::optional<std::nullptr_t> on;
      assert(!on);
      assert(on == xstd::none);
      on = xstd::none;
      assert(!on);
      assert(on == xstd::none);
      auto n = xstd::none;
      on = n;
      xstd::optional<std::nullptr_t> on2(xstd::none);
      assert(!on2);
      assert(on2 == xstd::none);
      assert(on2 == on);
      xstd::optional<std::nullptr_t> on3(n);
      assert(!on3);
      assert(on3 == xstd::none);
      on3 = n;
      assert(!on3);
      const xstd::none_t cn{};
      assert(on3 == xstd::none);
      xstd::optional<std::nullptr_t> on4(cn);
      assert(!on4);
      assert(on4 == xstd::none);
      on4 = cn;
      assert(!on4);
      assert(on4 == xstd::none);
  }
}


