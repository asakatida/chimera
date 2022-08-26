// Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//! Fibonacci heap for use as a garbage collector storage

#pragma once

#include <cstdint>
#include <memory>
#include <mutex>

#include <gsl/gsl>

namespace chimera::library::virtual_machine {
  //! Fibonacci heap
  //! follows std::set/list for methods provided
  template <typename Key, typename Compare = std::less<Key>,
            typename Allocator = std::allocator<Key>>
  struct FibonacciHeap {
    FibonacciHeap() noexcept = default;
    template <typename... Args>
    explicit FibonacciHeap(Args &&...args) {
      auto x = std::make_unique<Node>(std::forward<Args>(args)...);
      min = x.get();
      min->set_left(std::move(x));
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
      auto x = std::make_unique<Node>(Key{std::forward<Args>(args)...});
      if (min == nullptr) {
        auto *ptr = x.get();
        ptr->set_left(std::move(x));
        min = ptr->get_left();
        ++n;
        return min->get();
      }
      if (*x < *min) {
        min = x->get_left();
        x->set_left(std::move(x));
        ++n;
        return min->get();
      }
      min->emplace(std::move(x));
      ++n;
      return min->get_left()->get();
    }
    template <typename Predicate>
    void remove_if(Predicate && /*predicate*/) {}
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
      auto operator<(const Node &node) -> bool {
        return key.id() < node.key.id();
      }
      void emplace(std::unique_ptr<Node> &&node) {
        node->set_left(std::move(left));
        set_left(std::move(node));
      }
      [[nodiscard]] auto get() const -> const Key & { return key; }
      [[nodiscard]] auto get_left() -> Node * { return left.get(); }
      void remove_left() { std::ignore = std::move(left); }
      void reset_left() {
        if (left) {
          auto ptr = std::move(left);
          ptr.reset();
        }
      }
      void set_left(std::unique_ptr<Node> &&node) {
        left = std::move(node);
        left->right = this;
      }

    private:
      Key key;
      std::uint64_t degree = 0;
      bool mark = false;
      std::unique_ptr<Node> left = nullptr;
      Node *right = nullptr;
      Node *parent = nullptr;
      Node *child = nullptr;
    };

  private:
    Node *min = nullptr; //! owned by its own left member
    std::uint64_t n = 0;
  };
} // namespace chimera::library::virtual_machine
