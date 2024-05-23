#include <iostream>
#include "tuple.h"
#include <vector>

#include <tuple>

//  template <typename T>
//  void explicit_test_helper(Tuple<int, T> t = {11, 22}) {}
//
//  template <typename T>
//  constexpr bool explicit_test() {
//    return requires(T) {
//      explicit_test_helper<T>();
//    };
//  }
//
//  static_assert(!explicit_test<std::vector<int>>());

struct NeitherDefaultNorCopyConstructible {
  double d;
  NeitherDefaultNorCopyConstructible() = delete;
  NeitherDefaultNorCopyConstructible(double d): d(d) {}
  NeitherDefaultNorCopyConstructible(const NeitherDefaultNorCopyConstructible&) = delete;
  NeitherDefaultNorCopyConstructible(NeitherDefaultNorCopyConstructible&&) = delete;
  NeitherDefaultNorCopyConstructible& operator =(NeitherDefaultNorCopyConstructible&&) = delete;
  NeitherDefaultNorCopyConstructible& operator =(const NeitherDefaultNorCopyConstructible&) = delete;
};

static_assert(!std::is_copy_constructible_v<Tuple<int&, NeitherDefaultNorCopyConstructible>>);
// static_assert(std::is_move_constructible_v<NeitherDefaultNorCopyConstructible>);

// static_assert(std::is_move_constructible_v<Tuple<int&, NeitherDefaultNorCopyConstructible>>);

int main() {
  std::cout << "Hello, World!" << std::endl;

  Tuple<int, char> t1(1, 'a');

  int x = 2;
  char y = 'b';

  Tuple<int, char> t2(x, y);

  Tuple<int, char> t3(true, 1);

  Tuple<int, char> t3e = {true, 1};

  // Tuple<int, std::vector<int>> t3t = {11, 22}; // should not compile

  std::pair<int, char> p{1, 'a'};
  Tuple<int, char> t5(p);

  Tuple<int, char> t6(t1);

  Tuple<int, char> t7(std::make_pair(3, 'c'));

  Tuple<int, char> t8(Tuple<int, char>(4, 'd'));

  t8 = t7;

  t7 = std::move(t6);

  Tuple<bool, bool> t10b;

  Tuple<int, char> t10;

  Tuple<long long, int> t9(5, 60);
  t10 = t9;

  std::cout << std::is_assignable_v<int&, const long long&> << ' ' << std::is_assignable_v<char&, const int&> << '\n'; // 1 1

  Tuple<long long, int> t4(t1);

  Tuple<int, char> t11;
  t11 = p;

  t11 = std::move(p);

  Tuple<int, char> t12;
  t12 = makeTuple(6, 'f');

  int a = 7;
  char b = 'g';
  //  Tuple<int, char> t13 = tie(a, b);

  int c = 7;
  char d = 'g';
  // Tuple<int, char> t14 = forwardAsTuple(std::move(c), std::move(d));

  std::cout << std::is_copy_constructible_v<char&&> << '\n';

  // Tuple<char&&> t14_('1');  // really need this?
  // Tuple<char&&> t14__(std::move('1'));

  Tuple<int, char, long long, bool> g1 = {1, 'a', 10, false};
  // Tuple<int, int> g1(1, 2);
  std::cout << get<0>(g1) << '\n';
  std::cout << get<1>(g1) << '\n';
  std::cout << get<2>(g1) << '\n';
  std::cout << get<3>(g1) << '\n';
  get<0>(g1) = 7;
  std::cout << get<0>(g1) << '\n';

  Tuple<int, int> t(1, 5);
  // std::cout << t.head_; так нельзя
  std::cout << get<0>(t) << "\n";
  get<1>(t) = 2;
  std::cout << get<1>(t) << "\n";

  Tuple<int, char, long long, bool> g2 = {1, 'a', 10, false};
  std::cout << get<char>(g2) << '\n';
  get<long long>(g2) = 7;
  std::cout << get<long long>(g2) << '\n';
  // std::cout << get<double>(g2) << '\n'; // CE

  Tuple<bool, double> c1(false, 1.5);
  Tuple<int, char> c2(7, 'e');
  Tuple<bool, double, int, char> c1_2 = tupleCat(c1, c2);
  std::cout << get<0>(c1_2) << ' ';
  std::cout << get<1>(c1_2) << ' ';
  std::cout << get<2>(c1_2) << ' ';
  std::cout << get<3>(c1_2) << '\n';

  Tuple<bool, double, int, char> c3 = tupleCat(Tuple<bool, double>(false, 1.7), Tuple<int, char>(7, 'e'));
  std::cout << get<0>(c3) << ' ';
  std::cout << get<1>(c3) << ' ';
  std::cout << get<2>(c3) << ' ';
  std::cout << get<3>(c3) << '\n';


  Tuple<bool, double, int, char> c4 = tupleCat(Tuple<bool, double>(false, 1.7), Tuple<int, char>(6, 'e'));
  Tuple<bool, double, int, char> c5 = tupleCat(Tuple<bool, double>(false, 1.7), Tuple<int, char>(7, 'e'));

  std::cout << (c1_2 < c3) << ' ' << (c1_2 > c3) << '\n';
  std::cout << (c1_2 < c4) << ' ' << (c1_2 > c4) << '\n';
  std::cout << (c4 <= c3) << ' ' << (c4 >= c3) << '\n';
  std::cout << (c5 < c3) << ' ' << (c5 > c3) << '\n';
  std::cout << (c5 == c3) << ' ' << (c5 != c3) << '\n';

  Tuple<int, char> v1 = {1, 'a'};
  Tuple<long long, int> v2(v1);
  std::cout << std::is_constructible_v<long long , decltype(get<0>(v1))> << '\n';
  std::cout << std::is_constructible_v<char, decltype(get<1>(v1))> << '\n';

  Tuple<char, int> v3 = {'a', 1};
  // Tuple<std::vector<int>, std::string> v4(v1); // CE

  int x_ = 2;
  int y_ = 5;
  Tuple<int&, int> q1(x_, y_);
  Tuple<int, int> q2 = q1;
  static_assert(std::is_constructible_v<const int&, int>);

  std::string s = "abc";
  int z = 8;
  Tuple<std::string&, int&> q3(s, z);
  // Tuple<std::string&, int&&> q4(q3);

  static_assert(!std::is_constructible_v<Tuple<std::string&, int&&>, Tuple<std::string&, int&>>);



  return 0;
}
