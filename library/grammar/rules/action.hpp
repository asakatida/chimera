//! Expose tao::pegtl::nothing in namespace for use with typename Rule::Type.

#pragma once

#include <tao/pegtl.hpp>

namespace chimera::library::grammar::token {
  template <typename Rule>
  struct Action : tao::pegtl::nothing<Rule> {};
} // namespace chimera::library::grammar::token
