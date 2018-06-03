// Copyright (c) 2018 Adam Grandquist <grandquista@gmail.com>
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

#include "object/number/simplify.hpp"

#include <iostream>

#include "object/number/compare.hpp"
#include "object/number/mult.hpp"
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
                return to_rational(left * rD, Number(rN));
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
                return to_rational((left * rD), Number(rN));
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
                return to_rational((left * rD), Number(rN));
              },
              right.numerator, right.denominator);
        }

        static Rational to_rational(const Rational &left, const Base &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return to_rational(Number(lN), (lD * right));
              },
              left.numerator, left.denominator);
        }

        static Rational to_rational(const Rational &left,
                                    const Natural &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return to_rational(Number(lN), lD * right);
              },
              left.numerator, left.denominator);
        }

        static Rational to_rational(const Rational &left,
                                    const Integer &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return to_rational(Number(lN), lD * right);
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
          if (left == 0u) {
            return Number();
          }
          if (left == 1u) {
            return Number(Rational{left, right});
          }
          if (right == 0u) {
            return Number(Rational{left, {}});
          }
          if (right == 1u) {
            return Number(left);
          }
          Number a{left}, b{right};
          while (even(a) && even(b)) {
            a >>= 1u;
            b >>= 1u;
          }
          if (a == 1u) {
            return Number(to_rational(std::move(a), std::move(b)));
          }
          if (b == 1u) {
            return a;
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
          if (aPrime == 1u) {
            return Number(to_rational(std::move(a), std::move(b)));
          }
          if (a == aPrime) {
            return Number(to_rational(Number(1), Number(b).floor_div(aPrime)));
          }
          if (b == aPrime) {
            return Number(a).floor_div(aPrime);
          }
          return Number(to_rational(Number(a).floor_div(aPrime), Number(b).floor_div(aPrime)));
        }

        Number simplify(Base base) { return Number(base); }
        Number simplify(Natural natural) {
          while (!natural.value.empty()) {
            if (natural.value.back() != 0) {
              break;
            }
            natural.value.pop_back();
          }
          if (natural.value.empty()) {
            return Number();
          }
          if (natural.value.size() == 1) {
            return Number(Base{natural.value[0]});
          }
          natural.value.shrink_to_fit();
          return Number(natural);
        }
        Number simplify(Integer integer) {
          return std::visit([](const auto &value) { return -simplify(value); },
                            integer.value);
        }
        Number simplify(Rational rational) {
          return std::visit(
              [](const auto &numerator, const auto &denominator) {
                return reduce(numerator, denominator);
              },
              rational.numerator, rational.denominator);
        }
        Number simplify(const Number &number) {
          return number.visit([](auto value) { return simplify(value); });
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
};      // namespace chimera
