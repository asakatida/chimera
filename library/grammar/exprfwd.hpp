//! parse definitions for code expressions.
//! forward declarations for format strings

#pragma once

#include "grammar/flags.hpp"

namespace chimera::library::grammar {
  template <flags::Flag Option>
  struct ConditionalExpression;
  template <flags::Flag Option>
  struct StarExpr;
  template <flags::Flag Option>
  struct YieldExpr;
} // namespace chimera::library::grammar
