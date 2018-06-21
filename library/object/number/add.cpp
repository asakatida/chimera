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

#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"
#include "object/number/positive.hpp"
#include "object/number/sub.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename T>
        T copy(const T &t) {
          return t;
        }

        struct Plus {
          template <typename Left>
          Rational operator()(const Left &left, const Rational &right) {
            return std::visit(
                [&left](const auto &rN, const auto &rD) {
                  return Rational(rN + left * rD, copy(rD));
                },
                right.numerator, right.denominator);
          }

          template <typename Right>
          Rational operator()(const Rational &left, const Right &right) {
            return std::visit(
                [&right](const auto &lN, const auto &lD) {
                  return Rational(lN + right * lD, copy(lD));
                },
                left.numerator, left.denominator);
          }
        };

        Positive operator+(std::uint64_t left, Base right) {
          return right + left;
        }

        Natural operator+(std::uint64_t left, const Natural &right) {
          return right + left;
        }

        Positive operator+(std::uint64_t left, const Positive &right) {
          return right + left;
        }

        Integer operator+(std::uint64_t left, const Negative &right) {
          return right + left;
        }

        Integer operator+(std::uint64_t left, const Integer &right) {
          return right + left;
        }

        Rational operator+(std::uint64_t left, const Rational &right) {
          return right + left;
        }

        Real operator+(std::uint64_t left, const Real &right) {
          return right + left;
        }

        Positive operator+(Base left, std::uint64_t right) {
          auto value = sum(left.value, right);
          if (value.overflow == 0) {
            return Positive(Base{value.result});
          }
          return Positive(Natural{{value.result, value.overflow}});
        }

        Positive operator+(Base left, Base right) { return left + right.value; }

        Natural operator+(Base left, const Natural &right) {
          return right + left;
        }

        Positive operator+(Base left, const Positive &right) {
          return right + left;
        }

        Integer operator+(Base left, const Negative &right) {
          return right + left;
        }

        Integer operator+(Base left, const Integer &right) {
          return right + left;
        }

        Rational operator+(Base left, const Rational &right) {
          return right + left;
        }

        Real operator+(Base left, const Real &right) { return right + left; }

        Natural operator+(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return left;
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
          return value;
        }

        Natural operator+(const Natural &left, Base right) {
          return left + right.value;
        }

        Natural operator+(const Natural &left, const Natural &right) {
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
          return value;
        }

        Natural operator+(const Natural &left, const Positive &right) {
          return right + left;
        }

        Integer operator+(const Natural &left, const Negative &right) {
          return right + left;
        }

        Integer operator+(const Natural &left, const Integer &right) {
          return right + left;
        }

        Rational operator+(const Natural &left, const Rational &right) {
          return right + left;
        }

        Real operator+(const Natural &left, const Real &right) {
          return right + left;
        }

        Positive operator+(const Positive &left, std::uint64_t right) {
          return std::visit(
              RightOperation<Positive, std::uint64_t, std::plus<>>{right},
              left.value);
        }

        Positive operator+(const Positive &left, Base right) {
          return left + right.value;
        }

        Natural operator+(const Positive &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Natural(value + right); },
              left.value);
        }

        Positive operator+(const Positive &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Positive(l + r); },
              left.value, right.value);
        }

        Integer operator+(const Positive &left, const Negative &right) {
          return right + left;
        }

        Integer operator+(const Positive &left, const Integer &right) {
          return right + left;
        }

        Rational operator+(const Positive &left, const Rational &right) {
          return right + left;
        }

        Real operator+(const Positive &left, const Real &right) {
          return right + left;
        }

        Integer operator+(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Integer(right - value); },
              left.value);
        }

        Integer operator+(const Negative &left, Base right) {
          return left + right.value;
        }

        Integer operator+(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(right - value); },
              left.value);
        }

        Integer operator+(const Negative &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(right - value); },
              left.value);
        }

        Negative operator+(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return -(l + r); }, left.value,
              right.value);
        }

        Integer operator+(const Negative &left, const Integer &right) {
          return right + left;
        }

        Rational operator+(const Negative &left, const Rational &right) {
          return right + left;
        }

        Real operator+(const Negative &left, const Real &right) {
          return right + left;
        }

        Integer operator+(const Integer &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Integer(value + right); },
              left.value);
        }

        Integer operator+(const Integer &left, Base right) {
          return left + right.value;
        }

        Integer operator+(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value + right); },
              left.value);
        }

        Integer operator+(const Integer &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value + right); },
              left.value);
        }

        Integer operator+(const Integer &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value + right); },
              left.value);
        }

        Integer operator+(const Integer &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Integer(l + r); },
              left.value, right.value);
        }

        Rational operator+(const Integer &left, const Rational &right) {
          return right + left;
        }

        Real operator+(const Integer &left, const Real &right) {
          return right + left;
        }

        Rational operator+(const Rational &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &lN, const auto &lD) {
                return Rational(lN + right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator+(const Rational &left, Base right) {
          return left + right.value;
        }

        Rational operator+(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN + right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator+(const Rational &left, const Positive &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN + right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator+(const Rational &left, const Negative &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN + right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator+(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN + right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Real operator+(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return Real(lN * rD + rN * lD, lD * rD); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        Real operator+(const Rational &left, const Real &right) {
          return right + left;
        }

        Real operator+(const Real &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Real(value + right); },
              left.value);
        }

        Real operator+(const Real &left, Base right) {
          return left + right.value;
        }

        Real operator+(const Real &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value + right); },
              left.value);
        }

        Real operator+(const Real &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l + r); },
              left.value, right.value);
        }

        Real operator+(const Real &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l - r); },
              left.value, right.value);
        }

        Real operator+(const Real &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l + r); },
              left.value, right.value);
        }

        Real operator+(const Real &left, const Rational &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value + right); },
              left.value);
        }

        Real operator+(const Real &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l + r); },
              left.value, right.value);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
