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
      static auto reshape(Iter &&iter,
                          std::index_sequence<I...> /*unused*/) -> Type {
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
      explicit Transaction(Stack *stack) : stack(stack), state(stack->stack) {}
      Transaction(const Transaction &) = delete;
      Transaction(Transaction &&) = delete;
      ~Transaction() noexcept {
        if (stack != nullptr) {
          stack->stack = std::move(state);
        }
      }
      auto operator=(const Transaction &) -> Transaction & = delete;
      auto operator=(Transaction &&) noexcept -> Transaction & = delete;
      void commit() noexcept { stack = nullptr; }

    private:
      Stack *stack;
      std::vector<ValueT> state;
    };
    [[nodiscard]] auto transaction() { return Transaction{this}; }
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
