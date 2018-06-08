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

#include "object/number/details.hpp"

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        static std::variant<Base, Natural> positive(Natural &&value) {
          while (!value.value.empty()) {
            if (value.value.back() != 0) {
              break;
            }
            value.value.pop_back();
          }
          if (value.value.empty()) {
            return Base{};
          }
          if (value.value.size() == 1) {
            return Base{value.value[0]};
          }
          value.value.shrink_to_fit();
          return value;
        }

        using IntegerValue = std::variant<Base, Natural, Negative>;

        static IntegerValue integer(Natural &&value) {
          while (!value.value.empty()) {
            if (value.value.back() != 0) {
              break;
            }
            value.value.pop_back();
          }
          if (value.value.empty()) {
            return Base{};
          }
          if (value.value.size() == 1) {
            return Base{value.value[0]};
          }
          value.value.shrink_to_fit();
          return value;
        }

        static IntegerValue integer(Positive &&value) {
          return std::visit([](auto &&v) { return IntegerValue(v); },
                            value.value);
        }

        Positive::Positive(Base base) : value(base) {}
        Positive::Positive(Natural natural)
            : value(positive(std::move(natural))) {}

        Negative::Negative(Base base) : value(base) {}
        Negative::Negative(Natural natural)
            : value(positive(std::move(natural))) {}
        Negative::Negative(Positive positive)
            : value(std::move(positive.value)) {}

        Integer::Integer(Base base) : value(base) {}
        Integer::Integer(Natural natural)
            : value(integer(std::move(natural))) {}
        Integer::Integer(Positive positive)
            : value(integer(std::move(positive))) {}
        Integer::Integer(Negative negative) : value(std::move(negative)) {}
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
