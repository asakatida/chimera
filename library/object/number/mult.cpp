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

#include "object/number/mult.hpp"

#include <gsl/gsl>

#include "object/number/div.hpp"
#include "object/number/left_shift.hpp"
#include "object/number/overflow.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator*(std::uint64_t left, const Base right) {
          return right * left;
        }

        Number operator*(std::uint64_t left, const Natural &right) {
          return right * left;
        }

        Number operator*(std::uint64_t left, const Integer &right) {
          return right * left;
        }

        Number operator*(std::uint64_t left, const Rational &right) {
          return right * left;
        }

        Number operator*(const Base left, std::uint64_t right) {
          auto value = mult(left.value, right);
          if (value.overflow == 0) {
            return Number(Base{value.result});
          }
          return Number(Natural{{value.result, value.overflow}});
        }

        Number operator*(const Base left, const Base right) {
          return left * right.value;
        }

        Number operator*(const Base left, const Natural &right) {
          return right * left;
        }

        Number operator*(const Base left, const Integer &right) {
          return right * left;
        }

        Number operator*(const Base left, const Rational &right) {
          return right * left;
        }

        Number operator*(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return Number();
          }
          if (right == 1) {
            return Number(left);
          }
          Natural value;
          value.value.reserve(left.value.size() + 1);
          Carryover carryover{};
          for (std::uint64_t i : left.value) {
            auto m = mult(i, right);
            carryover = sum(m.result, carryover.result);
            value.value.push_back(carryover.result);
            carryover = sum(m.overflow, carryover.overflow);
            Ensures(carryover.overflow == 0);
          }
          if (carryover.result != 0) {
            value.value.push_back(carryover.result);
          }
          return Number(value);
        }

        Number operator*(const Natural &left, const Base right) {
          return left * right.value;
        }

        Number operator*(const Natural &left, const Natural &right) {
          std::vector<Number> integers;
          integers.reserve(right.value.size());
          for (const auto &i : right.value) {
            integers.push_back((left * i) << (64 * integers.size()));
          }
          return std::accumulate(integers.begin(), integers.end(), Number());
        }

        Number operator*(const Natural &left, const Integer &right) {
          return right * left;
        }

        Number operator*(const Natural &left, const Rational &right) {
          return right * left;
        }

        Number operator*(const Integer &left, std::uint64_t right) {
          return -std::visit(
              [&right](const auto &value) { return value * right; },
              left.value);
        }

        Number operator*(const Integer &left, const Base right) {
          return left * right.value;
        }

        Number operator*(const Integer &left, const Natural &right) {
          return -std::visit(
              [&right](const auto &value) { return value * right; },
              left.value);
        }

        Number operator*(const Integer &left, const Integer &right) {
          return std::visit([](const auto &a, const auto &b) { return a * b; },
                            left.value, right.value);
        }

        Number operator*(const Integer &left, const Rational &right) {
          return right * left;
        }

        Number operator*(const Rational &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN * right) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator*(const Rational &left, const Base right) {
          return left * right.value;
        }

        Number operator*(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN * right) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator*(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN * right) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator*(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rN) / (lD * rD); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
