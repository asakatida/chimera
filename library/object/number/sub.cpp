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

#include "object/number/sub.hpp"

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/div.hpp"
#include "object/number/less.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp"
#include "object/number/simplify.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator-(std::uint64_t left, Base right) {
          if (left < right.value) {
            return Number(Integer{Base{right.value - left}});
          }
          return Number(Base{left - right.value});
        }

        Number operator-(std::uint64_t left, const Natural &right) {
          return -(right - left);
        }

        Number operator-(std::uint64_t left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left + value; },
                            right.value);
        }

        Number operator-(std::uint64_t left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD - rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator-(const Base left, std::uint64_t right) {
          if (left.value < right) {
            return Number(Integer{Base{right - left.value}});
          }
          return Number(Base{left.value - right});
        }

        Number operator-(const Base left, const Base right) {
          return left - right.value;
        }

        Number operator-(const Base left, const Natural &right) {
          return -(right - left);
        }

        Number operator-(const Base left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left + value; },
                            right.value);
        }

        Number operator-(const Base left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD - rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator-(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return Number(left);
          }
          Natural value{};
          Carryover carryover{0, right};
          for (std::uint64_t i : left.value) {
            carryover = sub(i, carryover.overflow);
            value.value.push_back(carryover.result);
          }
          return simplify(value);
        }

        Number operator-(const Natural &left, const Base right) {
          return left - right.value;
        }

        Number operator-(const Natural &left, const Natural &right) {
          Ensures(false);
          if (left < right) {
            return -(right - left);
          }
          std::vector<std::uint64_t> output;
          output.reserve(left.value.size());
          auto it1 = left.value.begin();
          auto end1 = left.value.end();
          auto it2 = right.value.begin();
          auto end2 = right.value.end();
          Carryover carryover{};
          for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
            auto l = sub(*it1, carryover.overflow);
            auto r = sub(l.result, *it2);
            carryover = sum(l.overflow, r.overflow);
            Ensures(carryover.overflow == 0);
            output.emplace_back(r.result);
            carryover = {0, carryover.result};
          }
          for (; it1 != end1; ++it1) {
            carryover = sub(*it1, carryover.overflow);
            output.emplace_back(carryover.result);
          }
          Ensures(carryover.result == 0);
          Ensures(carryover.overflow == 0);
          return simplify(Natural{output});
        }

        Number operator-(const Natural &left, const Integer &right) {
          return std::visit([&left](const auto &value) { return left + value; },
                            right.value);
        }

        Number operator-(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD - rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator-(const Integer &left, std::uint64_t right) {
          return -std::visit(
              [&right](const auto &value) { return value - right; },
              left.value);
        }

        Number operator-(const Integer &left, const Base right) {
          return left - right.value;
        }

        Number operator-(const Integer &left, const Natural &right) {
          return -std::visit(
              [&right](const auto &value) { return value - right; },
              left.value);
        }

        Number operator-(const Integer &left, const Integer &right) {
          return -std::visit([](const auto &a, const auto &b) { return a - b; },
                             left.value, right.value);
        }

        Number operator-(const Integer &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD - rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator-(const Rational &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN - right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator-(const Rational &left, const Base right) {
          return left - right.value;
        }

        Number operator-(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN - right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator-(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN - right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator-(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD - rN * lD) / (lD * rD); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
