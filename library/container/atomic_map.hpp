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

//! template atomic wrapper for std::map

#pragma once

#include <map>     // for map
#include <utility> // for forward

#include "container/atomic_container.hpp" // for AtomicContainer

namespace chimera::library::container {
  void atomic_map();
  template <typename Key, typename Value>
  struct AtomicMap : AtomicContainer<std::map<Key, Value>> {
    using Container = AtomicContainer<std::map<Key, Value>>;
    using Container::Container;
    using Container::read;
    using Container::write;
    template <typename... Args>
    [[nodiscard]] auto at(Args &&...args) const -> auto & {
      return read().value.at(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto at(Args &&...args) -> auto & {
      return read().value.at(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto cbegin(Args &&...args) const {
      return read().value.cbegin(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto cend(Args &&...args) const {
      return read().value.cend(std::forward<Args>(args)...);
    }
    template <typename... Args>
    [[nodiscard]] auto contains(Args &&...args) const {
      return Container::read().value.contains(std::forward<Args>(args)...);
    }
    template <typename... Args>
    [[nodiscard]] auto count(Args &&...args) const {
      return read().value.count(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto size(Args &&...args) const {
      return read().value.size(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto operator[](Args &&...args) -> auto & {
      return write().value.operator[](std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto erase(Args &&...args) {
      return write().value.erase(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto insert_or_assign(Args &&...args) {
      return write().value.insert_or_assign(std::forward<Args>(args)...);
    }
    template <typename... Args>
    auto try_emplace(Args &&...args) {
      return write().value.try_emplace(std::forward<Args>(args)...);
    }
  };
} // namespace chimera::library::container
