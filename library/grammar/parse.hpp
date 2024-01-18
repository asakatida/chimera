//! wrapper for tao::pegtl::parse

#pragma once

#include "grammar/rules.hpp"
#include "grammar/rules/control.hpp"
#include "options.hpp"

#include <gsl/gsl>
#include <tao/pegtl.hpp>

namespace chimera::library::grammar {
  template <typename Grammar, typename... Args>
  void parse(const options::Optimize &optimize, Args &&...args) {
    Ensures((tao::pegtl::parse<tao::pegtl::must<Grammar>, token::Action, Normal,
                               tao::pegtl::apply_mode::action,
                               tao::pegtl::rewind_mode::required>(args...)));
    switch (optimize) {
      case options::Optimize::NONE:
      case options::Optimize::BASIC:
      case options::Optimize::DISCARD_DOCS:
        break;
    }
  }
} // namespace chimera::library::grammar
