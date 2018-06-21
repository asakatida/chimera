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

#include "object/number/sub.hpp"

#include <gsl/gsl>

#include "object/number/add.hpp"
#include "object/number/less.hpp"
#include "object/number/mult.hpp"
#include "object/number/negative.hpp"
#include "object/number/overflow.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename T>
        T copy(const T &t) {
          return t;
        }

        Integer operator-(std::uint64_t left, Base right) {
          if (left < right.value) {
            return Integer(Negative(Base{right.value - left}));
          }
          return Integer(Base{left - right.value});
        }

        Negative operator-(std::uint64_t left, const Natural &right) {
          return -(right - left);
        }

        Integer operator-(std::uint64_t left, const Positive &right) {
          return std::visit(
              LeftOperation<Integer, std::uint64_t, std::minus<>>{left},
              right.value);
        }

        Positive operator-(std::uint64_t left, const Negative &right) {
          return std::visit(
              LeftOperation<Positive, std::uint64_t, std::plus<>>{left},
              right.value);
        }

        Integer operator-(std::uint64_t left, const Integer &right) {
          return std::visit(
              LeftOperation<Integer, std::uint64_t, std::minus<>>{left},
              right.value);
        }

        Rational operator-(std::uint64_t left, const Rational &right) {
          return std::visit(
              [left](const auto &rN, const auto &rD) {
                return Rational(left * rD - rN, copy(rD));
              },
              right.numerator, right.denominator);
        }

        Real operator-(std::uint64_t left, const Real &right) {
          return std::visit(
              [left](const auto &value) { return Real(left - value); },
              right.value);
        }

        Integer operator-(Base left, std::uint64_t right) {
          if (left.value < right) {
            return Integer(Negative(Base{right - left.value}));
          }
          return Integer(Base{left.value - right});
        }

        Integer operator-(Base left, Base right) { return left - right.value; }

        Negative operator-(Base left, const Natural &right) {
          return -(right - left);
        }

        Integer operator-(Base left, const Positive &right) {
          return std::visit(
              [left](const auto &value) { return Integer(left - value); },
              right.value);
        }

        Positive operator-(Base left, const Negative &right) {
          return std::visit(
              [left](const auto &value) { return Positive(left + value); },
              right.value);
        }

        Integer operator-(Base left, const Integer &right) {
          return std::visit(
              [left](const auto &value) { return Integer(left - value); },
              right.value);
        }

        Rational operator-(Base left, const Rational &right) {
          return std::visit(
              [left](const auto &rN, const auto &rD) {
                return Rational(left * rD - rN, copy(rD));
              },
              right.numerator, right.denominator);
        }

        Real operator-(Base left, const Real &right) {
          return std::visit(
              [left](const auto &value) { return Real(left - value); },
              right.value);
        }

        Positive operator-(const Natural &left, std::uint64_t right) {
          if (right == 0) {
            return Positive(copy(left));
          }
          Natural value{};
          Carryover carryover{0, right};
          for (std::uint64_t i : left.value) {
            carryover = sub(i, carryover.overflow);
            value.value.push_back(carryover.result);
          }
          return Positive(std::move(value));
        }

        Positive operator-(const Natural &left, Base right) {
          return left - right.value;
        }

        Integer operator-(const Natural &left, const Natural &right) {
          if (left < right) {
            return -(right - left);
          }
          Natural value{};
          value.value.reserve(left.value.size());
          auto it1 = left.value.begin();
          auto end1 = left.value.end();
          auto it2 = right.value.begin();
          auto end2 = right.value.end();
          Carryover carryover{};
          for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
            auto next = sub(*it1, *it2);
            carryover = sub(next.result, carryover.result);
            value.value.push_back(carryover.result);
            carryover = sum(carryover.overflow, next.overflow);
            Ensures(carryover.overflow == 0);
          }
          carryover.overflow = carryover.result;
          for (; it1 != end1; ++it1) {
            carryover = sub(*it1, carryover.overflow);
            value.value.push_back(carryover.result);
          }
          Ensures(carryover.overflow == 0);
          return Integer(std::move(value));
        }

        Integer operator-(const Natural &left, const Positive &right) {
          return std::visit(
              [&left](const auto &value) { return Integer(left - value); },
              right.value);
        }

        Natural operator-(const Natural &left, const Negative &right) {
          return std::visit([&left](const auto &value) { return left + value; },
                            right.value);
        }

        Integer operator-(const Natural &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return Integer(left - value); },
              right.value);
        }

        Rational operator-(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return Rational(left * rD - rN, copy(rD));
              },
              right.numerator, right.denominator);
        }

        Real operator-(const Natural &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left - value); },
              right.value);
        }

        Integer operator-(const Positive &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Integer operator-(const Positive &left, Base right) {
          return left - right.value;
        }

        Integer operator-(const Positive &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Integer operator-(const Positive &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Positive operator-(const Positive &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value - right); },
              left.value);
        }

        Integer operator-(const Positive &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Rational operator-(const Positive &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return Rational(left * rD - rN, copy(rD));
              },
              right.numerator, right.denominator);
        }

        Real operator-(const Positive &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left - value); },
              right.value);
        }

        Negative operator-(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -(value + right); },
              left.value);
        }

        Negative operator-(const Negative &left, Base right) {
          return left - right.value;
        }

        Negative operator-(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -(value + right); },
              left.value);
        }

        Negative operator-(const Negative &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return -(l + r); }, left.value,
              right.value);
        }

        Integer operator-(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Integer(-(l + r)); },
              left.value, right.value);
        }

        Integer operator-(const Negative &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Integer(r - l); },
              left.value, right.value);
        }

        Rational operator-(const Negative &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return Rational(left * rD - rN, copy(rD));
              },
              right.numerator, right.denominator);
        }

        Real operator-(const Negative &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(r - l); },
              left.value, right.value);
        }

        Integer operator-(const Integer &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Integer operator-(const Integer &left, Base right) {
          return left - right.value;
        }

        Integer operator-(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Integer operator-(const Integer &left, const Positive &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Integer operator-(const Integer &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Integer operator-(const Integer &left, const Integer &right) {
          return std::visit(
              [&right](const auto &value) { return Integer(value - right); },
              left.value);
        }

        Rational operator-(const Integer &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return Rational(left * rD - rN, copy(rD));
              },
              right.numerator, right.denominator);
        }

        Real operator-(const Integer &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left - value); },
              right.value);
        }

        Rational operator-(const Rational &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &lN, const auto &lD) {
                return Rational(lN - right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator-(const Rational &left, Base right) {
          return left - right.value;
        }

        Rational operator-(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN - right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator-(const Rational &left, const Positive &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN - right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator-(const Rational &left, const Negative &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN - right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Rational operator-(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Rational(lN - right * lD, copy(lD));
              },
              left.numerator, left.denominator);
        }

        Real operator-(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return Real(lN * rD - rN * lD, lD * rD); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        Real operator-(const Rational &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) { return Real(left - value); },
              right.value);
        }

        Real operator-(const Real &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return Real(value - right); },
              left.value);
        }

        Real operator-(const Real &left, Base right) {
          return std::visit(
              [right](const auto &value) { return Real(value - right); },
              left.value);
        }

        Real operator-(const Real &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value - right); },
              left.value);
        }

        Real operator-(const Real &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l - r); },
              left.value, right.value);
        }

        Real operator-(const Real &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l + r); },
              left.value, right.value);
        }

        Real operator-(const Real &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l - r); },
              left.value, right.value);
        }

        Real operator-(const Real &left, const Rational &right) {
          return std::visit(
              [&right](const auto &value) { return Real(value - right); },
              left.value);
        }

        Real operator-(const Real &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return Real(l - r); },
              left.value, right.value);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
