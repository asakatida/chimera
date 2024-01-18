//! wrapper for tao::pegtl::parse

#include "asdl/asdl.hpp"       // for Expression
#include "grammar/grammar.hpp" // for EvalInput
#include "grammar/input.hpp"   // for Input
#include "grammar/parse.hpp"   // for parse

#include <iosfwd> // for istream

namespace chimera::library::options {
  enum class Optimize;
} // namespace chimera::library::options

namespace chimera::library::asdl {
  Expression::Expression(const options::Optimize &optimize, std::istream &input,
                         const char *source) {
    grammar::parse<grammar::EvalInput>(optimize, grammar::Input(input, source),
                                       *this);
  }
  [[nodiscard]] auto Expression::expr() const -> const ExprImpl & {
    return body;
  }
} // namespace chimera::library::asdl
