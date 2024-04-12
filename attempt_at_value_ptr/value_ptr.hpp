/**
* @brief TODO
* @author Matthew Guidry (github: mguid65)
* @date 2/24/24
*/

#ifndef VALUE_PTR_HPP
#define VALUE_PTR_HPP

#include <memory>
#include <type_traits>

namespace mguid {

template <class TType, template <class...> class TPrimary>
struct IsSpecializationOf : std::false_type {};

template <template <class...> class TPrimary, class... TArgs>
struct IsSpecializationOf<TPrimary<TArgs...>, TPrimary> : std::true_type {};

template <class TType, template <class...> class TPrimary>
inline constexpr bool IsSpecializationOfV =
   IsSpecializationOf<TType, TPrimary>::value;

template <typename TValueType>
class ValuePtr {
public:
 // DEFAULT CONSTRUCTION

 constexpr ValuePtr() requires(!std::is_default_constructible_v<TValueType>) = delete;

 constexpr ValuePtr() noexcept(
     std::is_nothrow_default_constructible_v<TValueType>)
   requires(std::is_default_constructible_v<TValueType>)
     : m_ptr{std::make_unique<TValueType>()} {}

 // COPY CONSTRUCTION FROM VALUE_PTR<TVALUETYPE>

 constexpr ValuePtr(const ValuePtr<TValueType>&)
   requires(!std::is_copy_constructible_v<TValueType>)
 = delete;

 constexpr ValuePtr(const ValuePtr<TValueType>& other) noexcept(
     std::is_nothrow_copy_constructible_v<TValueType>)
   requires(std::is_copy_constructible_v<TValueType>)

     : m_ptr{std::make_unique<TValueType>(*other)} {}

 // COPY ASSIGNMENT FROM VALUE_PTR<TVALUETYPE>

 constexpr ValuePtr& operator=(const ValuePtr<TValueType>&)
   requires(!std::is_copy_assignable_v<TValueType>)
 = delete;

 constexpr ValuePtr& operator=(const ValuePtr<TValueType>& other) noexcept(
     std::is_nothrow_copy_assignable_v<TValueType>)
   requires(std::is_copy_assignable_v<TValueType>)
 {
   if (&other != this) { m_ptr = std::make_unique<TValueType>(*other); }
   return *this;
 }

 // MOVE CONSTRUCTION FROM VALUE_PTR<TVALUETYPE>

 constexpr ValuePtr(ValuePtr<TValueType>&&)
   requires(!std::is_move_constructible_v<TValueType>)
 = delete;

 constexpr ValuePtr(ValuePtr<TValueType>&& other)

     noexcept(std::is_nothrow_move_constructible_v<TValueType>)
   requires(std::is_move_constructible_v<TValueType>)
     : m_ptr{std::move(other.m_ptr)} {}

 // MOVE ASSIGNMENT FROM VALUE_PTR<TVALUETYPE>

 constexpr ValuePtr& operator=(ValuePtr<TValueType>&&)
   requires(!std::is_move_assignable_v<TValueType>)
 = delete;

 constexpr ValuePtr& operator=(ValuePtr<TValueType>&& other) noexcept(
     std::is_nothrow_move_assignable_v<TValueType>)
   requires(std::is_move_assignable_v<TValueType>)
 {
   m_ptr = std::move(other.m_ptr);
   return *this;
 }

 // COPY CONSTRUCT FROM VALUE_PTR<OTHER>

 template <typename TOtherType>
 constexpr ValuePtr(const ValuePtr<TOtherType>&)
   requires(!std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
            !std::is_constructible_v<
                TValueType, std::add_const_t<std::add_lvalue_reference_t<
                                std::remove_cvref_t<TOtherType>>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr(const ValuePtr<TOtherType>& other) noexcept(
     std::is_nothrow_constructible_v<
         TValueType, std::add_const_t<std::add_lvalue_reference_t<
                         std::remove_cvref_t<TOtherType>>>>)
   requires(!std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
            std::is_constructible_v<
                TValueType, std::add_const_t<std::add_lvalue_reference_t<
                                std::remove_cvref_t<TOtherType>>>>)
     : m_ptr{std::make_unique<TValueType>(*other)} {}

 // COPY ASSIGNMENT FROM VALUE_PTR<OTHER>

 template <typename TOtherType>
 constexpr ValuePtr& operator=(const ValuePtr<TOtherType>&)
   requires(!std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
            !std::is_assignable_v<TValueType,
                                  std::add_const_t<std::add_lvalue_reference_t<
                                      std::remove_cvref_t<TOtherType>>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr& operator=(const ValuePtr<TOtherType>& other) noexcept(
     std::is_nothrow_assignable_v<TValueType,
                                  std::add_const_t<std::add_lvalue_reference_t<
                                      std::remove_cvref_t<TOtherType>>>>)
   requires(!std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
            std::is_assignable_v<TValueType,
                                 std::add_const_t<std::add_lvalue_reference_t<
                                     std::remove_cvref_t<TOtherType>>>>)
 {
   // Not sure this is possible
   if (&other != this) { m_ptr = std::make_unique<TValueType>(*other); }
   return *this;
 }

 // MOVE CONSTRUCTION FROM VALUE_PTR<OTHER>

 template <typename TOtherType>
 constexpr ValuePtr(ValuePtr<TOtherType>&&)
   requires(!std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
            !std::is_constructible_v<TValueType,
                                     std::add_rvalue_reference_t<
                                         std::remove_reference_t<TOtherType>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr(ValuePtr<TOtherType>&& other) noexcept(
     std::is_nothrow_constructible_v<
         TValueType,
         std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>>)
   requires(
       !std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
       std::is_constructible_v<
           TValueType,
           std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>>)
     : m_ptr{std::move(other.m_ptr)} {}

 // MOVE ASSIGNMENT FROM VALUE_PTR<OTHER>

 template <typename TOtherType>
 constexpr ValuePtr& operator=(ValuePtr<TOtherType>&&)
   requires(!std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
            !std::is_assignable_v<TValueType,
                                  std::add_rvalue_reference_t<
                                      std::remove_reference_t<TOtherType>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr& operator=(ValuePtr<TOtherType>&& other) noexcept(
     std::is_nothrow_assignable_v<
         TValueType,
         std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>>)
   requires(
       !std::same_as<std::remove_cvref_t<TOtherType>, TValueType> &&
       std::is_assignable_v<
           TValueType,
           std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>>)
 {
   m_ptr = std::move(other.m_ptr);
   return *this;
 }

 // COPY CONSTRUCT FROM OTHER

 template <typename TOtherType>
 constexpr ValuePtr(const TOtherType&)
   requires(!IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
            !std::is_constructible_v<
                TValueType, std::add_const_t<std::add_lvalue_reference_t<
                                std::remove_cvref_t<TOtherType>>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr(const TOtherType& other) noexcept(
     std::is_nothrow_constructible_v<
         TValueType, std::add_const_t<std::add_lvalue_reference_t<
                         std::remove_cvref_t<TOtherType>>>>)
   requires(!IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
            std::is_constructible_v<
                TValueType, std::add_const_t<std::add_lvalue_reference_t<
                                std::remove_cvref_t<TOtherType>>>>)
     : m_ptr{std::make_unique<TValueType>(other)} {}

 // COPY ASSIGNMENT FROM OTHER

 template <typename TOtherType>
 constexpr ValuePtr& operator=(const TOtherType&)
   requires(!IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
            !std::is_assignable_v<TValueType,
                                  std::add_const_t<std::add_lvalue_reference_t<
                                      std::remove_cvref_t<TOtherType>>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr& operator=(const TOtherType& other) noexcept(
     std::is_nothrow_assignable_v<TValueType,
                                  std::add_const_t<std::add_lvalue_reference_t<
                                      std::remove_cvref_t<TOtherType>>>>)
   requires(!IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
            std::is_assignable_v<TValueType,
                                 std::add_const_t<std::add_lvalue_reference_t<
                                     std::remove_cvref_t<TOtherType>>>>)
 {
   *m_ptr = other;
   return *this;
 }

 // MOVE CONSTRUCTION FROM OTHER

 template <typename TOtherType>
 constexpr ValuePtr(
     std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>)
   requires(std::is_rvalue_reference_v<TOtherType> &&
            !IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
            !std::is_constructible_v<TValueType,
                                     std::add_rvalue_reference_t<
                                         std::remove_reference_t<TOtherType>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr(
     std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>
         other) noexcept(std::
                             is_nothrow_constructible_v<
                                 TValueType,
                                 std::add_rvalue_reference_t<
                                     std::remove_reference_t<TOtherType>>>)
   requires(
       std::is_rvalue_reference_v<TOtherType> &&
       !IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
       std::is_constructible_v<
           TValueType,
           std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>>)
     : m_ptr{std::make_unique<TValueType>(std::move(other))} {}

 // MOVE ASSIGNMENT FROM VALUE_PTR<OTHER>

 template <typename TOtherType>
 constexpr ValuePtr& operator=(
     std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>)
   requires(std::is_rvalue_reference_v<TOtherType> &&
            !IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
            !std::is_assignable_v<TValueType,
                                  std::add_rvalue_reference_t<
                                      std::remove_reference_t<TOtherType>>>)
 = delete;

 template <typename TOtherType>
 constexpr ValuePtr& operator=(
     std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>
         other) noexcept(std::
                             is_nothrow_assignable_v<
                                 TValueType,
                                 std::add_rvalue_reference_t<
                                     std::remove_reference_t<TOtherType>>>)
   requires(
       std::is_rvalue_reference_v<TOtherType> &&
       !IsSpecializationOfV<std::remove_cvref_t<TOtherType>, ValuePtr> &&
       std::is_assignable_v<
           TValueType,
           std::add_rvalue_reference_t<std::remove_reference_t<TOtherType>>>)
 {
   *m_ptr = std::move(other);
   return *this;
 }

 friend constexpr void swap(ValuePtr& lhs, ValuePtr& rhs) noexcept {
   std::swap(lhs.m_ptr, rhs.m_ptr);
 }

 constexpr const TValueType& operator*() const { return m_ptr.operator*(); }
 constexpr TValueType& operator*() { return m_ptr.operator*(); }
 constexpr const TValueType* operator->() const { return m_ptr.operator->(); }
 constexpr TValueType* operator->() { return m_ptr.operator->(); }

 constexpr ~ValuePtr() = default;

private:
 std::unique_ptr<TValueType> m_ptr{nullptr};
};

template <typename TValueType, typename... TArgs>
ValuePtr<TValueType> MakeValuePtr(TArgs&&... args) {
 return ValuePtr<TValueType>(TValueType(std::forward<TArgs>(args)...));
}

}  // namespace mguid

#endif  // VALUE_POINTER_HPP