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

#include "object/number/xor.hpp"

#include <gsl/gsl>

#include "object/number/negative.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        Number operator^(std::uint64_t left, Base right) {
          return {left ^ right.value};
        }

        Number operator^(std::uint64_t left, const Natural &right) {
          auto value = right;
          value.value[0] ^= left;
          return value;
        }

        Number operator^(std::uint64_t left, const Negative &right) {
          return std::visit(
              [&left](const auto &value) { return -(left ^ value); },
              right.value);
        }

        Number operator^(std::uint64_t /*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number operator^(std::uint64_t /*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number operator^(std::uint64_t /*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number operator^(Base left, std::uint64_t right) {
          return {left.value ^ right};
        }

        Number operator^(Base left, Base right) {
          return {left.value ^ right.value};
        }

        Number operator^(Base left, const Natural &right) {
          auto value = right;
          value.value[0] ^= left.value;
          return value;
        }

        Number operator^(Base left, const Negative &right) {
          return std::visit(
              [&left](const auto &value) { return -(left ^ value); },
              right.value);
        }

        Number operator^(Base /*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number operator^(Base /*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number operator^(Base /*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number operator^(const Natural &left, std::uint64_t right) {
          auto value = left;
          value.value[0] ^= right;
          return value;
        }

        Number operator^(const Natural &left, Base right) {
          return left ^ right.value;
        }

        Number operator^(const Natural &left, const Natural &right) {
          auto value = left;
          value.value.resize(std::max(left.value.size(), right.value.size()));
          if (left.value.size() > right.value.size()) {
            std::transform(right.value.begin(), right.value.end(),
                           left.value.begin(), value.value.begin(),
                           std::bit_xor<std::uint64_t>{});
          } else {
            std::transform(left.value.begin(), left.value.end(),
                           right.value.begin(), value.value.begin(),
                           std::bit_xor<std::uint64_t>{});
          }

          return value;
        }

        Number operator^(const Natural &left, const Negative &right) {
          return std::visit(
              [&left](const auto &value) { return -(left ^ value); },
              right.value);
        }

        Number operator^(const Natural &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number operator^(const Natural &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number operator^(const Natural &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number operator^(const Negative &left, std::uint64_t right) {
          return std::visit(
              [&right](const auto &value) { return -(value ^ right); },
              left.value);
        }

        Number operator^(const Negative &left, Base right) {
          return left ^ right.value;
        }

        Number operator^(const Negative &left, const Natural &right) {
          return std::visit(
              [&right](const auto &value) { return -(value ^ right); },
              left.value);
        }

        Number operator^(const Negative &left, const Negative &right) {
          return std::visit(
              [](const auto &a, const auto &b) { return a ^ b); },
              left.value, right.value);
        }
        Number operator^(const Negative &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number operator^(const Negative &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number operator^(const Negative &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number operator^(const Rational &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number operator^(const Rational &/*left*/, Base /*right*/) {
          Expects(false);
        }

        Number operator^(const Rational &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }

        Number operator^(const Rational &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }

        Number operator^(const Rational &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number operator^(const Rational &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number operator^(const Rational &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number operator^(const Imag &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number operator^(const Imag &/*left*/, Base /*right*/) {
          Expects(false);
        }

        Number operator^(const Imag &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }

        Number operator^(const Imag &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }

        Number operator^(const Imag &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number operator^(const Imag &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number operator^(const Imag &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

        Number operator^(const Complex &/*left*/, std::uint64_t /*right*/) {
          Expects(false);
        }

        Number operator^(const Complex &/*left*/, Base /*right*/) {
          Expects(false);
        }

        Number operator^(const Complex &/*left*/, const Natural &/*right*/) {
          Expects(false);
        }

        Number operator^(const Complex &/*left*/, const Negative &/*right*/) {
          Expects(false);
        }

        Number operator^(const Complex &/*left*/, const Rational &/*right*/) {
          Expects(false);
        }

        Number operator^(const Complex &/*left*/, const Imag &/*right*/) {
          Expects(false);
        }

        Number operator^(const Complex &/*left*/, const Complex &/*right*/) {
          Expects(false);
        }

      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
