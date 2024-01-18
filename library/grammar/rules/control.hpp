//! tao::pegtl::normal that finds embeded rule transformations.
//! based on tao::pegtl::change_state.

#pragma once

#include <gsl/gsl>
#include <tao/pegtl.hpp>

#include <type_traits>

namespace chimera::library::grammar {
  template <typename Rule, typename = std::void_t<>>
  struct Normal : tao::pegtl::normal<Rule> {};
  template <typename Rule>
  struct Normal<Rule, std::void_t<decltype(typename Rule::Transform{})>>
      : tao::pegtl::normal<Rule> {
    using LocalControl = tao::pegtl::normal<Rule>;
    template <tao::pegtl::apply_mode A, tao::pegtl::rewind_mode M,
              template <typename...> class Action,
              template <typename...> class Control, typename Input,
              typename Outer, typename... Args>
    static auto match(Input &&input, Outer &&outer, Args &&...args) -> bool {
      if constexpr (A == tao::pegtl::apply_mode::action) {
        auto transaction = outer.transaction();
        typename Rule::Transform state;
        if (LocalControl::template match<A, M, Action, Control>(
                input, state, std::forward<Args>(args)...)) {
          state.finalize(state, std::forward<Outer>(outer));
          transaction.commit();
          return true;
        }
        return false;
      }
      if constexpr (A != tao::pegtl::apply_mode::action) {
        return LocalControl::template match<A, M, Action, Control>(
            input, std::forward<Outer>(outer), std::forward<Args>(args)...);
      }
      Expects(false);
    }
  };
} // namespace chimera::library::grammar
