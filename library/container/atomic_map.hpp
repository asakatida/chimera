//! template atomic wrapper for std::map

#pragma once

#include "container/atomic_container.hpp" // for AtomicContainer

#include <map>     // for map
#include <utility> // for forward

namespace chimera::library::container {
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
    [[nodiscard]] auto at(Args &&...args) -> auto & {
      return read().value.at(std::forward<Args>(args)...);
    }
    template <typename... Args>
    [[nodiscard]] auto cbegin(Args &&...args) const {
      return read().value.cbegin(std::forward<Args>(args)...);
    }
    template <typename... Args>
    [[nodiscard]] auto cend(Args &&...args) const {
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
    [[nodiscard]] auto size(Args &&...args) const {
      return read().value.size(std::forward<Args>(args)...);
    }
    template <typename... Args>
    [[nodiscard]] auto operator[](Args &&...args) -> auto & {
      return write().value.operator[](std::forward<Args>(args)...);
    }
    template <typename... Args>
    void erase(Args &&...args) {
      write().value.erase(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void insert_or_assign(Args &&...args) {
      write().value.insert_or_assign(std::forward<Args>(args)...);
    }
    template <typename... Args>
    [[nodiscard]] auto try_emplace(Args &&...args) {
      return write().value.try_emplace(std::forward<Args>(args)...);
    }
  };
} // namespace chimera::library::container
