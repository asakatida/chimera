// Copyright (c) 2017 Adam Grandquist <grandquista@gmail.com>
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

#include <shared_mutex>

namespace chimera {
  namespace library {
    namespace container {
      template <typename Value>
      struct AtomicContainer {
        AtomicContainer() noexcept {}

        explicit AtomicContainer(const Value &v) : value(v) {}

        explicit AtomicContainer(Value &&v) noexcept : value(std::move(v)) {}

        AtomicContainer(const AtomicContainer &other) {
          value = other.read().value;
        }

        AtomicContainer(AtomicContainer &&other) noexcept {
          value = std::move(other.write().value);
        }

        ~AtomicContainer() noexcept = default;

        AtomicContainer &operator=(const AtomicContainer &other) {
          if (this != &other) {
            write().value = other.read().value;
          }
          return *this;
        }

        AtomicContainer &operator=(AtomicContainer &&other) noexcept {
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

        Read read() const {
          return Read{std::shared_lock<std::shared_mutex>(
                          const_cast<std::shared_mutex &>(mutex)),
                      value};
        }

        Read read() {
          return Read{std::shared_lock<std::shared_mutex>(mutex), value};
        }

        Write write() {
          return Write{std::unique_lock<std::shared_mutex>(mutex), value};
        }

      private:
        std::shared_mutex mutex{};
        Value value;
      };
    } // namespace container
  }   // namespace library
} // namespace chimera
