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

#include "object/number/overflow.hpp"

#include <limits>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Carryover div(const Carryover &left, std::uint64_t right) {
          auto a = __uint128_t(left.overflow) << 64 & left.result;
          return {std::uint64_t(a / right), std::uint64_t(a % right)};
        }

        Carryover left_shift(std::uint64_t left, std::uint64_t right) {
          auto carryover = __uint128_t(left) << right;
          return {std::uint64_t(carryover), std::uint64_t(carryover >> 64)};
        }

        Carryover mult(std::uint64_t left, std::uint64_t right) {
          auto carryover = __uint128_t(left) * right;
          return {std::uint64_t(carryover), std::uint64_t(carryover >> 64)};
        }

        Carryover right_shift(std::uint64_t left, std::uint64_t right) {
          auto carryover = __uint128_t(left) << (64 - right);
          return {std::uint64_t(carryover >> 64), std::uint64_t(carryover)};
        }

        Carryover sub(std::uint64_t left, std::uint64_t right) {
          if (left < right) {
            return {std::numeric_limits<std::uint64_t>::max() - (right - left),
                    1};
          }
          return {left - right, 0};
        }

        Carryover sum(std::uint64_t left, std::uint64_t right) {
          auto carryover = __uint128_t(left) + right;
          return {std::uint64_t(carryover), std::uint64_t(carryover >> 64)};
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
};      // namespace chimera
