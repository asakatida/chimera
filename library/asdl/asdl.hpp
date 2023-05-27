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

//! abstract syntax structures

#pragma once

#include "object/object.hpp" // for Object

#include <metal/lambda/apply.hpp>  // for apply
#include <metal/list/contains.hpp> // for contains
#include <metal/list/list.hpp>     // for list

#include <algorithm>   // for reverse
#include <iosfwd>      // for istream, string
#include <iterator>    // for back_inserter
#include <memory>      // for make_shared, shared_ptr
#include <optional>    // for optional
#include <string>      // for basic_string
#include <type_traits> // for enable_if_t
#include <utility>     // for forward, move
#include <variant>     // for get_if, variant, visit
#include <vector>      // for vector

namespace metal {
  template <template <class...> class expr>
  struct lambda;
} // namespace metal

namespace chimera::library {
  enum class Optimize;
  namespace asdl {
    struct ExtSlice;
  } // namespace asdl
} // namespace chimera::library

namespace chimera::library::asdl {
  namespace detail {
    template <typename... Types>
    struct Impl {
      using List = metal::list<Types...>;
      using ValueT = metal::apply<metal::lambda<std::variant>, List>;
      Impl() = default;
      template <typename Type,
                typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
      // NOLINTNEXTLINE(hicpp-explicit-conversions)
      Impl(Type &&type)
          : value(std::make_shared<ValueT>(ValueT(std::forward<Type>(type)))) {}
      template <typename Type,
                typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
      auto operator=(Type &&type) -> Impl & {
        value = std::make_shared<ValueT>(std::forward<Type>(type));
        return *this;
      }
      template <typename Type,
                typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
      [[nodiscard]] auto get() const noexcept -> std::optional<const Type> {
        if (auto *asdl = std::get_if<Type>(value.get()); asdl != nullptr) {
          return *asdl;
        }
        return {};
      }
      template <typename Type, typename Visitor,
                typename = std::enable_if_t<metal::contains<List, Type>() != 0>>
      [[nodiscard]] auto update_if(Visitor &&visitor) -> bool {
        if (auto *asdl = std::get_if<Type>(value.get()); asdl != nullptr) {
          *value = visitor(*asdl);
          return true;
        }
        return false;
      }
      template <typename Visitor>
      void visit(Visitor &&visitor) const {
        if (value) {
          std::visit(std::forward<Visitor>(visitor), *value);
        }
      }

    private:
      std::shared_ptr<ValueT> value;
    };
  } // namespace detail
  using ExprImpl =
      detail::Impl<struct Attribute, struct Await, struct Bin, struct Bool,
                   struct Call, struct Compare, struct Dict, struct DictComp,
                   struct Ellipsis, struct FormattedValue, struct GeneratorExp,
                   struct IfExp, struct JoinedStr, struct Lambda, struct List,
                   struct ListComp, struct Name, struct NameConstant,
                   object::Object, struct Set, struct SetComp, struct Starred,
                   struct Subscript, struct Tuple, struct Unary,
                   struct UnpackDict, struct Yield, struct YieldFrom>;
  struct Name {
    std::string value;
  };
  struct ModuleName {
    std::string value;
  };
  struct DocString {
    object::Object string;
  };
  struct Withitem {
    ExprImpl context_expr;
    std::optional<ExprImpl> optional_vars{};
  };
  struct Alias {
    Name name;
    std::optional<Name> asname{};
  };
  struct Keyword {
    std::optional<Name> arg{};
    ExprImpl value;
  };
  struct Arg {
    Name name;
    std::optional<ExprImpl> annotation{};
    std::optional<ExprImpl> arg_default{};
  };
  struct Arguments {
    std::vector<Arg> args{};
    std::optional<Arg> vararg{};
    std::vector<Arg> kwonlyargs{};
    std::optional<Arg> kwarg{};
  };
  struct Comprehension {
    ExprImpl target;
    ExprImpl iter;
    std::vector<ExprImpl> ifs{};
    bool is_async = false;
  };
  enum class Operator {
    ADD,
    SUB,
    MULT,
    MAT_MULT,
    DIV,
    MOD,
    POW,
    L_SHIFT,
    R_SHIFT,
    BIT_OR,
    BIT_XOR,
    BIT_AND,
    FLOOR_DIV,
  };
  struct Slice {
    std::optional<ExprImpl> lower{};
    std::optional<ExprImpl> upper{};
    std::optional<ExprImpl> step{};
  };
  struct Index {
    ExprImpl value;
  };
  using SliceImpl = detail::Impl<ExtSlice, Index, Slice>;
  struct ExtSlice {
    std::vector<SliceImpl> dims{};
  };
  struct Bool {
    enum Op {
      AND,
      OR,
    };
    Op op = AND;
    std::vector<ExprImpl> values{};
  };
  struct Bin {
    Operator op{};
    std::vector<ExprImpl> values{};
  };
  struct Unary {
    enum Op {
      BIT_NOT,
      NOT,
      ADD,
      SUB,
    };
    Op op = BIT_NOT;
    ExprImpl operand;
  };
  struct Lambda {
    Arguments args;
    ExprImpl body;
  };
  struct IfExp {
    ExprImpl test;
    ExprImpl body;
    ExprImpl orelse;
  };
  struct UnpackDict {};
  struct Dict {
    std::vector<ExprImpl> keys{};
    std::vector<ExprImpl> values{};
  };
  struct Set {
    std::vector<ExprImpl> elts{};
  };
  struct ListComp {
    ExprImpl elt;
    std::vector<Comprehension> generators{};
  };
  struct SetComp {
    ExprImpl elt;
    std::vector<Comprehension> generators{};
  };
  struct DictComp {
    ExprImpl key;
    ExprImpl value;
    std::vector<Comprehension> generators{};
  };
  struct GeneratorExp {
    ExprImpl elt;
    std::vector<Comprehension> generators{};
  };
  struct Await {
    ExprImpl value;
  };
  struct Yield {
    std::optional<ExprImpl> value{};
  };
  struct YieldFrom {
    ExprImpl value;
  };
  struct CompareExpr {
    enum Op {
      EQ,
      NOT_EQ,
      LT,
      LT_E,
      GT,
      GT_E,
      IS,
      IS_NOT,
      IN,
      NOT_IN,
    };
    Op op = EQ;
    ExprImpl value;
  };
  struct Compare {
    ExprImpl left;
    std::vector<CompareExpr> comparators{};
  };
  struct Call {
    ExprImpl func;
    std::vector<ExprImpl> args{};
    std::vector<Keyword> keywords{};
  };
  struct FormattedValue {
    ExprImpl value;
    enum Conversion {
      ASCII,
      REPR,
      STR,
    };
    Conversion conversion = ASCII;
    std::optional<ExprImpl> format_spec{};
  };
  struct JoinedStr {
    std::vector<ExprImpl> values{};
  };
  struct NameConstant {
    enum {
      FALSE,
      NONE,
      TRUE,
    } value = FALSE;
  };
  struct Ellipsis {};
  struct Attribute {
    ExprImpl value;
    Name attr;
  };
  struct Subscript {
    ExprImpl value;
    SliceImpl slice;
  };
  struct Starred {
    ExprImpl value;
  };
  struct List {
    std::vector<ExprImpl> elts{};
  };
  struct Tuple {
    std::vector<ExprImpl> elts{};
  };
  using StmtImpl =
      detail::Impl<struct AnnAssign, struct Assert, struct Assign,
                   struct AsyncFor, struct AsyncFunctionDef, struct AsyncWith,
                   struct AugAssign, struct Break, struct ClassDef,
                   struct Continue, struct Delete, struct Expr, struct For,
                   struct FunctionDef, struct Global, struct If, struct Import,
                   struct ImportFrom, struct Nonlocal, struct Raise,
                   struct Return, struct Try, struct While, struct With>;
  struct Break {};
  struct Continue {};
  struct Expr {
    ExprImpl value;
  };
  struct Nonlocal {
    std::vector<Name> names{};
  };
  struct Global {
    std::vector<Name> names{};
  };
  struct ImportFrom {
    ModuleName module;
    std::vector<Alias> names{};
  };
  struct Import {
    std::vector<Alias> names{};
  };
  struct Assert {
    ExprImpl test;
    std::optional<ExprImpl> msg{};
  };
  struct ExceptHandler {
    std::optional<ExprImpl> type{};
    std::optional<Name> name{};
    std::vector<StmtImpl> body{};
  };
  using Excepthandler = std::variant<ExceptHandler>;
  struct Try {
    std::vector<StmtImpl> body{};
    std::vector<Excepthandler> handlers{};
    std::vector<StmtImpl> orelse{};
    std::vector<StmtImpl> finalbody{};
  };
  struct Raise {
    std::optional<ExprImpl> exc{};
    std::optional<ExprImpl> cause{};
  };
  struct AsyncWith {
    std::vector<Withitem> items{};
    std::vector<StmtImpl> body{};
  };
  struct With {
    std::vector<Withitem> items{};
    std::vector<StmtImpl> body{};
  };
  struct IfBranch {
    ExprImpl test;
    std::vector<StmtImpl> body{};
  };
  struct If {
    std::vector<IfBranch> body{};
    std::vector<StmtImpl> orelse{};
  };
  struct While {
    ExprImpl test;
    std::vector<StmtImpl> body{};
    std::vector<StmtImpl> orelse{};
  };
  struct AsyncFor {
    ExprImpl target;
    ExprImpl iter;
    std::vector<StmtImpl> body{};
    std::vector<StmtImpl> orelse{};
  };
  struct For {
    ExprImpl target;
    ExprImpl iter;
    std::vector<StmtImpl> body{};
    std::vector<StmtImpl> orelse{};
  };
  struct AugAssign {
    ExprImpl target;
    Operator op = Operator::ADD;
    ExprImpl value;
  };
  struct AnnAssign {
    ExprImpl target;
    ExprImpl annotation;
    std::optional<ExprImpl> value{};
    bool simple{true};
  };
  struct Assign {
    std::vector<ExprImpl> targets{};
    ExprImpl value;
  };
  struct Delete {
    std::vector<ExprImpl> targets{};
  };
  struct Return {
    std::optional<ExprImpl> value{};
  };
  struct ClassDef {
    Name name;
    std::optional<DocString> doc_string{};
    std::vector<ExprImpl> bases{};
    std::vector<Keyword> keywords{};
    std::vector<StmtImpl> body{};
    std::vector<ExprImpl> decorator_list{};
  };
  struct AsyncFunctionDef {
    Name name;
    std::optional<DocString> doc_string{};
    Arguments args;
    std::vector<StmtImpl> body{};
    std::vector<ExprImpl> decorator_list{};
    std::optional<ExprImpl> returns{};
  };
  struct FunctionDef {
    Name name;
    std::optional<DocString> doc_string{};
    Arguments args;
    std::vector<StmtImpl> body{};
    std::vector<ExprImpl> decorator_list{};
    std::optional<ExprImpl> returns{};
  };
  struct BaseASDL {
    struct NullTransaction {
      static void commit() noexcept {}
    };
    [[nodiscard]] static auto transaction() noexcept -> NullTransaction {
      return {};
    }
  };
  struct Module : BaseASDL {
    Module() = default;
    Module(const Optimize &optimize, std::istream &&input, const char *source);
    [[nodiscard]] auto doc() const -> const std::optional<DocString> &;
    [[nodiscard]] auto iter() const -> const std::vector<StmtImpl> &;
    template <typename Stack>
    void finalize(const Module & /*unused*/, Stack &&stack) {
      body.reserve(stack.size());
      while (stack.template top_is<StmtImpl>()) {
        body.emplace_back(stack.template pop<StmtImpl>());
      }
      std::reverse(body.begin(), body.end());
      if (stack.has_value()) {
        doc_string = stack.template pop<DocString>();
      }
      Ensures(stack.size() == 0);
    }

  private:
    std::vector<StmtImpl> body{};
    std::optional<DocString> doc_string{};
  };
  struct Interactive : BaseASDL {
    Interactive() = default;
    Interactive(const Optimize &optimize, std::istream &&input,
                const char *source);
    [[nodiscard]] auto iter() const -> const std::vector<StmtImpl> &;
    template <typename Stack>
    void finalize(const Interactive & /*unused*/, Stack &&stack) {
      body.reserve(stack.size());
      stack.template transform<StmtImpl>(std::back_inserter(body));
    }

  private:
    std::vector<StmtImpl> body{};
  };
  struct Expression : BaseASDL {
    Expression() = default;
    Expression(const Optimize &optimize, std::istream &&input,
               const char *source);
    [[nodiscard]] auto expr() const -> const ExprImpl &;
    template <typename Stack>
    void finalize(const Expression & /*unused*/, Stack &&stack) {
      if (auto size = stack.size(); size > 1) {
        Tuple tuple;
        tuple.elts.reserve(size);
        stack.template transform<ExprImpl>(std::back_inserter(tuple.elts));
        body = std::move(tuple);
      } else {
        body = stack.template pop<ExprImpl>();
      }
    }

  private:
    ExprImpl body;
  };
} // namespace chimera::library::asdl
