//! wrapper for tao::pegtl::parse

#include "asdl/asdl.hpp"       // for Module
#include "grammar/grammar.hpp" // for FileInput
#include "grammar/input.hpp"   // for Input
#include "grammar/parse.hpp"   // for parse

#include <iosfwd> // for istream
#include <vector> // for vector

namespace chimera::library::options {
  enum class Optimize;
} // namespace chimera::library::options

namespace chimera::library::asdl {
  Module::Module(const options::Optimize &optimize, std::istream &input,
                 const char *source) {
    grammar::parse<grammar::FileInput>(optimize, grammar::Input(input, source),
                                       *this);
  }
  [[nodiscard]] auto Module::doc() const -> const std::optional<DocString> & {
    return doc_string;
  }
  [[nodiscard]] auto Module::iter() const -> const std::vector<StmtImpl> & {
    return body;
  }
} // namespace chimera::library::asdl
