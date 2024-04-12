#include <array>
#include <iostream>
#include <tuple>

template <typename TArray, std::size_t... TIntSeq>
constexpr auto array_to_tuple_impl(TArray&& a,
                                   std::index_sequence<TIntSeq...>) noexcept {
  return std::make_tuple(std::forward<TArray>(a)[TIntSeq]...);
}

template <template <typename, std::size_t> typename TArr, typename T,
          std::size_t N, typename TIndices = std::make_index_sequence<N>>
constexpr auto array_to_tuple(TArr<T, N>&& a) noexcept {
  return array_to_tuple_impl(std::forward<TArr<T, N>>(a), TIndices{});
}

template <typename TFunc, template <typename, std::size_t> typename TArr,
          typename T, std::size_t N>
constexpr decltype(auto) apply(TFunc&& func, TArr<T, N>&& arr) noexcept {
  return std::apply(func, array_to_tuple(std::forward<TArr<T, N>>(arr)));
}

int main() {
  const auto arr = std::array{1, 2, 3, 4, 5, 6};

  // In c++23, std::apply takes anything that is TupleLike: pair, tuple,
  // array, ...
  std::cout << apply([](auto... vals) { return (vals + ...); }, arr) << '\n';
}
