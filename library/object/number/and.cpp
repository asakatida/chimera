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

#include <gsl/gsl>

namespace chimera {
  namespace library {
    namespace object {

      namespace number {
        Number operator&(const std::uint64_t &left, const Base &right) {
          return Number{Base{left & right.value}};
        }

        Number operator&(const std::uint64_t & /*left*/,
                         const Natural & /*right*/) {
          return Number{};
        }

        Number operator&(const std::uint64_t & /*left*/,
                         const Integer & /*right*/) {
          return Number{};
        }

        Number operator&(const std::uint64_t & /*left*/,
                         const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Base & /*left*/,
                         const std::uint64_t & /*right*/) {
          return Number{};
        }

        Number operator&(const Base &left, const Base &right) {
          return Number{Base{left.value & right.value}};
        }

        Number operator&(const Base & /*left*/, const Natural & /*right*/) {
          return Number{};
        }

        Number operator&(const Base & /*left*/, const Integer & /*right*/) {
          return Number{};
        }

        Number operator&(const Base & /*left*/, const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Natural & /*left*/,
                         const std::uint64_t & /*right*/) {
          return Number{};
        }

        Number operator&(const Natural & /*left*/, const Base & /*right*/) {
          return Number{};
        }

        Number operator&(const Natural &left, const Natural &right) {
          std::vector<std::uint64_t> output;
          output.reserve(std::max(left.value.size(), right.value.size()));
          auto it1 = left.value.begin();
          auto end1 = left.value.end();
          auto it2 = right.value.begin();
          auto end2 = right.value.end();
          for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
            output.emplace_back(*it1 & *it2);
          }
          bool bitMask = right.bit;
          if (it1 == end1) {
            it1 = it2;
            end1 = end2;
            bitMask = left.bit;
          }
          if (bitMask) {
            output.insert(output.end(), it1, end1);
          } else {
            while ((!output.empty()) && output.back() == std::uint64_t(0)) {
              output.pop_back();
            }
            output.shrink_to_fit();
          }
          return Number{Natural{output, left.bit && right.bit}};
        }

        Number operator&(const Natural & /*left*/, const Integer & /*right*/) {
          return Number{};
        }

        Number operator&(const Natural & /*left*/, const Rational & /*right*/) {
          Ensures(false);
        }

        Number operator&(const Integer & /*left*/,
                         const std::uint64_t & /*right*/) {
          return Number{};
        }

        Number operator&(const Integer & /*left*/, const Base & /*right*/) {
          return Number{};
        }

        Number operator&(const Integer & /*left*/, const Natural & /*right*/) {
          return Number{};
        }

        Number operator&(const Integer &left, const Integer &right) {
          if (left.sign && right.sign) {
            return -std::visit([](auto &&a, auto &&b) { return a & b; },
                               left.value, right.value);
          }
          return std::visit([](auto &&a, auto &&b) { return a & b; },
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
