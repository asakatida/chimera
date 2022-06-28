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

#pragma once

#include <cstdint>

namespace chimera::library::object::number {
  struct Carryover {
    std::uint64_t result;
    std::uint64_t overflow;
  };

  auto div_mod(const Carryover &left, std::uint64_t right) -> Carryover;
  auto left_shift(std::uint64_t left, std::uint64_t right) -> Carryover;
  auto mult(std::uint64_t left, std::uint64_t right) -> Carryover;
  auto right_shift(std::uint64_t left, std::uint64_t right) -> Carryover;
  auto sub(std::uint64_t left, std::uint64_t right) -> Carryover;
  auto sum(std::uint64_t left, std::uint64_t right) -> Carryover;
} // namespace chimera::library::object::number
