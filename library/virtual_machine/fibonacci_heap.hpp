//! Fibonacci heap for use as a garbage collector storage

#pragma once

#include <gsl/gsl>

#include <cstdint>
#include <memory>
#include <mutex>

namespace chimera::library::virtual_machine {
  //! Fibonacci heap
  //! follows std::set/list for methods provided
  template <typename Key, typename Compare = std::less<Key>,
            typename Allocator = std::allocator<Key>>
  struct FibonacciHeap {
    FibonacciHeap() noexcept = default;
    template <typename... Args>
    explicit FibonacciHeap(Args &&...args) {
      Node node{std::forward<Args>(args)...};
      min = &node;
    }
    FibonacciHeap(const FibonacciHeap &fibonacciHeap) = delete;
    FibonacciHeap(FibonacciHeap &&fibonacciHeap) noexcept = default;
    ~FibonacciHeap() noexcept {
      if (min != nullptr && min == min->get_left()) {
        min->remove_left();
        min = nullptr;
      }
      if (min != nullptr) {
        min->reset_left();
      }
    }
    auto operator=(const FibonacciHeap &fibonacciHeap)
        -> FibonacciHeap & = delete;
    auto operator=(FibonacciHeap &&fibonacciHeap) noexcept
        -> FibonacciHeap & = default;
    template <typename Compare2>
    void merge(FibonacciHeap<Key, Compare2, Allocator> &&source) {
      if (min == nullptr) {
        using std::swap;
        swap(min, source.min);
      } else if (source.min != nullptr) {
        if (source.min < min) {
          min = source.min;
          source.min = nullptr;
        }
      }
      n += source.n;
      source.n = 0;
    }
    template <typename... Args>
    [[nodiscard]] auto emplace(Args &&...args) -> const Key & {
      auto new_node = std::make_unique<Node>(Key{std::forward<Args>(args)...});
      if (min == nullptr) {
        min = (new_node->left = std::move(new_node)).get_left();
        min->right = min;
        ++n;
        return min->key;
      }
      if (new_node < min) {
        min = (new_node->left = std::move(new_node)).get_left();
        min->right = min;
        ++n;
        return min->key;
      }
      new_node->left = std::move(min->left);
      new_node->left->right = new_node.get();
      new_node->right = min;
      min->left = std::move(new_node);
      ++n;
      return min->key;
    }
    template <typename Predicate>
    void remove_if(Predicate && /*predicate*/) {}

  private:
    //! Internal node structure
    struct Node {
      template <typename... Args>
      explicit Node(Args &&...args) : key(std::forward<Args>(args)...) {}
      Node(const Node &node) = delete;
      Node(Node &&node) noexcept = default;
      ~Node() noexcept {
        if (child != nullptr) {
          child->reset_left();
        }
      }
      auto operator=(const Node &node) -> Node & = delete;
      auto operator=(Node &&node) noexcept -> Node & = default;
      [[nodiscard]] auto operator<(const Node &node) -> bool;
      [[nodiscard]] auto get_left() const -> const Node * { return left.get(); }
      void remove_left() { std::ignore = std::move(left); }
      void reset_left() {
        if (left) {
          auto ptr = std::move(left);
          ptr.reset();
        }
      }

    private:
      Key key;
      std::uint64_t degree = 0;
      bool mark = false;
      std::unique_ptr<Node> left{};
      Node *right = nullptr;
      Node *parent = nullptr;
      Node *child = nullptr;
    };
    Node *min = nullptr; //! owned by its own left member
    std::uint64_t n = 0;
  };
} // namespace chimera::library::virtual_machine
