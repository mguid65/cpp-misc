#include <array>
#include <concepts>
#include <functional>
#include <iostream>
#include <type_traits>
#include <optional>

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

template <typename T>
concept ImplementsUnsafe =
    std::same_as<std::void_t<decltype(std::declval<
                                      typename T::template Unsafe<false>>())>,
                 void> &&
    std::same_as<std::void_t<decltype(std::declval<
                                      typename T::template Unsafe<true>>())>,
                 void>;

template <ImplementsUnsafe TUnderlying>
struct UnsafeProvider : TUnderlying {
  template <typename TFunc>
  void unsafe(TFunc&& func)
    requires is_const_rvalue_reference<typename generic_closure_traits<
                 decltype(&std::remove_reference_t<decltype(func)>::template
                          operator()<typename TUnderlying::template Unsafe<
                              false>>)>::template argt<0>> ||
             is_const_rvalue_reference<typename generic_closure_traits<
                 decltype(&std::remove_reference_t<
                          decltype(func)>::operator())>::template argt<0>>
  {
    return std::invoke(std::forward<TFunc>(func),
                       typename TUnderlying::template Unsafe<false>(
                           static_cast<TUnderlying&>(*this)));
  }

  template <typename TFunc>
  void const_unsafe(TFunc&& func) const
    requires is_const_rvalue_reference<typename generic_closure_traits<
                 decltype(&std::remove_reference_t<decltype(func)>::template
                          operator()<typename TUnderlying::template Unsafe<
                              true>>)>::template argt<0>> ||
             is_const_rvalue_reference<typename generic_closure_traits<
                 decltype(&std::remove_reference_t<
                          decltype(func)>::operator())>::template argt<0>>
  {
    return std::invoke(std::forward<TFunc>(func),
                       typename TUnderlying::template Unsafe<true>(
                           static_cast<const TUnderlying&>(*this)));
  }
};

template <typename T, std::size_t NSize>
struct SafeArrayImpl {
private:
  std::array<T, NSize> m_array;

public:
  template <bool TConst = false>
  struct Unsafe {
    Unsafe() = delete;
    Unsafe(const Unsafe&) = delete;
    Unsafe& operator=(const Unsafe&) = delete;
    Unsafe(Unsafe&&) = delete;
    Unsafe& operator=(Unsafe&&) = delete;

    decltype(auto) operator[](std::size_t idx) const {
      return m_ref.m_array[idx];
    }

    std::conditional_t<TConst, const SafeArrayImpl&, SafeArrayImpl&> safe()
        const {
      return m_ref;
    }

  private:
    std::conditional_t<TConst, const SafeArrayImpl&, SafeArrayImpl&> m_ref;

    Unsafe(std::conditional_t<TConst, const SafeArrayImpl&, SafeArrayImpl&> ref)
        : m_ref{ref} {}
    friend UnsafeProvider<SafeArrayImpl<T, NSize>>;
  };

  std::optional<std::reference_wrapper<T>> operator[](std::size_t idx) {
    if (idx >= NSize) {
      return std::nullopt;
    }
    return std::ref(m_array[idx]);
  }

  std::optional<std::reference_wrapper<const T>> operator[](std::size_t idx) const {
    if (idx >= NSize) {
      return std::nullopt;
    }
    return std::cref(m_array[idx]);
  }
};

template <typename T, std::size_t NSize>
using SafeArray = UnsafeProvider<SafeArrayImpl<T, NSize>>;

int main() {
  SafeArray<int, 10> sa{};
  sa.unsafe([](const auto&& unsafe_api) {
    unsafe_api[9] = 42;
  });

  sa.const_unsafe([](const auto&& unsafe_api) {
    std::cout << unsafe_api[9] << std::endl;
  });

  std::cout << sa[9].value().get() << std::endl;
}