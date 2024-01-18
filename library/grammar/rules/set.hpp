//! metal::list of unique types

#pragma once

#include <metal.hpp>

namespace chimera::library::grammar::rules {
  template <typename List, typename New>
  using Unique = metal::if_<
      metal::apply<metal::lambda<metal::distinct>, metal::append<List, New>>,
      metal::append<List, New>, List>;
  template <typename... Types>
  using Set = metal::fold_left<metal::lambda<Unique>, metal::list<>, Types...>;
} // namespace chimera::library::grammar::rules
