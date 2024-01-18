//! tao::pegtl::* wrappers to control stack specialization

#pragma once

#include "grammar/rules/action.hpp"
#include "grammar/rules/captures.hpp"
#include "grammar/rules/control.hpp"

#include <metal.hpp>
#include <tao/pegtl.hpp>

namespace chimera::library::grammar::rules {
  struct SyntaxError : virtual public std::exception {};
  struct ExprAssignError final : virtual public SyntaxError {
    [[nodiscard]] auto what() const noexcept -> const char * final {
      return "SyntaxError: cannot assign to expression here. Maybe you meant "
             "'==' instead of '='?";
    }
  };
  struct BytesASCIIOnlyError final : virtual public SyntaxError {
    [[nodiscard]] auto what() const noexcept -> const char * final {
      return "SyntaxError: bytes can only contain ASCII literal characters";
    }
  };
  template <typename... Types>
  struct Tuple {};
  template <typename... Types>
  struct Variant {};
  template <typename Type>
  struct Vector {};
  using tao::pegtl::opt;
  using tao::pegtl::rep;
  using tao::pegtl::seq;
  using tao::pegtl::sor;
  template <typename Rule>
  struct ASTType {
    using Type = Tuple<>;
  };
  template <typename... Rules>
  struct ASTType<opt<Rules...>> {
    using Type = Variant<Tuple<>, Tuple<>>;
  };
  template <unsigned Repeat, typename... Rules>
  struct ASTType<rep<Repeat, Rules...>> {
    using Type = Tuple<>;
  };
  template <typename... Rules>
  struct ASTType<seq<Rules...>> {
    using Type = Tuple<>;
  };
  template <typename... Rules>
  struct ASTType<sor<Rules...>> {
    using Type = Variant<>;
  };
} // namespace chimera::library::grammar::rules
namespace chimera::library::grammar {
  using rules::ExprAssignError;
  using rules::SyntaxError;
} // namespace chimera::library::grammar
