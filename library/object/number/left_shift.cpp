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

#include "object/number/left_shift.hpp"

#include <gsl/gsl>

#include "object/number/compare.hpp"
#include "object/number/div.hpp"
#include "object/number/mod.hpp"
#include "object/number/overflow.hpp"
#include "object/number/right_shift.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator<<(std::uint64_t left, const Base right) {
          if (right.value <= 64) {
            auto value = left_shift(left, right.value);
            if (value.overflow == 0) {
              return Number(Base{value.result});
            }
            return Number(Natural{
                std::vector<std::uint64_t>{value.result, value.overflow}});
          }
          return Natural{{left}} << right;
        }

        Number operator<<(std::uint64_t left, const Natural &right) {
          return Natural{{left}} << right;
        }

        Number operator<<(std::uint64_t left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return left >> value; },
              right.value);
        }

        Number operator<<(std::uint64_t /*left*/, const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Base left, std::uint64_t right) {
          if (right <= 64) {
            auto value = left_shift(left.value, right);
            if (value.overflow == 0) {
              return Number(Base{value.result});
            }
            return Number(Natural{
                std::vector<std::uint64_t>{value.result, value.overflow}});
          }
          return Natural{{left.value}} << right;
        }

        Number operator<<(const Base left, const Base right) {
          return left << right.value;
        }

        Number operator<<(const Base /*left*/, const Natural & /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Base left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return left >> value; },
              right.value);
        }

        Number operator<<(const Base /*left*/, const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return Number(left);
          }
          auto value = left;
          if (std::size_t shift = right / 64; shift != 0) {
            value.value.insert(value.value.begin(), shift, 0);
          }
          if (std::uint64_t shift = right % 64; shift != 0) {
            Natural result;
            std::uint64_t overflow = 0;
            for (std::uint64_t i : value.value) {
              auto carryover = left_shift(i, shift);
              result.value.push_back(carryover.result | overflow);
              overflow = carryover.overflow;
            }
          }
          return Number(value);
        }

        Number operator<<(const Natural &left, const Base right) {
          return left << right.value;
        }

        Number operator<<(const Natural &left, const Natural &right) {
          auto value = left;
          if (auto shift = std::size_t(right / 64); shift != 0) {
            value.value.insert(value.value.begin(), shift, 0);
          }
          if (auto shift = std::uint64_t(right % 64); shift != 0) {
            Natural result;
            std::uint64_t overflow = 0;
            for (std::uint64_t i : value.value) {
              auto carryover = left_shift(i, shift);
              result.value.push_back(carryover.result | overflow);
              overflow = carryover.overflow;
            }
          }
          return Number(value);
        }

        Number operator<<(const Natural &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return left >> value; },
              right.value);
        }

        Number operator<<(const Natural & /*left*/,
                          const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Integer &left, std::uint64_t right) {
          return -std::visit(
              [&right](const auto &value) { return value << right; },
              left.value);
        }

        Number operator<<(const Integer &left, const Base right) {
          return left << right.value;
        }

        Number operator<<(const Integer &left, const Natural &right) {
          return -std::visit(
              [&right](const auto &value) { return value << right; },
              left.value);
        }

        Number operator<<(const Integer &left, const Integer &right) {
          return -std::visit(
              [](const auto &a, const auto &b) { return a >> b; }, left.value,
              right.value);
        }

        Number operator<<(const Integer & /*left*/,
                          const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Rational & /*left*/, std::uint64_t /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Rational & /*left*/, const Base /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Rational & /*left*/,
                          const Natural & /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Rational & /*left*/,
                          const Integer & /*right*/) {
          Ensures(false);
        }

        Number operator<<(const Rational & /*left*/,
                          const Rational & /*right*/) {
          Ensures(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
