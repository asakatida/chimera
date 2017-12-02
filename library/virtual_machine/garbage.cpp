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

#include "virtual_machine/garbage.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

#include <gsl/gsl>

namespace chimera {
  namespace library {
    namespace virtual_machine {
      GarbageCollector::GarbageCollector()
          : thread([this] { this->collect(); }) {}

      GarbageCollector::~GarbageCollector() noexcept {
        if (thread.joinable()) {
          flag.clear();
          thread.join();
        }
      }

      void GarbageCollector::collect() {
        flag.test_and_set();
        FibonacciHeap<object::Object, Compare> garbage{};
        while (flag.test_and_set()) {
          garbage.remove_if([](auto &&node) { return node.use_count() <= 1; });
          FibonacciHeap<object::Object, Compare> moving{};
          {
            std::lock_guard<std::mutex> lock(mutex);
            using std::swap;
            swap(moving, fibonacciHeap);
          }
          garbage.merge(std::move(moving));
          std::this_thread::yield();
        }
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
