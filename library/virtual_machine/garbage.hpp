//! collects objects that are not referenced outside itself

#pragma once

#include "object/object.hpp"
#include "virtual_machine/fibonacci_heap.hpp"

#include <gsl/gsl>

#include <atomic>
#include <mutex>
#include <thread>

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
    [[nodiscard]] auto emplace(Args &&...args) {
      const std::lock_guard<std::mutex> lock(mutex);
      return fibonacciHeap.emplace(std::forward<Args>(args)...);
    }

  private:
    void collect();
    struct Compare {
      [[nodiscard]] auto operator()(const object::Object &left,
                                    const object::Object &right) -> bool {
        return left.id() < right.id();
      }
    };
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    std::thread thread;
    std::mutex mutex{};
    FibonacciHeap<object::Object, Compare> fibonacciHeap{};
  };
} // namespace chimera::library::virtual_machine
