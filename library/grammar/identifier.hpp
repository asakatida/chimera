//! parse definitions for identifier tokens.

#pragma once

#include "asdl/asdl.hpp"
#include "grammar/rules.hpp"
#include "grammar/utf8_id_continue.hpp"
#include "grammar/utf8_id_start.hpp"
#include "grammar/whitespace.hpp"

namespace chimera::library::grammar {
  namespace token {
    using XidStart = seq<Utf8IdStart>;
    using XidContinue = seq<Utf8IdContinue>;
    struct Name : seq<XidStart, star<XidContinue>> {};
    template <>
    struct Action<Name> {
      template <typename Input, typename Stack, typename... Args>
      static void apply(const Input &input, Stack &&stack, Args &&.../*args*/) {
        stack.push(asdl::Name{input.string()});
      }
    };
  } // namespace token
  template <flags::Flag Option>
  struct Name : token::Token<Option, token::Name> {};
} // namespace chimera::library::grammar
