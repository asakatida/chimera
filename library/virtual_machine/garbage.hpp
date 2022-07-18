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

//! collects objects that are not referenced outside itself

#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include <gsl/gsl>

#include "object/object.hpp"
#include "virtual_machine/fibonacci_heap.hpp"

namespace chimera::library::virtual_machine {
  struct GarbageCollector {
    GarbageCollector();
    GarbageCollector(const GarbageCollector &collector) = delete;
    GarbageCollector(GarbageCollector &&collector) = delete;
    ~GarbageCollector() noexcept;
    auto operator=(const GarbageCollector &collector)
        -> GarbageCollector & = delete;
    auto operator=(GarbageCollector &&collector) -> GarbageCollector & = delete;
    template <typename... Args>
    auto emplace(Args &&...args) {
      std::lock_guard<std::mutex> lock(mutex);
      return fibonacciHeap.emplace(std::forward<Args>(args)...);
    }

  private:
    void collect();
    struct Compare {
      auto operator()(const object::Object &a, const object::Object &b)
          -> bool {
        return a.id() < b.id();
      }
    };
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    std::thread thread;
    std::mutex mutex{};
    FibonacciHeap<object::Object, Compare> fibonacciHeap{};
  };
} // namespace chimera::library::virtual_machine
