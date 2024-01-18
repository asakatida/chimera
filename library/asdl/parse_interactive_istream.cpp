//! wrapper for tao::pegtl::parse

#include "asdl/asdl.hpp"       // for Interactive
#include "grammar/grammar.hpp" // for SingleInput
#include "grammar/input.hpp"   // for Input
#include "grammar/parse.hpp"   // for parse

#include <iosfwd> // for istream
#include <vector> // for vector

namespace chimera::library::options {
  enum class Optimize;
} // namespace chimera::library::options

namespace chimera::library::asdl {
  Interactive::Interactive(const options::Optimize &optimize,
                           std::istream &input, const char *source) {
    grammar::parse<grammar::SingleInput>(optimize,
                                         grammar::Input(input, source), *this);
  }
  [[nodiscard]] auto Interactive::iter() const
      -> const std::vector<StmtImpl> & {
    return body;
  }
} // namespace chimera::library::asdl
