// Copyright (c) 2018 Asa Katida <github@holomaplefeline.net>
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

#include "object/number/gcd.hpp"

#include <algorithm>

#include <gsl/gsl>

#include "container/reverse.hpp"
#include "object/number/add.hpp"
#include "object/number/less.hpp"
#include "object/number/mod.hpp"
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

        template <typename Left>
        Integer div(const Left &left, const Rational &right) {
          return std::visit(
              [&left](const auto &rN, const auto &rD) {
                return Integer(gcd(left * rD, rN));
              },
              right.numerator, right.denominator);
        }

        template <typename Right>
        Integer div(const Rational &left, const Right &right) {
          return std::visit(
              [&right](const auto &lN, const auto &lD) {
                return Integer(gcd(lN, lD * right));
              },
              left.numerator, left.denominator);
        }

        Base gcd(std::uint64_t left, Base right) {
          Base aPrime{left}, bPrime = right;
          while (0u < bPrime) {
            auto temp = bPrime;
            bPrime = aPrime % bPrime;
            aPrime = temp;
          }
          return aPrime;
        }

        Base gcd(std::uint64_t left, const Natural &right) {
          return gcd(right, left);
        }

        Base gcd(std::uint64_t left, const Positive &right) {
          return std::visit(
              [left](const auto &value) { return gcd(left, value); },
              right.value);
        }

        Negative gcd(std::uint64_t left, const Negative &right) {
          return std::visit(
              [left](const auto &value) { return -gcd(left, value); },
              right.value);
        }

        Integer gcd(std::uint64_t left, const Integer &right) {
          return std::visit(
              [left](const auto &value) {
                return Integer(gcd(left, value));
              },
              right.value);
        }

        Integer gcd(std::uint64_t left, const Rational &right) {
          return div(left, right);
        }

        Integer gcd(std::uint64_t left, const Real &right) {
          return std::visit(
              [left](const auto &value) {
                return Integer(gcd(left, value));
              },
              right.value);
        }

        Base gcd(Base left, std::uint64_t right) {
          Expects(right != 0);
          return {left.value / right};
        }

        Base gcd(Base left, Base right) {
          Expects(right.value != 0);
          return {left.value / right.value};
        }

        Base gcd(Base left, const Natural &right) {
          return gcd(left.value, right);
        }

        Base gcd(Base left, const Positive &right) {
          return gcd(left.value, right);
        }

        Negative gcd(Base left, const Negative &right) {
          return gcd(left.value, right);
        }

        Integer gcd(Base left, const Integer &right) {
          return gcd(left.value, right);
        }

        Integer gcd(Base left, const Rational &right) {
          return gcd(left.value, right);
        }

        Integer gcd(Base left, const Real &right) {
          return gcd(left.value, right);
        }

        Base gcd(const Natural &left, std::uint64_t right) {
          Positive aPrime(copy(left)), bPrime(Base{right});
          while (0u < bPrime) {
            auto temp = bPrime;
            bPrime = aPrime % bPrime;
            aPrime = temp;
          }
          return std::get<Base>(aPrime.value);
        }

        Base gcd(const Natural &left, Base right) {
          return gcd(left, right.value);
        }

        Positive gcd(const Natural &left, const Natural &right) {
          // TODO(asakatida)
          // while (even(left) && even(right)) {
          //   left = left >> 1u;
          //   right = right >> 1u;
          // }
          // if (left == 1u) {
          //   return to_rational(std::move(left), std::move(right));
          // }
          // if (right == 1u) {
          //   return real(std::move(left));
          // }
          Positive aPrime(copy(left)), bPrime(copy(right));
          while (0u < bPrime) {
            auto temp = bPrime;
            bPrime = aPrime % bPrime;
            aPrime = temp;
          }
          return aPrime;
        }

        // TODO(asakatida)
        // static Positive gcd(const Positive &left, const Positive &right) {
        //   auto aPrime = left, bPrime = right;
        //   std::visit(Repr<std::ostream>{std::cout << "left: "}, left.value) << '\n';
        //   std::visit(Repr<std::ostream>{std::cout << "right: "}, right.value) << '\n';
        //   while (!(aPrime == bPrime)) {
        //     std::visit(Repr<std::ostream>{std::cout << "aPrime: "}, aPrime.value) << '\n';
        //     std::visit(Repr<std::ostream>{std::cout << "bPrime: "}, bPrime.value) << '\n';
        //     if (even(aPrime)) {
        //       aPrime = aPrime >> 1u;
        //     } else if (even(bPrime)) {
        //       bPrime = bPrime >> 1u;
        //     } else if (bPrime < aPrime) {
        //       aPrime = +(aPrime - bPrime) >> 1u;
        //     } else {
        //       bPrime = +(bPrime - aPrime) >> 1u;
        //     }
        //   }
        //   std::visit(Repr<std::ostream>{std::cout << "aPrime: "}, aPrime.value) << '\n';
        //   return aPrime;
        // }

        Positive gcd(const Natural &left, const Positive &right) {
          return std::visit(
              [&left](const auto &value) {
                return Positive(gcd(left, value));
              },
              right.value);
        }

        Negative gcd(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &r) { return -gcd(left, r); },
              right.value);
        }

        Integer gcd(const Natural &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) {
                return Integer(gcd(left, value));
              },
              right.value);
        }

        Integer gcd(const Natural &left, const Rational &right) {
          return div(left, right);
        }

        Integer gcd(const Natural &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) {
                return Integer(gcd(left, value));
              },
              right.value);
        }

        Positive gcd(const Positive &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) {
                return Positive(gcd(value, right));
              },
              left.value);
        }

        Positive gcd(const Positive &left, Base right) {
          return gcd(left, right.value);
        }

        Positive gcd(const Positive &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) {
                return Positive(gcd(value, right));
              },
              left.value);
        }

        Positive gcd(const Positive &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Positive(gcd(l, r));
              },
              left.value, right.value);
        }

        Negative gcd(const Positive &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return -gcd(l, r); },
              left.value, right.value);
        }

        Integer gcd(const Positive &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Positive &left, const Rational &right) {
          return div(left, right);
        }

        Integer gcd(const Positive &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }

        Negative gcd(const Negative &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return -gcd(value, right); },
              left.value);
        }

        Negative gcd(const Negative &left, Base right) {
          return gcd(left, right.value);
        }

        Negative gcd(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -gcd(value, right); },
              left.value);
        }

        Negative gcd(const Negative &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) { return -gcd(l, r); },
              left.value, right.value);
        }

        Positive gcd(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Positive(gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Negative &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(-gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Negative &left, const Rational &right) {
          return div(left, right);
        }

        Integer gcd(const Negative &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(-gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Integer &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) {
                return Integer(gcd(value, right));
              },
              left.value);
        }

        Integer gcd(const Integer &left, Base right) {
          return gcd(left, right.value);
        }

        Integer gcd(const Integer &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(gcd(value, right));
              },
              left.value);
        }

        Integer gcd(const Integer &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Integer &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(gcd(value, right));
              },
              left.value);
        }

        Integer gcd(const Integer &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Integer &left, const Rational &right) {
          return div(left, right);
        }

        Integer gcd(const Integer &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Rational &left, std::uint64_t right) {
          return div(left, right);
        }

        Integer gcd(const Rational &left, Base right) {
          return gcd(left, right.value);
        }

        Integer gcd(const Rational &left, const Natural &right) {
          return div(left, right);
        }

        Integer gcd(const Rational &left, const Positive &right) {
          return div(left, right);
        }

        Integer gcd(const Rational &left, const Negative &right) {
          return div(left, right);
        }

        Integer gcd(const Rational &left, const Integer &right) {
          return div(left, right);
        }

        Integer gcd(const Rational &left, const Rational &right) {
          return std::visit(
              [](const auto &lN, const auto &lD, const auto &rN,
                 const auto &rD) {
                return Integer(gcd(lN * rD, lD * rN));
              },
              left.numerator, left.denominator, right.numerator,
              right.denominator);
        }

        Integer gcd(const Rational &left, const Real &right) {
          return std::visit(
              [&left](const auto &value) {
                return Integer(gcd(left, value));
              },
              right.value);
        }

        Integer gcd(const Real &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) {
                return Integer(gcd(value, right));
              },
              left.value);
        }

        Integer gcd(const Real &left, Base right) {
          return gcd(left, right.value);
        }

        Integer gcd(const Real &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(gcd(value, right));
              },
              left.value);
        }

        Integer gcd(const Real &left, const Positive &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Real &left, const Negative &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(gcd(value, right));
              },
              left.value);
        }

        Integer gcd(const Real &left, const Integer &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }

        Integer gcd(const Real &left, const Rational &right) {
          return std::visit(
              [&right](const auto &value) {
                return Integer(gcd(value, right));
              },
              left.value);
        }

        Integer gcd(const Real &left, const Real &right) {
          return std::visit(
              [](const auto &l, const auto &r) {
                return Integer(gcd(l, r));
              },
              left.value, right.value);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
