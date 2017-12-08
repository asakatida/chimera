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

#include "object/number/and.hpp"

#include <limits>

#include <gsl/gsl>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator&(const std::uint64_t &left, const Base &right) {
          return Number{Base{left & right.value}};
        }

        Number operator&(const std::uint64_t &left, const Natural &right) {
          return Number{Base{left & right.value[0]}};
        }

        Number operator&(const std::uint64_t &left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left & value; },
                            right.value);
        }

        Number operator&(const std::uint64_t & /*left*/,
                         const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Base &left, const std::uint64_t &right) {
          return Number{Base{left.value & right}};
        }

        Number operator&(const Base &left, const Base &right) {
          return Number{Base{left.value & right.value}};
        }

        Number operator&(const Base &left, const Natural &right) {
          return Number{Base{left.value & right.value[0]}};
        }

        Number operator&(const Base &left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left & value; },
                            right.value);
        }

        Number operator&(const Base & /*left*/, const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Natural &left, const std::uint64_t &right) {
          return Number{Base{left.value[0] & right}};
        }

        Number operator&(const Natural &left, const Base &right) {
          return Number{Base{left.value[0] & right.value}};
        }

        Number operator&(const Natural &left, const Natural &right) {
          auto value = left;
          value.value.resize(std::min(value.value.size(), right.value.size()));
          std::transform(value.value.begin(), value.value.end(),
                         right.value.begin(), value.value.begin(),
                         [](const auto &a, const auto &b) { return a & b; });
          while (value.value.back() == 0) {
            value.value.pop_back();
            if (value.value.empty()) {
              return Number{};
            }
          }
          if (value.value.size() == 1) {
            return Number{Base{value.value[0]}};
          }
          value.value.shrink_to_fit();
          return Number{value};
        }

        Number operator&(const Natural &left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left & value; },
                            right.value);
        }

        Number operator&(const Natural & /*left*/, const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Integer &left, const std::uint64_t &right) {
          return std::visit(
              [&right](const auto &value) { return value & right; },
              left.value);
        }

        Number operator&(const Integer &left, const Base &right) {
          return std::visit(
              [&right](const auto &value) { return value & right; },
              left.value);
        }

        Number operator&(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return value & right; },
              left.value);
        }

        Number operator&(const Integer &left, const Integer &right) {
          return -std::visit([](const auto &a, const auto &b) { return a & b; },
                             left.value, right.value);
        }

        Number operator&(const Integer & /*left*/, const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Rational & /*left*/,
                         const std::uint64_t & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Rational & /*left*/, const Base & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Rational & /*left*/, const Natural & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Rational & /*left*/, const Integer & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Rational & /*left*/,
                         const Rational & /*right*/) {
          Ensures(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
