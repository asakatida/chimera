//! expose ::rules::* for easy use.

#pragma once

#include "grammar/rules/rules.hpp"

#include <tao/pegtl.hpp>

namespace chimera::library::grammar {
  using tao::pegtl::at;
  using tao::pegtl::discard;
  using tao::pegtl::eof;
  using tao::pegtl::failure;
  using tao::pegtl::if_must;
  using tao::pegtl::if_then_else;
  using tao::pegtl::list;
  using tao::pegtl::list_must;
  using tao::pegtl::list_tail;
  using tao::pegtl::minus;
  using tao::pegtl::not_at;
  using tao::pegtl::opt;
  using tao::pegtl::opt_must;
  using tao::pegtl::plus;
  using tao::pegtl::rep;
  using tao::pegtl::rep_opt;
  using tao::pegtl::seq;
  using tao::pegtl::sor;
  using tao::pegtl::star;
  using tao::pegtl::success;
  using tao::pegtl::until;
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  using tao::pegtl::ascii::any;
  using tao::pegtl::ascii::not_one;
  using tao::pegtl::ascii::not_range;
  using tao::pegtl::ascii::one;
  using tao::pegtl::ascii::range;
  using tao::pegtl::ascii::ranges;
#else
  using tao::pegtl::utf8::any;
  using tao::pegtl::utf8::not_one;
  using tao::pegtl::utf8::not_range;
  using tao::pegtl::utf8::one;
  using tao::pegtl::utf8::range;
  using tao::pegtl::utf8::ranges;
#endif
} // namespace chimera::library::grammar
