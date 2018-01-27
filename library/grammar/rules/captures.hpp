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

//! generic control status transformations

#pragma once

#include <optional>
#include <vector>

#include "grammar/rules/set.hpp"
#include "grammar/rules/stack.hpp"

namespace chimera {
  namespace library {
    namespace grammar {
      namespace rules {
        template <typename... Types>
        using UniqueStack = metal::apply<metal::lambda<Stack>, Set<Types...>>;
        template <typename Type>
        struct OptionalCapture : Stack<Type> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (Stack<Type>::has_value()) {
              std::visit(outer, Stack<Type>::pop());
            } else {
              outer.push(std::optional<Type>{});
            }
          }
        };
        template <typename Type>
        struct OptionalCapture<std::optional<Type>>
            : Stack<std::optional<Type>> {
          template <typename Outer>
          void success(Outer &&outer) {
            if (Stack<std::optional<Type>>::has_value()) {
              std::visit(outer, Stack<std::optional<Type>>::pop());
            } else {
              outer.push(std::optional<Type>{});
            }
          }
        };
        template <typename Type, typename... Types>
        struct ReshapeCapture : UniqueStack<Types...> {
          template <typename Outer>
          void success(Outer &&outer) {
            outer.push(
                UniqueStack<Types...>::template reshape<Type, Types...>());
          }
        };
        template <typename... Types>
        struct VariantCapture : Stack<Types...> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::visit(outer, Stack<Types...>::pop());
          }
        };
        template <typename Type>
        struct VectorCapture : Stack<Type> {
          template <typename Outer>
          void success(Outer &&outer) {
            std::vector<Type> vector;
            vector.reserve(Stack<Type>::size());
            Stack<Type>::template transform<Type>(std::back_inserter(vector));
            outer.push(std::move(vector));
          }
        };
      } // namespace rules
    }   // namespace grammar
  }     // namespace library
} // namespace chimera
