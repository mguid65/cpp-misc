#include <array>
#include <bit>
#include <climits>
#include <cstdint>
#include <iostream>
#include <limits>

constexpr std::array<std::uint_least8_t, 256> get_bit_count_table() {
  constexpr auto popcount = [](std::uint_least8_t val) {
    return (val & 1) + ((val >> 1) & 1) + ((val >> 2) & 1) + ((val >> 3) & 1) +
           ((val >> 4) & 1) + ((val >> 5) & 1) + ((val >> 6) & 1) +
           ((val >> 7) & 1);
  };

  return [&]<std::size_t... NIdxs>(std::index_sequence<NIdxs...>) {
    std::array<std::uint_least8_t, 256> result;
    ([&]() { result[NIdxs] = popcount(NIdxs); }(), ...);
    return result;
  }(std::make_index_sequence<256>{});
}

constexpr int popcount_lut(std::integral auto val) {
  using u_type = std::make_unsigned_t<decltype(val)>;

  auto u_val = static_cast<u_type>(val);
  constexpr auto divisions = 1 + ((sizeof(u_type) * CHAR_BIT) / 8);
  constexpr auto bit_count_lut = get_bit_count_table();

  int count{0u};
  for (auto i{0u}; i < divisions; ++i) {
    count += bit_count_lut[static_cast<std::uint8_t>(u_val)];
    u_val >>= 8;
  }
  return count;
};

auto main() -> int {
  std::cout << popcount_lut(std::numeric_limits<std::int_least8_t>::min())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least8_t>::min())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least16_t>::min())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least16_t>::min())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least32_t>::min())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least32_t>::min())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least64_t>::min())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least64_t>::min())
            << std::endl;

  std::cout << popcount_lut(std::numeric_limits<std::int_least8_t>::max())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least8_t>::max())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least16_t>::max())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least16_t>::max())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least32_t>::max())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least32_t>::max())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least64_t>::max())
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least64_t>::max())
            << std::endl;

  std::cout << popcount_lut(std::numeric_limits<std::int_least8_t>::max() >> 4)
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least8_t>::max() >> 4)
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least16_t>::max() >> 8)
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least16_t>::max() >>
                            8)
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least32_t>::max() >>
                            16)
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least32_t>::max() >>
                            16)
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::int_least64_t>::max() >>
                            32)
            << std::endl;
  std::cout << popcount_lut(std::numeric_limits<std::uint_least64_t>::max() >>
                            32)
            << std::endl;
}