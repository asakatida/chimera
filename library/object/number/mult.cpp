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
#include "object/number/overflow.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator*(const std::uint64_t &left, const Base &right) {
          auto value = mult(left, right.value);
          if (value.overflow == 0) {
            return Number(Base{value.result});
          }
          return Number(Natural{{value.result, value.overflow}});
        }

        Number operator*(const std::uint64_t &left, const Natural &right) {
          if (left == 0) {
            return Number();
          }
          if (left == 1) {
            return Number(right);
          }
          auto value = right;
          Carryover carryover{};
          for (auto &&i : value.value) {
            auto m = mult(i, left);
            carryover = sum(m.result, carryover.overflow);
            i = carryover.result;
            carryover = sum(m.overflow, carryover.overflow);
            Ensures(carryover.overflow == 0);
            carryover = {0, carryover.result};
          }
          if (carryover.overflow != 0) {
            value.value.push_back(carryover.overflow);
          }
          return Number(value);
        }

        Number operator*(const std::uint64_t &left, const Integer &right) {
          return -std::visit(
              [&left](const auto &value) { return left * value; }, right.value);
        }

        Number operator*(const std::uint64_t &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator*(const Base &left, const std::uint64_t &right) {
          auto value = mult(left.value, right);
          if (value.overflow == 0) {
            return Number(Base{value.result});
          }
          return Number(Natural{{value.result, value.overflow}});
        }

        Number operator*(const Base &left, const Base &right) {
          return left * right.value;
        }

        Number operator*(const Base &left, const Natural &right) {
          if (left.value == 0) {
            return Number();
          }
          if (left.value == 1) {
            return Number(right);
          }
          auto value = right;
          Carryover carryover{};
          for (auto &&i : value.value) {
            auto m = mult(i, left.value);
            carryover = sum(m.result, carryover.overflow);
            i = carryover.result;
            carryover = sum(m.overflow, carryover.overflow);
            Ensures(carryover.overflow == 0);
            carryover = {0, carryover.result};
          }
          if (carryover.overflow != 0) {
            value.value.push_back(carryover.overflow);
          }
          return Number(value);
        }

        Number operator*(const Base &left, const Integer &right) {
          return -std::visit(
              [&left](const auto &value) { return left * value; }, right.value);
        }

        Number operator*(const Base &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator*(const Natural &left, const std::uint64_t &right) {
          if (right == 0) {
            return Number();
          }
          if (right == 1) {
            return Number(left);
          }
          auto value = left;
          Carryover carryover{};
          for (auto &&i : value.value) {
            auto m = mult(i, right);
            carryover = sum(m.result, carryover.overflow);
            i = carryover.result;
            carryover = sum(m.overflow, carryover.overflow);
            Ensures(carryover.overflow == 0);
            carryover = {0, carryover.result};
          }
          if (carryover.overflow != 0) {
            value.value.push_back(carryover.overflow);
          }
          return Number(value);
        }

        Number operator*(const Natural &left, const Base &right) {
          return left * right.value;
        }

        Number operator*(const Natural &left, const Natural &right) {
          std::vector<Number> integers;
          for (const auto &i : left.value) {
            std::vector<std::uint64_t> output;
            output.reserve(right.value.size() + integers.size() + 1);
            output.resize(integers.size());
            Carryover carryover{};
            for (const auto &j : right.value) {
              auto result = mult(i, j);
              auto next = sum(result.result, carryover.overflow);
              carryover = sum(result.overflow, carryover.overflow);
              Ensures(carryover.overflow == 0);
              carryover = sum(next.overflow, carryover.result);
              Ensures(carryover.overflow == 0);
              carryover = {0, carryover.result};
              output.emplace_back(result.result);
            }
            if (carryover.overflow != 0) {
              output.emplace_back(carryover.overflow);
            }
            integers.emplace_back(Number(Natural{output}));
          }
          return std::accumulate(integers.begin(), integers.end(), Number());
        }

        Number operator*(const Natural &left, const Integer &right) {
          return -std::visit(
              [&left](const auto &value) { return left * value; }, right.value);
        }

        Number operator*(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator*(const Integer &left, const std::uint64_t &right) {
          return -std::visit(
              [&right](const auto &value) { return value * right; },
              left.value);
        }

        Number operator*(const Integer &left, const Base &right) {
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
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator*(const Rational &left, const std::uint64_t &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN * right) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator*(const Rational &left, const Base &right) {
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
