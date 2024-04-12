#include <array>
#include <concepts>
#include <tuple>
#include <type_traits>

template <typename T>
concept is_const_rvalue_reference = std::is_rvalue_reference_v<T> &&
                                    std::is_const_v<std::remove_reference_t<T>>;

template <typename...>
struct generic_closure_traits;

template <typename TClassType, typename TReturnType, typename... TArgs>
struct generic_closure_traits<TReturnType (TClassType::*)(TArgs...) const> {
  static constexpr auto argc = sizeof...(TArgs);

  using result_type = TReturnType;

  template <std::size_t NIdx>
  using argt = std::tuple_element_t<NIdx, std::tuple<TArgs..., void>>;
};

template <typename TFunc>
void foo(TFunc&& func)
  requires is_const_rvalue_reference<typename generic_closure_traits<
               decltype(&std::remove_reference_t<decltype(func)>::template
                        operator()<int>)>::template argt<0>> ||
           is_const_rvalue_reference<typename generic_closure_traits<
               decltype(&std::remove_reference_t<decltype(func)>::operator())>::
                                         template argt<0>>
{}

int main() {
  foo([](const auto&&) {});
  foo([](const int&&) {});
}