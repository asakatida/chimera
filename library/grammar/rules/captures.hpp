//! generic control status transformations

#pragma once

#include "grammar/rules/set.hpp"
#include "grammar/rules/stack.hpp"

#include <optional>
#include <vector>

namespace chimera::library::grammar::rules {
  template <typename... Types>
  using UniqueStack = metal::apply<metal::lambda<Stack>, Set<Types...>>;
  template <typename Type>
  struct OptionalCapture : Stack<Type> {
    using LocalStack = Stack<Type>;
    template <typename Outer>
    void success(Outer &&outer) {
      if (LocalStack::has_value()) {
        std::visit(outer, LocalStack::pop());
      } else {
        outer.push(std::optional<Type>{});
      }
    }
  };
  template <typename Type>
  struct OptionalCapture<std::optional<Type>> : Stack<std::optional<Type>> {
    using LocalStack = Stack<std::optional<Type>>;
    template <typename Outer>
    void success(Outer &&outer) {
      if (LocalStack::has_value()) {
        std::visit(outer, LocalStack::pop());
      } else {
        outer.push(std::optional<Type>{});
      }
    }
  };
  template <typename Type, typename... Types>
  struct ReshapeCapture : UniqueStack<Types...> {
    using LocalStack = UniqueStack<Types...>;
    template <typename Outer>
    void success(Outer &&outer) {
      Ensures(LocalStack::size() == sizeof...(Types));
      outer.push(LocalStack::template reshape<Type, Types...>());
    }
  };
  template <typename... Types>
  struct VariantCapture : Stack<Types...> {
    using LocalStack = Stack<Types...>;
    template <typename Outer>
    void success(Outer &&outer) {
      std::visit(outer, LocalStack::pop());
    }
  };
  template <typename Type>
  struct VectorCapture : Stack<Type> {
    using LocalStack = Stack<Type>;
    template <typename Outer>
    void success(Outer &&outer) {
      std::vector<Type> vector;
      vector.reserve(LocalStack::size());
      LocalStack::template transform<Type>(std::back_inserter(vector));
      outer.push(std::move(vector));
    }
  };
} // namespace chimera::library::grammar::rules
