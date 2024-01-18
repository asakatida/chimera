//! collects objects that are not referenced outside itself

#include "virtual_machine/garbage.hpp"

#include <atomic>
#include <mutex>
#include <thread>

namespace chimera::library::virtual_machine {
  GarbageCollector::GarbageCollector() : thread([this] { this->collect(); }) {}
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
        const std::lock_guard<std::mutex> lock(mutex);
        using std::swap;
        swap(moving, fibonacciHeap);
      }
      garbage.merge(std::move(moving));
      std::this_thread::yield();
    }
  }
} // namespace chimera::library::virtual_machine
