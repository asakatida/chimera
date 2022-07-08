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

#include "object/number/base.hpp"

namespace chimera::library::object::number {
  Base::Base() : value(0) {}
  Base::Base(std::uint64_t i) : value(i) {}
  Base::Base(const Base &other) = default;
  Base::Base(Base &&other) noexcept { swap(std::move(other)); }
  Base::~Base() = default;
  auto Base::operator=(const Base &other) -> Base & {
    if (this != &other) {
      value = other.value;
    }
    return *this;
  }
  auto Base::operator=(Base &&other) noexcept -> Base & {
    swap(std::move(other));
    return *this;
  }
  void Base::swap(Base &&other) noexcept {
    using std::swap;
    swap(value, other.value);
  }
} // namespace chimera::library::object::number
