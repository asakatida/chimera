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

//! parse state template
//! Inherited by specializations that provide a success method to merge with
//! outer states.  Control hooks create all instances of specializations.

#pragma once

#include <algorithm>
#include <variant>
#include <vector>

#include <gsl/gsl>

namespace chimera {
  namespace library {
    namespace grammar {
      template <typename... Types>
      struct Stack {
        using ValueT = std::variant<Types...>;
        template <typename Type, typename = decltype(std::get<Type>(ValueT{}))>
        void push(Type &&type) {
          return stack.push_back(ValueT{std::forward<Type>(type)});
        }
        const ValueT &top() const {
          Expects(has_value());
          return stack.back();
        }
        ValueT &top() {
          Expects(has_value());
          return stack.back();
        }
        template <typename Type, typename = decltype(std::get<Type>(ValueT{}))>
        void print_debug() const {
          if (!top_is<Type>()) {
            if (has_value()) {
              std::visit(
                  [](auto &&t) {
                    std::cout << "Wanted: " << typeid(Type).name()
                              << "\nFound: " << typeid(t).name() << '\n';
                  },
                  top());
            } else {
              std::cout << "Wanted: " << typeid(Type).name()
                        << "\nFound: empty stack\n";
            }
          }
        }
        template <typename Type, typename = decltype(std::get<Type>(ValueT{}))>
        const Type &top() const {
          print_debug<Type>();
          Ensures(top_is<Type>());
          return std::get<Type>(top());
        }
        template <typename Type, typename = decltype(std::get<Type>(ValueT{}))>
        Type &top() {
          print_debug<Type>();
          Ensures(top_is<Type>());
          return std::get<Type>(top());
        }
        ValueT pop() {
          auto finally = gsl::finally([this] { this->stack.pop_back(); });
          return std::move(top());
        }
        template <typename Type, typename = decltype(std::get<Type>(ValueT{}))>
        Type pop() {
          auto finally = gsl::finally([this] { this->stack.pop_back(); });
          return std::move(top<Type>());
        }
        const std::vector<ValueT> &vector() const { return stack; }
        template <
            typename OutputIt,
            typename Type = typename std::iterator_traits<OutputIt>::value_type,
            typename = decltype(std::get<Type>(ValueT{}))>
        void transform(OutputIt &&outputIt) {
          std::transform(
              stack.begin(), stack.end(), std::forward<OutputIt>(outputIt),
              [](ValueT &value) {
                if (!std::holds_alternative<Type>(value)) {
                  std::visit(
                      [](auto &&t) { std::cout << typeid(t).name() << '\n'; },
                      value);
                }
                Ensures(std::holds_alternative<Type>(value));
                return std::move(std::get<Type>(value));
              });
          return stack.clear();
        }
        bool has_value() const { return !stack.empty(); }
        template <typename Type, typename = decltype(std::get<Type>(ValueT{}))>
        bool top_is() const {
          return has_value() && std::holds_alternative<Type>(top());
        }
        std::size_t size() const { return stack.size(); }
        template <typename Type, typename = decltype(std::get<Type>(ValueT{}))>
        void operator()(Type &&type) {
          return push(std::forward<Type>(type));
        }

      private:
        std::vector<ValueT> stack;
      };
    } // namespace grammar
  }   // namespace library
} // namespace chimera
