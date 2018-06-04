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

#include "object/number/add.hpp"

#include <gsl/gsl>

#include "object/number/div.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/simplify.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator+(std::uint64_t left, const Base right) {
          return right + left;
        }

        Number operator+(std::uint64_t left, const Natural &right) {
          return right + left;
        }

        Number operator+(std::uint64_t left, const Integer &right) {
          return right + left;
        }

        Number operator+(std::uint64_t left, const Rational &right) {
          return right + left;
        }

        Number operator+(const Base left, std::uint64_t right) {
          auto value = sum(left.value, right);
          if (value.overflow == 0) {
            return Number(Base{value.result});
          }
          return Number(Natural{
              std::vector<std::uint64_t>{value.result, value.overflow}});
        }

        Number operator+(const Base left, const Base right) {
          return left + right.value;
        }

        Number operator+(const Base left, const Natural &right) {
          return right + left;
        }

        Number operator+(const Base left, const Integer &right) {
          return right + left;
        }

        Number operator+(const Base left, const Rational &right) {
          return right + left;
        }

        Number operator+(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return Number(left);
          }
          Natural value;
          value.value.reserve(left.value.size());
          Carryover carryover{0, right};
          for (std::uint64_t i : left.value) {
            carryover = sum(i, carryover.overflow);
            value.value.push_back(carryover.result);
          }
          if (carryover.overflow != 0) {
            value.value.push_back(carryover.overflow);
          }
          return simplify(value);
        }

        Number operator+(const Natural &left, const Base right) {
          return left + right.value;
        }

        Number operator+(const Natural &left, const Natural &right) {
          Natural value;
          value.value.reserve(std::max(left.value.size(), right.value.size()));
          auto it1 = left.value.begin();
          auto end1 = left.value.end();
          auto it2 = right.value.begin();
          auto end2 = right.value.end();
          Carryover carryover{};
          for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
            auto next = sum(*it1, *it2);
            carryover = sum(carryover.result, next.result);
            value.value.push_back(carryover.result);
            carryover = sum(carryover.overflow, next.overflow);
            Ensures(carryover.overflow == 0);
          }
          if (it1 == end1) {
            it1 = it2;
            end1 = end2;
          }
          carryover.overflow = carryover.result;
          for (; it1 != end1; ++it1) {
            carryover = sum(carryover.overflow, *it1);
            value.value.push_back(carryover.result);
          }
          if (carryover.overflow != 0) {
            value.value.push_back(carryover.overflow);
          }
          return simplify(value);
        }

        Number operator+(const Natural &left, const Integer &right) {
          return right + left;
        }

        Number operator+(const Natural &left, const Rational &right) {
          return right + left;
        }

        Number operator+(const Integer &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &value) { return right - value; },
              left.value);
        }

        Number operator+(const Integer &left, const Base right) {
          return std::visit(
              [&right](const auto &value) { return right - value; },
              left.value);
        }

        Number operator+(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return right - value; },
              left.value);
        }

        Number operator+(const Integer &left, const Integer &right) {
          return -std::visit([](const auto &l, const auto &r) { return l + r; },
                             left.value, right.value);
        }

        Number operator+(const Integer &left, const Rational &right) {
          return right + left;
        }

        Number operator+(const Rational &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return (lN + right * lD) / lD;
              },
              left.numerator, left.denominator);
        }

        Number operator+(const Rational &left, const Base right) {
          return left + right.value;
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
