#include "virtual_machine/global_context.hpp"

#include <catch2/catch_test_macros.hpp>

#include "builtins/builtins.hpp"
#include "virtual_machine/thread_context.hpp"

using namespace std::literals;

TEST_CASE("grammar VirtualMachine ``") {
  SECTION("destructor") {
    chimera::library::Options options;
    options.chimera = "chimera";
    options.script = "test.py";
    const chimera::library::virtual_machine::GlobalContext globalContext(
        options);
    chimera::library::virtual_machine::ProcessContext processContext{
        globalContext};
    auto module = processContext.parse_file(""sv, "<test>");
    chimera::library::virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("__main__")};
    REQUIRE_THROWS_AS(threadContext.evaluate(module),
                      chimera::library::object::AttributeError);
  }
  REQUIRE(true);
}

TEST_CASE("grammar VirtualMachine `type`") {
  chimera::library::Options options;
  options.chimera = "chimera";
  options.script = "test.py";
  const chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("type"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_THROWS_AS(threadContext.evaluate(module),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `a@b=c`") {
  chimera::library::Options options;
  options.chimera = "chimera";
  options.script = "test.py";
  const chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("a@b=c"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_THROWS_AS(threadContext.evaluate(module),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `0x10 & 0x01`") {
  chimera::library::Options options{.chimera = "chimera", .script = "test.py"};
  const chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("0x10 & 0x01"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_THROWS_AS(threadContext.evaluate(module),
                    chimera::library::object::BaseException);
}

// TEST_CASE("grammar VirtualMachine `()`") {
//   chimera::library::Options options{chimera : "chimera", script : "test.py"};
//   const chimera::library::virtual_machine::GlobalContext
//   globalContext(options); chimera::library::virtual_machine::ProcessContext
//   processContext{
//       globalContext};
//   auto module = processContext.parse_file("()"sv, "<test>");
//   chimera::library::virtual_machine::ThreadContext threadContext{
//       processContext, processContext.make_module("__main__")};
//   REQUIRE_THROWS_AS(threadContext.evaluate(module),
//                     chimera::library::object::BaseException);
// }

// TEST_CASE("grammar VirtualMachine `(' ',)`") {
//   chimera::library::Options options;
//   options.chimera = "chimera";
//   options.script = "test.py";
//   const chimera::library::virtual_machine::GlobalContext
//   globalContext(options); chimera::library::virtual_machine::ProcessContext
//   processContext{
//       globalContext};
//   auto module = processContext.parse_file("`' ',`"sv, "<test>");
//   chimera::library::virtual_machine::ThreadContext threadContext{
//       processContext, processContext.make_module("__main__")};
//   REQUIRE_THROWS_AS(threadContext.evaluate(module),
//                     chimera::library::object::BaseException);
// }

// TEST_CASE("grammar VirtualMachine `(' ', None)`") {
//   chimera::library::Options options;
//   options.chimera = "chimera";
//   options.script = "test.py";
//   const chimera::library::virtual_machine::GlobalContext
//   globalContext(options); chimera::library::virtual_machine::ProcessContext
//   processContext{
//       globalContext};
//   auto module = processContext.parse_file("`' ', None`"sv, "<test>");
//   chimera::library::virtual_machine::ThreadContext threadContext{
//       processContext, processContext.make_module("__main__")};
//   REQUIRE_THROWS_AS(threadContext.evaluate(module),
//                     chimera::library::object::BaseException);
// }

// TEST_CASE("grammar VirtualMachine `(' ', None, )`") {
//   chimera::library::Options options;
//   options.chimera = "chimera";
//   options.script = "test.py";
//   const chimera::library::virtual_machine::GlobalContext
//   globalContext(options); chimera::library::virtual_machine::ProcessContext
//   processContext{
//       globalContext};
//   auto module = processContext.parse_file("`' ', None, `"sv, "<test>");
//   chimera::library::virtual_machine::ThreadContext threadContext{
//       processContext, processContext.make_module("__main__")};
//   REQUIRE_THROWS_AS(threadContext.evaluate(module),
//                     chimera::library::object::BaseException);
// }
