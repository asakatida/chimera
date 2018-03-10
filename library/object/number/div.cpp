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

#include "object/number/div.hpp"

#include "container/reverse.hpp"
#include "object/number/div.hpp"
#include "object/number/floor_div.hpp"
#include "object/number/mult.hpp"
#include "object/number/overflow.hpp" // for Carryover
#include "object/number/right_shift.hpp"
#include "object/number/sub.hpp"
#include "object/number/util.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        static bool even(const std::uint64_t &i) { return (i & 1u) == 0u; }
        static bool even(const Base &i) { return even(i.value); }
        static bool even(const Natural &i) { return even(i.value[0]); }
        static bool even(const Integer &i) {
          return std::visit([](const auto &value) { return even(value); },
                            i.value);
        }
        static bool even(const Rational & /*i*/) { return false; }
        static bool even(const Number &i) {
          return i.visit([](const auto &value) { return even(value); });
        }

        static Rational to_rational(Number &&left, Number &&right);

        static Rational to_rational(const Base &left, const Base &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Base &left, const Natural &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Base &left, const Integer &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Base &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return to_rational(left * rD, Number{rN});
              },
              right.numerator, right.denominator);
        }

        static Rational to_rational(const Natural &left, const Base &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Natural &left, const Natural &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Natural &left, const Integer &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Natural &left,
                                    const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return to_rational((left * rD), Number{rN});
              },
              right.numerator, right.denominator);
        }

        static Rational to_rational(const Integer &left, const Base &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Integer &left, const Natural &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Integer &left, const Integer &right) {
          return Rational{left, right};
        }

        static Rational to_rational(const Integer &left,
                                    const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return to_rational((left * rD), Number{rN});
              },
              right.numerator, right.denominator);
        }

        static Rational to_rational(const Rational &left, const Base &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return to_rational(Number{lN}, (lD * right));
              },
              left.numerator, left.denominator);
        }

        static Rational to_rational(const Rational &left,
                                    const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return to_rational(Number{lN}, lD * right);
              },
              left.numerator, left.denominator);
        }

        static Rational to_rational(const Rational &left,
                                    const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return to_rational(Number{lN}, lD * right);
              },
              left.numerator, left.denominator);
        }

        static Rational to_rational(const Rational &left,
                                    const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return to_rational(lN * rD, lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        static Rational to_rational(Number &&left, Number &&right) {
          return left.visit(
              right, [](auto &&l, auto &&r) { return to_rational(l, r); });
        }

        template <typename Left, typename Right>
        Number reduce(Left &&left, Right &&right) {
          Number a{left}, b{right};
          while (even(a) && even(b)) {
            a >>= 1u;
            b >>= 1u;
          }
          auto aPrime = a, bPrime = b;
          while (aPrime != bPrime) {
            if (even(aPrime)) {
              aPrime >>= 1u;
            } else if (even(bPrime)) {
              bPrime >>= 1u;
            } else if (aPrime > bPrime) {
              aPrime = (aPrime - bPrime) >> 1u;
            } else {
              bPrime = (bPrime - aPrime) >> 1u;
            }
          }
          return Number{to_rational(a.floor_div(aPrime), b.floor_div(aPrime))};
        }

        Number operator/(const std::uint64_t &left, const Base &right) {
          if (left % right.value == 0) {
            return Number{Base{left / right.value}};
          }
          return reduce(Base{left}, right);
        }

        Number operator/(const std::uint64_t &left, const Natural &right) {
          return reduce(Base{left}, right);
        }

        Number operator/(const std::uint64_t &left, const Integer &right) {
          return -std::visit([&left](const auto &r) { return left / r; },
                             right.value);
        }

        Number operator/(const std::uint64_t &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) / rN;
              },
              right.numerator, right.denominator);
        }

        Number operator/(const Base &left, const std::uint64_t &right) {
          return reduce(left, Base{right});
        }

        Number operator/(const Base &left, const Base &right) {
          return reduce(left, right);
        }

        Number operator/(const Base &left, const Natural &right) {
          return reduce(left, right);
        }

        Number operator/(const Base &left, const Integer &right) {
          return -std::visit([&left](const auto &r) { return left / r; },
                             right.value);
        }

        Number operator/(const Base &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) / rN;
              },
              right.numerator, right.denominator);
        }

        Number operator/(const Natural &left, const std::uint64_t &right) {
          return reduce(left, Base{right});
        }

        Number operator/(const Natural &left, const Base &right) {
          return reduce(left, right);
        }

        Number operator/(const Natural &left, const Natural &right) {
          return reduce(left, right);
        }

        Number operator/(const Natural &left, const Integer &right) {
          return -std::visit([&left](const auto &r) { return left / r; },
                             right.value);
        }

        Number operator/(const Natural &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) / rN;
              },
              right.numerator, right.denominator);
        }

        Number operator/(const Integer &left, const std::uint64_t &right) {
          return -std::visit([&right](const auto &l) { return l / right; },
                             left.value);
        }

        Number operator/(const Integer &left, const Base &right) {
          return -std::visit([&right](const auto &l) { return l / right; },
                             left.value);
        }

        Number operator/(const Integer &left, const Natural &right) {
          return -std::visit([&right](const auto &l) { return l / right; },
                             left.value);
        }

        Number operator/(const Integer &left, const Integer &right) {
          return std::visit([](const auto &l, const auto &r) { return l / r; },
                            left.value, right.value);
        }

        Number operator/(const Integer &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return (left * rD) / rN;
              },
              right.numerator, right.denominator);
        }

        Number operator/(const Rational &left, const std::uint64_t &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN / (lD * right);
              },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Base &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN / (lD * right);
              },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN / (lD * right);
              },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return lN / (lD * right);
              },
              left.numerator, left.denominator);
        }

        Number operator/(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) { return (lN * rD) / (lD * rN); },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
