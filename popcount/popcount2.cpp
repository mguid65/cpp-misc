#include <array>
#include <bit>
#include <bitset>
#include <climits>
#include <cstdint>
#include <iostream>
#include <limits>
#include <cassert>

template <std::size_t NBitWidth, typename TFirst, typename... TOther>
struct smallest_fitting {
private:
  using rhs_recursive_type =
      typename smallest_fitting<NBitWidth, TOther...>::type;

public:
  using type =
      typename std::conditional<(sizeof(TFirst) * CHAR_BIT) >= NBitWidth,
                                TFirst, rhs_recursive_type>::type;
};

template <std::size_t NBitWidth, typename TFirst>
struct smallest_fitting<NBitWidth, TFirst> {
  static_assert(sizeof(TFirst) * CHAR_BIT >= NBitWidth);
  using type = TFirst;
};

template <std::size_t NBitWidth, typename... TOther>
using smallest_fitting_t = smallest_fitting<NBitWidth, TOther...>::type;

template <std::size_t NBitWidth>
struct uint_least {
  using type = smallest_fitting_t<NBitWidth, std::uint_least8_t, std::uint_least16_t, std::uint_least32_t, std::uint_least64_t>;
};

template <std::size_t NBitWidth>
using uint_least_t = uint_least<NBitWidth>::type;

template <std::size_t NBitWidth = 8>
constexpr auto get_bit_count_table() {
  static_assert(NBitWidth < 32);
  using entry_type = uint_least_t<NBitWidth>;
  constexpr auto num_entries = 1ul << NBitWidth;
  using r_type = std::array<std::uint_least8_t, num_entries>;

  constexpr auto popcount = [](entry_type val) {
    auto count{0u};
    for (auto i{0u}; i < NBitWidth; ++i) {
      count += ((val >> i) & 1);
    }
    return count;
  };

  return [&]<std::size_t... NIdxs>(std::index_sequence<NIdxs...>) {
    std::array<std::uint_least8_t, num_entries> result;
    ([&]() { result[NIdxs] = popcount(NIdxs); }(), ...);
    return result;
  }(std::make_index_sequence<num_entries>{});
}

template <std::size_t NTableBitWidth = 8>
constexpr int popcount_lut(std::integral auto val) {
  static_assert(NTableBitWidth < 32);
  using u_type = std::make_unsigned_t<decltype(val)>;

  auto u_val = static_cast<u_type>(val);
  constexpr auto divisions = 1 + ((sizeof(u_type) * CHAR_BIT) / NTableBitWidth);
  constexpr auto bit_count_lut = get_bit_count_table<NTableBitWidth>();
  constexpr auto mask = (1ul << (NTableBitWidth)) - 1;

  int count{0u};
  for (auto i{0u}; i < divisions; ++i) {
    count += bit_count_lut[static_cast<uint_least_t<NTableBitWidth>>(mask & u_val)];
    u_val >>= NTableBitWidth;
  }
  return count;
};

auto main() -> int {
  for (auto i{0u}; i < 1024; ++i) {
    assert(popcount_lut<10>(i) == std::popcount(i));
  }

  std::cout << popcount_lut<10>(std::uint_least32_t(-1));
}