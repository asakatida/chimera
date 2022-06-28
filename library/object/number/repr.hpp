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

#pragma once

#include "object/number/number.hpp"

namespace chimera::library::object::number {
  template <typename OStream>
  struct Repr {
    OStream &os;
    auto operator()(std::uint64_t i) -> OStream & { return os << i << ','; }

    auto operator()(Base base) -> OStream & {
      return os << "Number(" << base.value << ')';
    }

    auto operator()(const Natural &natural) -> OStream & {
      os << "Number({{";
      std::for_each(natural.value.begin(), natural.value.end(), *this);
      return os << "}})";
    }

    auto operator()(const Negative &negative) -> OStream & {
      os << '-';
      return std::visit(*this, negative.value);
    }

    auto operator()(const Rational &rational) -> OStream & {
      std::visit(*this, rational.numerator);
      os << '/';
      return std::visit(*this, rational.denominator);
    }

    auto operator()(const Imag &imag) -> OStream & {
      return std::visit(*this, imag.value) << ".complex()";
    }

    auto operator()(const Complex &complex) -> OStream & {
      std::visit(*this, complex.real) << '+';
      return std::visit(*this, complex.imag) << ".complex()";
    }
  };
  template <typename OStream>
  auto Number::repr(OStream &os) const -> OStream & {
    return std::visit(Repr<OStream>{os}, value);
  }
} // namespace chimera::library::object::number
