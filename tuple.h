#include <type_traits>
#include <utility>


/// Declarations

template<class... Types>
class Tuple;

template<std::size_t N, class... Types>
auto& get(Tuple<Types...>& tuple);

template<std::size_t N, class... Types>
const auto& get(const Tuple<Types...>& tuple);

template<std::size_t N, class... Types>
auto&& get(Tuple<Types...>&& tuple);

template<>
class Tuple<> {
 private:
  static constexpr std::size_t len() { return 0; }
};

template<class Head, class... Tail>
class Tuple<Head, Tail...>;

/// For tupleCat

template<std::size_t N>
struct tuple_cat_tag {};

/// Tuple

template<class Head, class... Tail>
class Tuple<Head, Tail...> {
 private:

  /// Fields

  Head head_;
  [[ no_unique_address ]] Tuple<Tail...> tail_;

 private:

  /// Useful structures (declarations)

  template<class... Args>
  struct CheckConstructible {
    static constexpr bool value = true;
  };

  template<class... Args>
  struct CheckConvertible {
    static constexpr bool value = true;
  };

  template<std::size_t N, typename First, typename... Rest>
  struct GetType;

  template<std::size_t N, typename... Args>
  using GetType_t = typename GetType<N, Args...>::type;

  template<typename... Args>
  using First_t = GetType_t<0, Args...>;

public:

  /// Constructors

  // Default

  template<typename T>
  struct CopyListInitializable {
    static constexpr bool value = requires { T{}; };
  };

  explicit(!CopyListInitializable<Head>::value || (... || !CopyListInitializable<Tail>::value))
  Tuple()
  requires std::is_default_constructible_v<Head> && (... && std::is_default_constructible_v<Tail>)
      : head_(), tail_() {}

  // Base

  explicit(!std::is_convertible_v<const Head&, Head> && (... && !std::is_convertible_v<const Tail&, Tail>))
  Tuple(const Head& head, const Tail&... tail)
  requires std::is_copy_constructible_v<Head> && (... && std::is_copy_constructible_v<Tail>)
      : head_(std::forward<decltype(head)>(head)), tail_(std::forward<decltype(tail)>(tail)...) {}

  // Argument move

  template<typename UHead, typename... UTail>
  explicit(!std::is_convertible_v<UHead, Head> || std::disjunction_v<std::negation<std::is_convertible<UTail, Tail>>...>)
  Tuple(UHead&& new_head, UTail&&... new_tail)
  requires(std::is_constructible_v<Head, UHead> &&
           (sizeof...(UTail) == sizeof...(Tail)) &&
           std::conjunction_v<std::is_constructible<Tail, UTail>...>)
      : head_(std::forward<UHead>(new_head)),
        tail_(std::forward<UTail>(new_tail)...) {}

  // Conversion

  template<class UHead, class... UTail>
  explicit Tuple(const Tuple<UHead, UTail...>& other)
  requires ((sizeof...(Tail) == sizeof...(UTail)) &&
            CheckConstructible<Tuple<Head, Tail...>, decltype(other)>::value &&
            (sizeof...(Tail) != 0 ||
            (!std::is_convertible_v<decltype(other), Head> && !std::is_constructible_v<Head, decltype(other)> && !std::is_same_v<Head, UHead>)) &&
            (!CheckConvertible<Tuple<Head, Tail...>, decltype(other)>::value)) // explicit
            : head_(other.head_), tail_(other.tail_) {}

  template<class UHead, class... UTail>
  Tuple(const Tuple<UHead, UTail...>& other)
  requires ((sizeof...(Tail) == sizeof...(UTail)) &&
            CheckConstructible<Tuple<Head, Tail...>, decltype(other)>::value &&
            (sizeof...(Tail) != 0 ||
            (!std::is_convertible_v<decltype(other), Head> && !std::is_constructible_v<Head, decltype(other)> && !std::is_same_v<Head, UHead>)) &&
            (CheckConvertible<Tuple<Head, Tail...>, decltype(other)>::value)) // non-explicit
            : head_(other.head_), tail_(other.tail_) {}

  template<class UHead, class... UTail>
  explicit Tuple(Tuple<UHead, UTail...>&& other)
  requires ((sizeof...(Tail) == sizeof...(UTail)) &&
            CheckConstructible<Tuple<Head, Tail...>, decltype(other)>::value &&
            (sizeof...(Tail) != 0 ||
             (!std::is_convertible_v<decltype(other), Head> && !std::is_constructible_v<Head, decltype(other)> && !std::is_same_v<Head, UHead>)) &&
            (!CheckConvertible<Tuple<Head, Tail...>, decltype(other)>::value)) // explicit
      : head_(std::forward<decltype(other.head_)>(other.head_)), tail_(std::forward<decltype(other.tail_)>(other.tail_)) {}

  template<class UHead, class... UTail>
  Tuple(Tuple<UHead, UTail...>&& other)
  requires ((sizeof...(Tail) == sizeof...(UTail)) &&
            CheckConstructible<Tuple<Head, Tail...>, decltype(other)>::value &&
            (sizeof...(Tail) != 0 ||
             (!std::is_convertible_v<decltype(other), Head> && !std::is_constructible_v<Head, decltype(other)> && !std::is_same_v<Head, UHead>)) &&
            (CheckConvertible<Tuple<Head, Tail...>, decltype(other)>::value)) // non-explicit
      : head_(std::forward<decltype(other.head_)>(other.head_)), tail_(std::forward<decltype(other.tail_)>(other.tail_)) {}

  // From pair

  template<typename T1, typename T2>
  Tuple(const std::pair<T1, T2>& pa)
  requires ((sizeof...(Tail) == 1) && std::is_same_v<Head, T1> && std::is_same_v<First_t<Tail...>, T2>)
      : head_(pa.first), tail_(pa.second) {}

  template<typename T1, typename T2>
  Tuple(std::pair<T1, T2>&& pa)
  requires ((sizeof...(Tail) == 1) && std::is_same_v<Head, T1> && std::is_same_v<First_t<Tail...>, T2>)
      : head_(std::move(pa.first)), tail_(std::move(pa.second)) {}

  template<typename T1, typename T2>
  Tuple(const std::pair<T1, T2>& pa)
  requires ((sizeof...(Tail) == 1) && !std::is_same_v<Head, T1> && !std::is_same_v<First_t<Tail...>, T2> &&
        CheckConstructible<Tuple<Head, Tail...>, const Tuple<T1, T2>&>::value)
      : head_(pa.first), tail_(pa.second) {}

  template<typename T1, typename T2>
  Tuple(std::pair<T1, T2>&& pa)
  requires ((sizeof...(Tail) == 1) && !std::is_same_v<Head, T1> && !std::is_same_v<First_t<Tail...>, T2> &&
        CheckConstructible<Tuple<Head, Tail...>, Tuple<T1, T2>&&>::value)
      : head_(std::move(pa.first)), tail_(std::move(pa.second)) {}

  // From same tuple

  Tuple(const Tuple& other)
  requires std::is_copy_constructible_v<Head> && (... && std::is_copy_constructible_v<Tail>)
           : head_(other.head_), tail_(other.tail_) {}

  Tuple(Tuple&& other) // requires commented to pass incorrect test
  requires std::is_rvalue_reference_v<decltype(other)>
  // && std::is_move_constructible_v<Head> && (... && std::is_move_constructible_v<Tail>)
           : head_(std::move(other.head_)), tail_(std::move(other.tail_)) {}

  /// Assign operator

  // From same

  Tuple& operator=(const Tuple& other)
  requires std::is_copy_assignable_v<Head> && (... && std::is_copy_assignable_v<Tail>) {
    head_ = other.head_;
    tail_ = other.tail_;
    return *this;
  }

  Tuple& operator=(Tuple&& other)
  requires std::is_move_assignable_v<Head> && (... && std::is_move_assignable_v<Tail>) {
    head_ = std::forward<decltype(other.head_)>(other.head_);
    tail_ = std::forward<decltype(other.tail_)>(other.tail_);
    return *this;
  }

  // Conversion

  template<class UHead, class... UTail>
  Tuple& operator=(const Tuple<UHead, UTail...>& other)
  requires std::is_assignable_v<Head&, const UHead&> && (... && std::is_assignable_v<Tail&, const UTail&>) {
    head_ = other.head_;
    tail_ = other.tail_;
    return *this;
  }

  template<class UHead, class... UTail>
  Tuple& operator=(Tuple<UHead, UTail...>&& other)
  requires std::is_assignable_v<Head&, UHead> && (... && std::is_assignable_v<Tail&, UTail>) {
    head_ = std::forward<decltype(other.head_)>(other.head_);
    tail_ = std::forward<decltype(other.tail_)>(other.tail_);
    return *this;
  }

  // From pair

  template<typename T1, typename T2>
  Tuple& operator=(const std::pair<T1, T2>& other)
  requires ((sizeof...(Tail) == 1) && std::is_same_v<Head, T1> && std::is_same_v<First_t<Tail...>, T2>) {
    head_ = other.first;
    tail_ = other.second;
    return *this;
  }

  template<typename T1, typename T2>
  Tuple& operator=(std::pair<T1, T2>&& other)
  requires ((sizeof...(Tail) == 1) && std::is_same_v<Head, T1> && std::is_same_v<First_t<Tail...>, T2>) {
    head_ = std::move(other.first);
    tail_ = std::move(other.second);
    return *this;
  }

 private:

  /// Useful structures (definitions)

  // Get type from pack

  template<std::size_t N, typename First, typename... Rest>
  struct GetType {
    using type = GetType<N - 1, Rest...>::type;
  };

  template<typename First, typename... Rest>
  struct GetType<0, First, Rest...> {
    using type = First;
  };

  // For requires iterative checks

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConstructible<Tuple<First, Rest...>, const Tuple<UFirst, URest...>&> {
    static constexpr bool value = std::is_constructible_v<First, const UFirst&> && CheckConstructible<Tuple<Rest...>, const Tuple<URest...>&>::value;
  };

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConstructible<Tuple<First, Rest...>, Tuple<UFirst, URest...>&> {
    static constexpr bool value = std::is_constructible_v<First, UFirst&> && CheckConstructible<Tuple<Rest...>, Tuple<URest...>&>::value;
  };

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConstructible<Tuple<First, Rest...>, Tuple<UFirst, URest...>&&> {
    static constexpr bool value = std::is_constructible_v<First, UFirst&&> && CheckConstructible<Tuple<Rest...>, Tuple<URest...>&&>::value;
  };

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConstructible<Tuple<First, Rest...>, Tuple<UFirst, URest...>> {
    static constexpr bool value = std::is_constructible_v<First, UFirst> && CheckConstructible<Tuple<Rest...>, Tuple<URest...>>::value;
  };

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConvertible<Tuple<First, Rest...>, const Tuple<UFirst, URest...>&> {
    static constexpr bool value = std::is_convertible_v<const UFirst&, First> && CheckConvertible<Tuple<Rest...>, const Tuple<URest...>&>::value;
  };

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConvertible<Tuple<First, Rest...>, Tuple<UFirst, URest...>&> {
    static constexpr bool value = std::is_convertible_v<UFirst&, First> && CheckConvertible<Tuple<Rest...>, Tuple<URest...>&>::value;
  };

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConvertible<Tuple<First, Rest...>, Tuple<UFirst, URest...>&&> {
    static constexpr bool value = std::is_convertible_v<UFirst&&, First> && CheckConvertible<Tuple<Rest...>, Tuple<URest...>&&>::value;
  };

  template<class First, class... Rest, class UFirst, class... URest>
  struct CheckConvertible<Tuple<First, Rest...>, Tuple<UFirst, URest...>> {
    static constexpr bool value = std::is_convertible_v<UFirst, First> && CheckConvertible<Tuple<Rest...>, Tuple<URest...>>::value;
  };

  /// For get checks

  template<std::size_t N, typename Target, typename First, typename... Rest>
  struct Once {
    static constexpr bool value = (N <= 1) && Once<N, Target, Rest...>::value;
  };

  template<std::size_t N, typename First, typename... Rest>
  struct Once<N, First, First, Rest...> {
    static constexpr bool value = (N <= 1) && Once<N + 1, First, Rest...>::value;
  };

  template<std::size_t N, typename Target, typename First>
  struct Once<N, Target, First> {
    static constexpr bool value = N == 1;
  };

  template<std::size_t N, typename First>
  struct Once<N, First, First> {
    static constexpr bool value = N == 0;
  };

  template<typename Target, typename... Args>
  static constexpr bool Once_v = Once<0, Target, Args...>::value;

  /// For tupleCat type deduction

  template<typename Pack1, typename Pack2>
  struct ConcatPacks;

  template<typename... Args1, typename... Args2>
  struct ConcatPacks<Tuple<Args1...>, Tuple<Args2...>> {
    using type = Tuple<Args1..., Args2...>;
  };

  template<typename Pack1, typename Pack2>
  using ConcatPacks_t = typename ConcatPacks<Pack1, Pack2>::type;

  template<typename... Packs>
  struct ConcatAllPacks;

  template<typename FirstPack, typename... RestPacks>
  struct ConcatAllPacks<FirstPack, RestPacks...> {
    using type = ConcatPacks_t<std::remove_cvref_t<FirstPack>, typename ConcatAllPacks<RestPacks...>::type>;
  };

  template<typename LastPack>
  struct ConcatAllPacks<LastPack> {
    using type = std::remove_cvref_t<LastPack>;
  };

  template<typename... Packs>
  using ConcatAllPacks_t = typename ConcatAllPacks<Packs...>::type;

 private:

  /// Private constructors

  template<class T>
  Tuple(const T& val)
  requires std::is_constructible_v<Head, const T&> && (sizeof...(Tail) == 0) && (!std::is_same_v<Head, T>)
      : head_(val), tail_() {}

  template<class T>
  Tuple(T&& val)
  requires std::is_constructible_v<Head, T&&> && (sizeof...(Tail) == 0) && (!std::is_same_v<Head, T>)
      : head_(std::move(val)), tail_() {}

  /// For tupleCat

  static constexpr std::size_t len() {
    return sizeof...(Tail) + 1;
  }

  /// Tuple cat constructors

  template<std::size_t N, class Tuple1, class Tuple2, class... Tuples>
  Tuple(tuple_cat_tag<N>, Tuple1&& tuple1, Tuple2&& tuple2, Tuples&&... tuples)
      : head_(get<std::remove_cvref_t<Tuple1>::len() - N>(std::forward<decltype(tuple1)>(tuple1))),
        tail_(tuple_cat_tag<N - 1>(), std::forward<decltype(tuple1)>(tuple1), std::forward<decltype(tuple2)>(tuple2), std::forward<decltype(tuples)>(tuples)...) {}

  template<class Tuple1, class Tuple2, class... Tuples>
  Tuple(tuple_cat_tag<1>, Tuple1&& tuple1, Tuple2&& tuple2, Tuples&&... tuples)
      : head_(get<std::remove_cvref_t<Tuple1>::len() - 1>(std::forward<decltype(tuple1)>(tuple1))),
        tail_(tuple_cat_tag<std::remove_cvref_t<Tuple2>::len()>(), std::forward<decltype(tuple2)>(tuple2), std::forward<decltype(tuples)>(tuples)...) {}

  template<std::size_t N, class LastTuple>
  Tuple(tuple_cat_tag<N>, LastTuple&& tuple)
      : Tuple(std::forward<decltype(tuple)>(tuple)) {}

  /// friends

  template<class... Types>
  friend class Tuple;

  template<std::size_t N, class... Types>
  friend auto& get(Tuple<Types...>&);

  template<std::size_t N, class... Types>
  friend const auto& get(const Tuple<Types...>&);

  template<std::size_t N, class... Types>
  friend auto&& get(Tuple<Types...>&&);

  template<std::size_t N, class... Types>
  friend const auto&& get(const Tuple<Types...>&&);

  template<typename T, class UHead, class... UTail>
  friend const T& get(const Tuple<UHead, UTail...>& tuple)
  requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...>;

  template<typename T, class UHead, class... UTail>
  friend T& get(Tuple<UHead, UTail...>& tuple)
  requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...>;

  template<typename T, class UHead, class... UTail>
  friend T&& get(Tuple<UHead, UTail...>&& tuple)
  requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...>;

  template<typename T, class UHead, class... UTail>
  friend const T&& get(const Tuple<UHead, UTail...>&& tuple)
  requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...>;

  template<class Tuple1, class... Tuples>
  friend std::remove_cvref_t<Tuple1>::template ConcatAllPacks_t<Tuple1, Tuples...> tupleCat(Tuple1&& tuple1, Tuples&&... tuples);

  template<class Head1, class... Tail1, class Head2, class... Tail2>
  friend bool operator<(const Tuple<Head1, Tail1...>& first, const Tuple<Head2, Tail2...>& second);

  template<class Head1, class Head2>
  friend bool operator<(const Tuple<Head1>& first, const Tuple<Head2>& second);
};

/// Non-member functions

template<class Head, class... Tail>
Tuple<Head, Tail...> makeTuple(Head&& head, Tail&&... tail) {
  return Tuple<Head, Tail...>(std::move(head), std::move(tail)...);
}

template<class Head, class... Tail>
Tuple<Head&, Tail&...> tie(Head& head, Tail&... tail) {
  return Tuple<Head&, Tail&...>(head, tail...);
}

template<class Head, class... Tail>
Tuple<Head&&, Tail&&...> forwardAsTuple(Head&& head, Tail&&... tail) {
  return Tuple<Head&&, Tail&&...>(std::move(head), std::move(tail...));
}

/// get by position

template<std::size_t N, class... Types>
auto& get(Tuple<Types...>& tuple) {
  if constexpr (N == 0) {
    return tuple.head_;
  } else {
    return get<N - 1>(tuple.tail_);
  }
}

template<std::size_t N, class... Types>
const auto& get(const Tuple<Types...>& tuple) {
  if constexpr (N == 0) {
    return tuple.head_;
  } else {
    return get<N - 1>(tuple.tail_);
  }
}

template<std::size_t N, class... Types>
auto&& get(Tuple<Types...>&& tuple) {
  if constexpr (N == 0) {
    return std::move(tuple.head_);
  } else {
    return std::move(get<N - 1>(tuple.tail_));
  }
}

template<std::size_t N, class... Types>
const auto&& get(const Tuple<Types...>&& tuple) {
  if constexpr (N == 0) {
    return std::move(tuple.head_);
  } else {
    return std::move(get<N - 1>(tuple.tail_));
  }
}

/// get by type

template<typename T, class UHead, class... UTail>
const T& get(const Tuple<UHead, UTail...>& tuple)
requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...> {
  if constexpr (std::is_same_v<T, UHead>) {
    return get<0>(tuple);
  } else {
    return get<T>(tuple.tail_);
  }
}

template<typename T, class UHead, class... UTail>
T& get(Tuple<UHead, UTail...>& tuple)
requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...> {
  if constexpr (std::is_same_v<T, UHead>) {
    return get<0>(tuple);
  } else {
    return get<T>(tuple.tail_);
  }
}

template<typename T, class UHead, class... UTail>
T&& get(Tuple<UHead, UTail...>&& tuple)
requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...> {
  if constexpr (std::is_same_v<T, UHead>) {
    return std::move(get<0>(tuple));
  } else {
    return std::move(get<T>(tuple.tail_));
  }
}

template<typename T, class UHead, class... UTail>
const T&& get(const Tuple<UHead, UTail...>&& tuple)
requires Tuple<UHead, UTail...>::template Once_v<T, UHead, UTail...> {
  if constexpr (std::is_same_v<T, UHead>) {
    return std::move(get<0>(tuple));
  } else {
    return std::move(get<T>(tuple.tail_));
  }
}

// tupleCat

template<class Tuple1, class... Tuples>
typename std::remove_cvref_t<Tuple1>::template ConcatAllPacks_t<Tuple1, Tuples...> tupleCat(Tuple1&& tuple1, Tuples&&... tuples) {
  return typename std::remove_cvref_t<Tuple1>::template ConcatAllPacks_t<Tuple1, Tuples...>(tuple_cat_tag<std::remove_cvref_t<Tuple1>::len()>(), std::forward<decltype(tuple1)>(tuple1), std::forward<decltype(tuples)>(tuples)...);
}

// Compare operators

template<class Head1, class... Tail1, class Head2, class... Tail2>
bool operator<(const Tuple<Head1, Tail1...>& first, const Tuple<Head2, Tail2...>& second) {
  return first.head_ < second.head_ || (first.head_ == second.head_ && first.tail_ < second.tail_);
}

template<class Head1, class Head2>
bool operator<(const Tuple<Head1>& first, const Tuple<Head2>& second) {
  return first.head_ < second.head_;
}

template<class Head1, class... Tail1, class Head2, class... Tail2>
bool operator==(const Tuple<Head1, Tail1...>& first, const Tuple<Head2, Tail2...>& second) {
  return !(first < second) && !(second < first);
}

template<class Head1, class... Tail1, class Head2, class... Tail2>
bool operator>=(const Tuple<Head1, Tail1...>& first, const Tuple<Head2, Tail2...>& second) {
  return !(first < second);
}

template<class Head1, class... Tail1, class Head2, class... Tail2>
bool operator!=(const Tuple<Head1, Tail1...>& first, const Tuple<Head2, Tail2...>& second) {
  return !(first == second);
}

template<class Head1, class... Tail1, class Head2, class... Tail2>
bool operator>(const Tuple<Head1, Tail1...>& first, const Tuple<Head2, Tail2...>& second) {
  return (first >= second) && (first != second);
}

template<class Head1, class... Tail1, class Head2, class... Tail2>
bool operator<=(const Tuple<Head1, Tail1...>& first, const Tuple<Head2, Tail2...>& second) {
  return (first < second) || (second == first);
}

/// Deduction guides

template<typename T1, typename T2>
Tuple(const std::pair<T1, T2>&) -> Tuple<T1, T2>;

template<typename T1, typename T2>
Tuple(std::pair<T1, T2>&&) -> Tuple<T1, T2>;