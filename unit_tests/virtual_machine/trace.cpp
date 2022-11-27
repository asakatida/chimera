#include "builtins/builtins.hpp"
#include "grammar/grammar.hpp"
#include "grammar/rules.hpp"
#include "grammar/rules/control.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/trace.hpp>

using namespace std::literals;

namespace chimera::library {
  struct EmptyNode {
    template <typename Stack>
    void success(Stack && /*stack*/) const {}
  };
  template <typename Data>
  void test_trace(Data &&data) {
    EmptyNode emptyNode;
    tao::pegtl::standard_trace<
        tao::pegtl::must<chimera::library::grammar::FileInput>,
        chimera::library::grammar::token::Action,
        typename chimera::library::grammar::MakeControl<>::Normal>(
        chimera::library::grammar::Input<tao::pegtl::memory_input<>>(
            data.data(), data.size(), "<unit_tests/virtual_machine/parse.cpp>"),
        emptyNode);
  }
} // namespace chimera::library

// TEST_CASE("virtual machine trace ``") {
//   REQUIRE_NOTHROW(chimera::library::test_trace(""sv));
// }
