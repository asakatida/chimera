#include "grammar/grammar.hpp"
#include "grammar/input.hpp"
#include "grammar/rules.hpp"
#include "grammar/rules/control.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/trace.hpp>

#include <sstream>

using namespace std::literals;

namespace chimera::library {
  struct EmptyNode : asdl::BaseASDL {
    template <typename Stack>
    void finalize(const EmptyNode & /*unused*/, Stack && /*unused*/) const {}
  };
  template <typename Data>
  void test_trace(Data &&data) {
    EmptyNode emptyNode;
    tao::pegtl::standard_trace<
        tao::pegtl::must<chimera::library::grammar::FileInput>,
        chimera::library::grammar::token::Action,
        typename chimera::library::grammar::MakeControl<>::Normal>(
        chimera::library::grammar::Input(
            std::forward<Data>(data), "<unit_tests/virtual_machine/parse.cpp>"),
        emptyNode);
  }
} // namespace chimera::library

// TEST_CASE("virtual machine trace ``") {
//   REQUIRE_NOTHROW(chimera::library::test_trace(""sv));
// }
