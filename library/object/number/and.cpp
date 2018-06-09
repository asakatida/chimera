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

#include "object/number/and.hpp"

#include <functional>

#include <gsl/gsl>

#include "object/number/negative.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Base operator&(std::uint64_t left, Base right) {
          return {left & right.value};
        }

        Base operator&(std::uint64_t left, const Natural &right) {
          return {left & right.value[0]};
        }

        Base operator&(std::uint64_t left, const Positive &right) {
          return std::visit([left](const auto &value) { return left & value; },
                            right.value);
        }

        Base operator&(std::uint64_t left, const Negative &right) {
          return std::visit([left](const auto &value) { return left & value; },
                            right.value);
        }

        Base operator&(std::uint64_t left, const Integer &right) {
          return std::visit([left](const auto &value) { return left & value; },
                            right.value);
        }

        Base operator&(std::uint64_t /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator&(std::uint64_t /*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator&(Base left, std::uint64_t right) {
          return {left.value & right};
        }

        Base operator&(Base left, Base right) {
          return {left.value & right.value};
        }

        Base operator&(Base left, const Natural &right) {
          return {left.value & right.value[0]};
        }

        Base operator&(Base left, const Positive &right) {
          return std::visit([left](const auto &value) { return left & value; },
                            right.value);
        }

        Base operator&(Base left, const Negative &right) {
          return std::visit([left](const auto &value) { return left & value; },
                            right.value);
        }

        Base operator&(Base left, const Integer &right) {
          return std::visit([left](const auto &value) { return left & value; },
                            right.value);
        }

        Base operator&(Base /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator&(Base /*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator&(const Natural &left, std::uint64_t right) {
          return {left.value[0] & right};
        }

        Base operator&(const Natural &left, Base right) {
          return {left.value[0] & right.value};
        }

        Natural operator&(const Natural &left, const Natural &right) {
          Natural value;
          value.value.resize(std::min(left.value.size(), right.value.size()));
          if (left.value.size() > right.value.size()) {
            std::transform(right.value.begin(), right.value.end(),
                           left.value.begin(), value.value.begin(),
                           std::bit_and<std::uint64_t>{});
          } else {
            std::transform(left.value.begin(), left.value.end(),
                           right.value.begin(), value.value.begin(),
                           std::bit_and<std::uint64_t>{});
          }
          return value;
        }

        Positive operator&(const Natural &left, const Positive &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left & value); },
              right.value);
        }

        Positive operator&(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left & value); },
              right.value);
        }

        Positive operator&(const Natural &left, const Integer &right) {
          return std::visit(
              [&left](const auto &value) { return Positive(left & value); },
              right.value);
        }

        Base operator&(const Natural & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator&(const Natural &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator&(const Positive &left, std::uint64_t right) {
          return std::visit(
              [right](const auto &value) { return value & right; }, left.value);
        }

        Base operator&(const Positive &left, Base right) {
          return std::visit(
              [right](const auto &value) { return value & right; }, left.value);
        }

        Positive operator&(const Positive &left, const Natural &right) {
          return std::visit(
              [right](const auto &value) { return Positive(value & right); },
              left.value);
        }

        Positive operator&(const Positive &left, const Positive &right) {
          return std::visit(
              [right](const auto &value) { return Positive(value & right); },
              left.value);
        }

        Positive operator&(const Positive &left, const Negative &right) {
          return std::visit(
              [right](const auto &l, const auto &r) { return Positive(l & r); },
              left.value, right.value);
        }

        Positive operator&(const Positive & /*left*/,
                           const Integer & /*right*/) {
          Expects(false);
        }

        Base operator&(const Positive & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator&(const Positive &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator&(const Negative &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &value) { return value & right; },
              left.value);
        }

        Base operator&(const Negative &left, Base right) {
          return left & right.value;
        }

        Positive operator&(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return Positive(value & right); },
              left.value);
        }

        Positive operator&(const Negative & /*left*/,
                           const Positive & /*right*/) {
          Expects(false);
        }

        Negative operator&(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &a, const auto &b) { return -(a & b); }, left.value,
              right.value);
        }

        Integer operator&(const Negative & /*left*/,
                          const Integer & /*right*/) {
          Expects(false);
        }

        Base operator&(const Negative & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator&(const Negative &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator&(const Integer & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Base operator&(const Integer & /*left*/, Base /*right*/) {
          Expects(false);
        }

        Positive operator&(const Integer & /*left*/,
                           const Natural & /*right*/) {
          Expects(false);
        }

        Positive operator&(const Integer & /*left*/,
                           const Positive & /*right*/) {
          Expects(false);
        }

        Integer operator&(const Integer & /*left*/,
                          const Negative & /*right*/) {
          Expects(false);
        }

        Integer operator&(const Integer & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }

        Base operator&(const Integer & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }

        Base operator&(const Integer &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
        Base operator&(const Rational & /*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Base operator&(const Rational & /*left*/, Base /*right*/) {
          Expects(false);
        }

        Base operator&(const Rational & /*left*/, const Natural & /*right*/) {
          Expects(false);
        }

        Base operator&(const Rational & /*left*/, const Positive & /*right*/) {
          Expects(false);
        }
        Base operator&(const Rational & /*left*/, const Negative & /*right*/) {
          Expects(false);
        }

        Base operator&(const Rational & /*left*/, const Integer & /*right*/) {
          Expects(false);
        }
        Base operator&(const Rational & /*left*/, const Rational & /*right*/) {
          Expects(false);
        }
        Base operator&(const Rational &/*left*/, const Real &/*right*/) {
          Expects(false);
        }

        Base operator&(const Real &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }
        Base operator&(const Real &/*left*/, Base /*right*/) {
          Expects(false);
        }
        Base operator&(const Real &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }
        Base operator&(const Real &/*left*/, const Positive &/*right*/) {
          Expects(false);
        }
        Base operator&(const Real &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }
        Base operator&(const Real &/*left*/, const Integer &/*right*/) {
          Expects(false);
        }
        Base operator&(const Real &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }
        Base operator&(const Real &/*left*/, const Real &/*right*/) {
          Expects(false);
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
