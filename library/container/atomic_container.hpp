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

//! meta template to construct an atomic wrapper for a container

#pragma once

#include <memory>
#include <mutex>
#include <shared_mutex>

namespace chimera::library::container {
  void atomic_container();
  template <typename Value>
  struct AtomicContainer {
    AtomicContainer() noexcept : mutex(std::make_unique<std::shared_mutex>()) {}
    template <typename... Args>
    explicit AtomicContainer(Args &&...args)
        : mutex(std::make_unique<std::shared_mutex>()),
          value(std::forward<Args>(args)...) {}
    AtomicContainer(const AtomicContainer &other)
        : mutex(std::make_unique<std::shared_mutex>()),
          value(other.read().value) {}
    AtomicContainer(AtomicContainer &&other) noexcept
        : mutex(std::make_unique<std::shared_mutex>()),
          value(std::move(other.write().value)) {}
    ~AtomicContainer() noexcept = default;
    auto operator=(const AtomicContainer &other) -> AtomicContainer & {
      if (this != &other) {
        write().value = other.read().value;
      }
      return *this;
    }
    auto operator=(AtomicContainer &&other) noexcept -> AtomicContainer & {
      if (this != &other) {
        write().value = std::move(other.write().value);
      }
      return *this;
    }
    struct Read {
      const std::shared_lock<std::shared_mutex> lock;
      const Value &value;
    };
    struct Write {
      const std::unique_lock<std::shared_mutex> lock;
      Value &value;
    };
    [[nodiscard]] auto read() const -> Read {
      return Read{std::shared_lock<std::shared_mutex>(*mutex), value};
    }
    [[nodiscard]] auto write() -> Write {
      return Write{std::unique_lock<std::shared_mutex>(*mutex), value};
    }

  private:
    std::unique_ptr<std::shared_mutex> mutex;
    Value value;
  };
} // namespace chimera::library::container
