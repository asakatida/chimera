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

#pragma once

#include <cstdint>
#include <numeric>
#include <variant>
#include <vector>

#include <tao/operators.hpp>

namespace chimera {
  namespace library {
    namespace object {
      namespace number {
        template <typename Left>
        Number operator+(Left &&left, Number &&right) {
          return Number(Left(left)) + right;
        }

        template <typename Right>
        Number operator+(Number &&left, Right &&right) {return left + Number(Right(right));}

        template <typename Left>
        Number operator-(Left &&left, Number &&right) {return Number(Left(left)) - right;}

        template <typename Right>
        Number operator-(Number &&left, Right &&right) {return left - Number(Right(right));}

        template <typename Left>
        Number operator*(Left &&left, Number &&right) {return Number(Left(left)) * right;}

        template <typename Right>
        Number operator*(Number &&left, Right &&right) {return left * Number(Right(right));}

        template <typename Left>
        Number operator/(Left &&left, Number &&right) {return Number(Left(left)) / right;}

        template <typename Right>
        Number operator/(Number &&left, Right &&right) {return left / Number(Right(right));}

        template <typename Left>
        Number operator%(Left &&left, Number &&right) {return Number(Left(left)) % right;}

        template <typename Right>
        Number operator%(Number &&left, Right &&right) {return left % Number(Right(right));}

        template <typename Left>
        Number operator&(Left &&left, Number &&right) {return Number(Left(left)) & right;}

        template <typename Right>
        Number operator&(Number &&left, Right &&right) {return left & Number(Right(right));}

        template <typename Left>
        Number operator|(Left &&left, Number &&right) {return Number(Left(left)) | right;}

        template <typename Right>
        Number operator|(Number &&left, Right &&right) {return left | Number(Right(right));}

        template <typename Left>
        Number operator^(Left &&left, Number &&right) {return Number(Left(left)) ^ right;}

        template <typename Right>
        Number operator^(Number &&left, Right &&right) {return left ^ Number(Right(right));}

        template <typename Left>
        Number operator<<(Left &&left, Number &&right) {return Number(Left(left)) << right;}

        template <typename Right>
        Number operator<<(Number &&left, Right &&right) {return left << Number(Right(right));}

        template <typename Left>
        Number operator>>(Left &&left, Number &&right) {return Number(Left(left)) >> right;}

        template <typename Right>
        Number operator>>(Number &&left, Right &&right) {return left >> Number(Right(right));}

        template <typename Left>
        bool operator==(const Left &left, const Number &right) {return Number(Left(left)) == right;}

        template <typename Right>
        bool operator==(const Number &left, const Right &right) {return left == Number(Right(right));}

        template <typename Left>
        bool operator<(const Left &left, const Number &right) {return Number(Left(left)) < right;}

        template <typename Right>
        bool operator<(const Number &left, const Right &right) {return left < Number(Right(right));}

        template <typename Left>
        Number floor_div(Left &&left, Number &&right) {return Number(Left(left)).floor_div(right);}

        template <typename Right>
        Number floor_div(Number &&left, Right &&right) {return left.floor_div(Number(Right(right)));}

        template <typename Left>
        Number gcd(Left &&left, Number &&right) {return Number(Left(left)).gcd(right);}

        template <typename Right>
        Number gcd(Number &&left, Right &&right) {return left.gcd(Number(Right(right)));}

        template <typename Left>
        Number pow(Left &&left, Number &&right) {return Number(Left(left)).pow(right);}

        template <typename Right>
        Number pow(Number &&left, Right &&right) {return left.pow(Number(Right(right)));}
      } // namespace number
    }   // namespace object
  }     // namespace library
} // namespace chimera
