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

#include "object/number/overflow.hpp"

#include <cstdint>
#include <tuple> // for tuple

#include <gsl/gsl>

namespace chimera {
  namespace library {
    namespace object {

      namespace number {
        Carryover sum(std::uint64_t left, std::uint64_t right) {
          auto carryover = (left & 0xFFFFFFFF) + (right & 0xFFFFFFFF);
          auto lowField = carryover & 0xFFFFFFFF;
          carryover >>= 32;
          carryover +=
              ((left >> 32) & 0xFFFFFFFF) + ((right >> 32) & 0xFFFFFFFF);
          return {{lowField | ((carryover & 0xFFFFFFFF) << 32)},
                  {carryover >> 32}};
        }

        Carryover sub(std::uint64_t left, std::uint64_t right) {
          if (left < right) {
            return {
                {std::numeric_limits<std::uint64_t>::max() - (right - left)},
                {1}};
          }
          return {{left - right}, {0}};
        }

        Carryover mult(std::uint64_t left, std::uint64_t right) {
          auto carryover = (left & 0xFFFF) * (right & 0xFFFF);
          auto field0 = carryover & 0xFFFF;
          carryover >>= 16;

          carryover += (left & 0xFFFF) * ((right >> 16) & 0xFFFF) +
                       ((left >> 16) & 0xFFFF) * (right & 0xFFFF);
          auto field16 = carryover & 0xFFFF;
          carryover >>= 16;

          carryover += (left & 0xFFFF) * ((right >> 32) & 0xFFFF) +
                       ((left >> 16) & 0xFFFF) * ((right >> 16) & 0xFFFF) +
                       ((left >> 32) & 0xFFFF) * (right & 0xFFFF);
          auto field32 = carryover & 0xFFFF;
          carryover >>= 16;

          carryover += (left & 0xFFFF) * ((right >> 48) & 0xFFFF) +
                       ((left >> 16) & 0xFFFF) * ((right >> 32) & 0xFFFF) +
                       ((left >> 32) & 0xFFFF) * ((right >> 16) & 0xFFFF) +
                       ((left >> 48) & 0xFFFF) * (right & 0xFFFF);
          auto field48 = carryover & 0xFFFF;
          carryover >>= 16;

          carryover += ((left >> 16) & 0xFFFF) * ((right >> 48) & 0xFFFF) +
                       ((left >> 32) & 0xFFFF) * ((right >> 32) & 0xFFFF) +
                       ((left >> 48) & 0xFFFF) * ((right >> 16) & 0xFFFF);
          auto field64 = carryover & 0xFFFF;
          carryover >>= 16;

          carryover += ((left >> 32) & 0xFFFF) * ((right >> 48) & 0xFFFF) +
                       ((left >> 48) & 0xFFFF) * ((right >> 32) & 0xFFFF);
          auto field80 = carryover & 0xFFFF;
          carryover >>= 16;

          carryover += ((left >> 48) & 0xFFFF) * ((right >> 48) & 0xFFFF);

          return {
              {field0 | (field16 << 16) | (field32 << 32) | (field48 << 48)},
              {field64 | (field80 << 16) | (carryover << 32)}};
        }
      } // namespace number
    }   // namespace object
  }     // namespace library
};      // namespace chimera
