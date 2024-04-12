#include <functional>
#include <iostream>
#include <type_traits>
#include <unordered_map>

template <typename TValueType,
          typename TUpdater =
              decltype([](TValueType&& new_val, TValueType& held) {
                held = new_val;
                return true;
              }),
          bool = std::is_default_constructible_v<TValueType>,
          bool = std::is_move_constructible_v<TValueType>,
          bool = std::is_copy_constructible_v<TValueType>,
          bool = std::is_copy_assignable_v<TValueType>,
          bool = std::is_move_assignable_v<TValueType>>
struct Observable {
  Observable() noexcept(std::is_nothrow_default_constructible_v<TValueType>)
      : m_value{} {}

  Observable(const Observable&) = delete;
  Observable& operator=(const Observable&) = delete;

  Observable(Observable&&) noexcept(
      std::is_nothrow_move_constructible_v<TValueType>) = default;
  Observable& operator=(Observable&&) noexcept(
      std::is_nothrow_move_assignable_v<TValueType>) = default;

  Observable(const TValueType& value) : m_value{value} {}
  Observable(TValueType&& value) : m_value{std::move(value)} {}

  Observable& operator=(const TValueType& value) {
    m_value = value;
    Notify(m_value);
    return *this;
  }

  Observable& operator=(TValueType&& value) {
    m_value = std::move(value);
    Notify(m_value);
    return *this;
  }

  void Update(const TValueType& val) {
    static constexpr auto updater = TUpdater{};
    if (updater(val, m_value)) {
      Notify(m_value);
    }
  }

  void Update(TValueType&& val) {
    static constexpr auto updater = TUpdater{};
    if (updater(std::forward<TValueType>(val), m_value)) {
      Notify(m_value);
    }
  }

  void Notify(TValueType last) const {
    for (const auto& sub : m_subs) {
      sub(last);
    }
  }

  void Subscribe(std::function<void(TValueType)>&& func) {
    m_subs.push_back(std::move(func));
  }

private:
  std::vector<std::function<void(TValueType)>> m_subs;
  TValueType m_value;
};

int main() {
  [[maybe_unused]] Observable<int> o;
  o.Subscribe(
      [](auto last) { std::cout << "Value Changed To:" << last << '\n'; });

  const int i{1};
  o = i;
  int j{2};
  o = std::move(j);
}