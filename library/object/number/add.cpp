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

#include "object/number/add.hpp"

#include <gsl/gsl> // for Ensures

#include "object/number/div.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp" // for Carryover
#include "object/number/positive.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator+(const std::uint64_t &left, const Base &right) {
          auto value = sum(left, right.value);
          if (value.overflow == 0) {
            return Number(Base{value.result});
          }
          return Number(Natural{
              std::vector<std::uint64_t>{value.result, value.overflow}});
        }

        Number operator+(const std::uint64_t &left, const Natural &right) {
          if (left == 0) {
            return Number(right);
          }
          auto value = right;
          Carryover carryover{0, left};
          for (auto &&i : value.value) {
            carryover = sum(i, carryover.overflow);
            i = carryover.result;
            if (carryover.overflow == 0) {
              return Number(value);
            }
          }
          value.value.push_back(carryover.overflow);
          return Number(value);
        }

        Number operator+(const std::uint64_t &left, const Integer &right) {
          return left - (+right);
        }

        Number operator+(const std::uint64_t &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD + rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator+(const Base &left, const std::uint64_t &right) {
          auto value = sum(left.value, right);
          if (value.overflow == 0) {
            return Number(Base{value.result});
          }
          return Number(Natural{
              std::vector<std::uint64_t>{value.result, value.overflow}});
        }

        Number operator+(const Base &left, const Base &right) {
          auto value = sum(left.value, right.value);
          if (value.overflow == 0) {
            return Number(Base{value.result});
          }
          return Number(Natural{
              std::vector<std::uint64_t>{value.result, value.overflow}});
        }

        Number operator+(const Base &left, const Natural &right) {
          if (left.value == 0) {
            return Number(right);
          }
          auto value = right;
          Carryover carryover{0, left.value};
          for (auto &&i : value.value) {
            carryover = sum(i, carryover.overflow);
            i = carryover.result;
            if (carryover.overflow == 0) {
              return Number(value);
            }
          }
          value.value.push_back(carryover.overflow);
          return Number(value);
        }

        Number operator+(const Base &left, const Integer &right) {
          return left - (+right);
        }

        Number operator+(const Base &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD + rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator+(const Natural &left, const std::uint64_t &right) {
          if (right == 0) {
            return Number(left);
          }
          auto value = left;
          Carryover carryover{0, right};
          for (auto &&i : value.value) {
            carryover = sum(i, carryover.overflow);
            i = carryover.result;
            if (carryover.overflow == 0) {
              return Number(value);
            }
          }
          value.value.push_back(carryover.overflow);
          return Number(value);
        }

        Number operator+(const Natural &left, const Base &right) {
          if (right.value == 0) {
            return Number(left);
          }
          auto value = left;
          Carryover carryover{0, right.value};
          for (auto &&i : value.value) {
            carryover = sum(i, carryover.overflow);
            i = carryover.result;
            if (carryover.overflow == 0) {
              return Number(value);
            }
          }
          value.value.push_back(carryover.overflow);
          return Number(value);
        }

        Number operator+(const Natural &left, const Natural &right) {
          std::vector<std::uint64_t> output;
          output.reserve(std::max(left.value.size(), right.value.size()) + 1);
          auto it1 = left.value.begin();
          auto end1 = left.value.end();
          auto it2 = right.value.begin();
          auto end2 = right.value.end();
          Carryover carryover{};
          for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
            auto l = sum(carryover.result, *it1);
            auto r = sum(l.result, *it2);
            carryover = sum(l.overflow, r.overflow);
            Ensures(carryover.overflow == 0);
            output.emplace_back(r.result);
          }
          if (it1 == end1) {
            it1 = it2;
            end1 = end2;
          }
          for (; it1 != end1; ++it1) {
            auto l = sum(carryover.result, *it1);
            output.emplace_back(l.result);
            carryover = {l.overflow, 0};
          }
          if (carryover.result != 0) {
            output.emplace_back(carryover.result);
          }
          return Number(Natural{output});
        }

        Number operator+(const Natural &left, const Integer &right) {
          return left - (+right);
        }

        Number operator+(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD + rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator+(const Integer &left, const std::uint64_t &right) {
          return right - (+left);
        }

        Number operator+(const Integer &left, const Base &right) {
          return right - (+left);
        }

        Number operator+(const Integer &left, const Natural &right) {
          return right - (+left);
        }

        Number operator+(const Integer &left, const Integer &right) {
          return -std::visit([](const auto &l, const auto &r) { return l + r; },
                             left.value, right.value);
        }

        Number operator+(const Integer &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD + rN) / rD;
              },
              right.numerator, right.denominator);
        }

        Number operator+(const Rational &left, const std::uint64_t &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN + right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator+(const Rational &left, const Base &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN + right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator+(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN + right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator+(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN + right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator+(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD + rN * lD) / (lD * rD); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
