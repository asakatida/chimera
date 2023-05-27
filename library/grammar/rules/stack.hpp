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

#include <gsl/gsl>

#include <algorithm>
#include <numeric>
#include <tuple>
#include <variant>
#include <vector>

namespace chimera::library::grammar::rules {
  template <typename... Types>
  struct Stack {
    using List = metal::list<Types...>;
    using ValueT = std::variant<Types...>;
    void operator()(ValueT &&value) { return push(std::move(value)); }
    void clear() noexcept { stack.clear(); }
    template <typename Self, typename... Args>
    void finalize(Self &&self, Args &&...args) {
      self.success(std::forward<Args>(args)...);
      Ensures(stack.empty());
    }
    [[nodiscard]] auto has_value() const -> bool { return !stack.empty(); }
    [[nodiscard]] auto pop() -> ValueT {
      auto finally = gsl::finally([this] { this->stack.pop_back(); });
      return std::move(top());
    }
    template <typename Type,
              typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
    [[nodiscard]] auto pop() -> Type {
      auto finally = gsl::finally([this] { this->stack.pop_back(); });
      return std::move(top<Type>());
    }
    void push(ValueT &&value) { stack.emplace_back(std::move(value)); }
    template <typename Base, typename Visitor>
    [[nodiscard]] auto reduce(Base &&base, Visitor &&visitor) -> Base {
      auto finally = gsl::finally([this] { clear(); });
      return std::reduce(stack.begin(), stack.end(), std::forward<Base>(base),
                         std::forward<Visitor>(visitor));
    };
    template <typename Type, typename... Args>
    struct Reshape {
      template <typename Iter, std::size_t... I>
      static auto reshape(Iter &&iter, std::index_sequence<I...> /*unused*/)
          -> Type {
        Ensures(std::holds_alternative<Args>(*(iter + I)) && ...);
        return Type{std::move(std::get<Args>(*(iter + I)))...};
      }
    };
    template <typename Type, typename... Args>
    [[nodiscard]] auto reshape() -> Type {
      using LocalStack = Reshape<Type, Args...>;
      auto finally = gsl::finally([this] { clear(); });
      Ensures(size() == sizeof...(Args));
      return LocalStack::reshape(stack.begin(),
                                 std::index_sequence_for<Args...>{});
    }
    [[nodiscard]] auto size() const -> std::size_t { return stack.size(); }
    [[nodiscard]] auto top() -> ValueT & { return stack.back(); }
    template <typename Type,
              typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
    [[nodiscard]] auto top() -> Type & {
      Ensures(std::holds_alternative<Type>(top()));
      return std::get<Type>(top());
    }
    template <typename Type,
              typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
    [[nodiscard]] auto top_is() const -> bool {
      return has_value() && std::holds_alternative<Type>(stack.back());
    }
    struct Transaction {
      Transaction(Stack *stack) : stack(stack), state(stack->stack) {}
      ~Transaction() noexcept {
        if (stack != nullptr) {
          stack->stack = std::move(state);
        }
      }
      void commit() noexcept { stack = nullptr; }

    private:
      Stack *stack;
      std::vector<ValueT> state;
    };
    [[nodiscard]] auto transaction() -> Transaction { return {this}; }
    template <typename OutputIt>
    void transform(OutputIt &&outputIt) {
      using IteratorTraits = std::iterator_traits<OutputIt>;
      transform<typename IteratorTraits::value_type, OutputIt>(
          std::forward<OutputIt>(outputIt));
    }
    template <typename Type, typename OutputIt,
              typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
    void transform(OutputIt &&outputIt) {
      auto finally = gsl::finally([this] { clear(); });
      for (auto &value : stack) {
        Ensures(std::holds_alternative<Type>(value));
        *(outputIt++) = std::get<Type>(std::move(value));
      }
    }

  private:
    friend Transaction;
    std::vector<ValueT> stack;
  };
} // namespace chimera::library::grammar::rules
