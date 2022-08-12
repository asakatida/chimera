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

//! parse state template
//! Inherited by specializations that provide a success method to merge with
//! outer states.  Control hooks create all instances of specializations.

#pragma once

#include <algorithm>
#include <tuple>
#include <variant>
#include <vector>

#include <gsl/gsl>

namespace chimera::library::grammar::rules {
  template <typename... Types>
  struct Stack {
    using ValueT = std::variant<Types...>;
    template <typename Type>
    void push(Type &&type) {
      stack.emplace_back(ValueT{std::forward<Type>(type)});
    }
    [[nodiscard]] auto top() const -> const ValueT & { return stack.back(); }
    auto top() -> ValueT & { return stack.back(); }
    template <typename Type>
    [[nodiscard]] auto top() const -> const Type & {
      Ensures(std::holds_alternative<Type>(top()));
      return std::get<Type>(top());
    }
    template <typename Type>
    auto top() -> Type & {
      Ensures(std::holds_alternative<Type>(top()));
      return std::get<Type>(top());
    }
    auto pop() -> ValueT {
      auto finally = gsl::finally([this] { this->stack.pop_back(); });
      return std::move(top());
    }
    template <typename Type>
    auto pop() -> Type {
      auto finally = gsl::finally([this] { this->stack.pop_back(); });
      return std::move(top<Type>());
    }
    template <typename Type, typename... Args>
    struct Reshape {
      template <typename Iter, std::size_t... I>
      static auto reshape(Iter &&it, std::index_sequence<I...> /*unused*/)
          -> Type {
        Ensures(std::holds_alternative<Args>(*(it + I)) && ...);
        return Type{std::move(std::get<Args>(*(it + I)))...};
      }
    };
    template <typename Type, typename... Args>
    auto reshape() -> Type {
      using _Stack = Reshape<Type, Args...>;
      Expects(sizeof...(Args) == size());
      auto finally = gsl::finally([this] { this->stack.clear(); });
      return _Stack::reshape(stack.begin(), std::index_sequence_for<Args...>{});
    }
    [[nodiscard]] auto vector() const -> const std::vector<ValueT> & {
      return stack;
    }
    auto vector() -> std::vector<ValueT> & { return stack; }
    template <typename OutputIt>
    auto transform(OutputIt &&outputIt) {
      using IteratorTraits = std::iterator_traits<OutputIt>;
      return transform<typename IteratorTraits::value_type>(
          std::forward<OutputIt>(outputIt));
    }
    template <typename Type, typename OutputIt>
    void transform(OutputIt &&outputIt) {
      auto finally = gsl::finally([this] { this->stack.clear(); });
      for (const auto &value : stack) {
        Ensures(std::holds_alternative<Type>(value));
        *(outputIt++) = std::move(std::get<Type>(value));
      }
    }
    [[nodiscard]] auto has_value() const -> bool { return !stack.empty(); }
    template <typename Type>
    [[nodiscard]] auto top_is() const -> bool {
      return has_value() && std::holds_alternative<Type>(top());
    }
    [[nodiscard]] auto size() const -> std::size_t { return stack.size(); }
    template <typename Type>
    void operator()(Type &&type) {
      return push(std::forward<Type>(type));
    }

  private:
    std::vector<ValueT> stack;
  };
} // namespace chimera::library::grammar::rules
