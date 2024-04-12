#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>

template <typename TValueType>
struct BinaryTreeNodeImpl;

template <typename TValueType>
struct BinaryTreeNode {
  constexpr BinaryTreeNode();
  constexpr BinaryTreeNode(TValueType&& value);

  BinaryTreeNode(const BinaryTreeNode<TValueType>& other);
  BinaryTreeNode& operator=(const BinaryTreeNode<TValueType>& other);
  BinaryTreeNode(BinaryTreeNode<TValueType>&&) noexcept = default;
  BinaryTreeNode& operator=(BinaryTreeNode<TValueType>&&) noexcept = default;

  [[nodiscard]] const TValueType& Value() const noexcept {
    return m_impl->m_value;
  }
  [[nodiscard]] TValueType& Value() noexcept { return m_impl->m_value; }

  void SetValue(TValueType&& val) {
    m_impl->m_value = std::forward<TValueType>(val);
  }

  BinaryTreeNode<TValueType>& operator=(TValueType&& val) {
    m_impl->m_value = std::forward<TValueType>(val);
    return *this;
  }

  std::optional<BinaryTreeNode<TValueType>>& SetLeftChild(
      const BinaryTreeNode<TValueType>& node) {
    m_impl->m_left = node;
    return m_impl->m_left;
  }

  std::optional<BinaryTreeNode<TValueType>>& SetRightChild(
      const BinaryTreeNode<TValueType>& node) {
    m_impl->m_right = node;
    return m_impl->m_right;
  }

  std::optional<BinaryTreeNode<TValueType>>& GetLeftChild(
      const BinaryTreeNode<TValueType>& node) {
    return m_impl->m_left;
  }

  std::optional<BinaryTreeNode<TValueType>>& GetRightChild(
      const BinaryTreeNode<TValueType>& node) {
    return m_impl->m_right;
  }

private:
  std::unique_ptr<BinaryTreeNodeImpl<TValueType>> m_impl;
};

template <typename TValueType>
struct BinaryTreeNodeImpl {
  TValueType m_value;
  std::optional<BinaryTreeNode<TValueType>> m_left;
  std::optional<BinaryTreeNode<TValueType>> m_right;
};

template <typename TValueType>
constexpr BinaryTreeNode<TValueType>::BinaryTreeNode()
    : m_impl{std::make_unique<BinaryTreeNodeImpl<TValueType>>()} {}

template <typename TValueType>
constexpr BinaryTreeNode<TValueType>::BinaryTreeNode(TValueType&& value)
    : m_impl{std::make_unique<BinaryTreeNodeImpl<TValueType>>(
          std::forward<TValueType>(value), std::nullopt, std::nullopt)} {}

template <typename TValueType>
BinaryTreeNode<TValueType>::BinaryTreeNode(
    const BinaryTreeNode<TValueType>& other)
    : m_impl{std::make_unique<BinaryTreeNodeImpl<TValueType>>(
          other.m_impl->m_value, other.m_impl->m_left, other.m_impl->m_right)} {
}

template <typename TValueType>
BinaryTreeNode<TValueType>& BinaryTreeNode<TValueType>::operator=(
    const BinaryTreeNode<TValueType>& other) {
  m_impl = std::make_unique<BinaryTreeNodeImpl<TValueType>>(
      other.m_impl->m_value, other.m_impl->m_left, other.m_impl->m_right);
  return *this;
}

template <typename TValueType>
using BinaryTree = BinaryTreeNode<TValueType>;

int main() {
  BinaryTree<int> bt{0};
  bt.SetRightChild(BinaryTreeNode<int>{1});
}