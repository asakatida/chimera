#include "virtual_machine/global_context.hpp"

#include <catch2/catch_test_macros.hpp>

#include "builtins/builtins.hpp"
#include "virtual_machine/thread_context.hpp"

using namespace std::literals;

TEST_CASE("grammar VirtualMachine ``") {
  SECTION("destructor") {
    const chimera::library::Options options{.chimera = "chimera",
                                            .script = "test.py"};
    chimera::library::virtual_machine::GlobalContext globalContext(options);
    chimera::library::virtual_machine::ProcessContext processContext{
        globalContext};
    auto module = processContext.parse_file(""sv, "<test>");
    chimera::library::virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("__main__")};
    REQUIRE_NOTHROW(threadContext.evaluate(module));
  }
  REQUIRE(true);
}

TEST_CASE("grammar VirtualMachine `type`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("type"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_THROWS_AS(threadContext.evaluate(module),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `a@b=c`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("a@b=c"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_THROWS_AS(threadContext.evaluate(module),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `0x10 & 0x01`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("0x10 & 0x01"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_THROWS_AS(threadContext.evaluate(module),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `()`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("()"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_NOTHROW(threadContext.evaluate(module));
}

TEST_CASE("grammar VirtualMachine `(' ',)`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("(' ',)"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_NOTHROW(threadContext.evaluate(module));
}

TEST_CASE("grammar VirtualMachine `(' ', None)`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("(' ', None)"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_NOTHROW(threadContext.evaluate(module));
}

TEST_CASE("grammar VirtualMachine `(' ', None, )`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("(' ', None, )"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_NOTHROW(threadContext.evaluate(module));
}

TEST_CASE("grammar VirtualMachine `'\\xffff'`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file(R"('\xffff')"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_NOTHROW(threadContext.evaluate(module));
}

TEST_CASE("grammar VirtualMachine `'\\77'`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file(R"('\77')"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_NOTHROW(threadContext.evaluate(module));
}

TEST_CASE("grammar VirtualMachine `raise`") {
  const chimera::library::Options options{.chimera = "chimera",
                                          .script = "test.py"};
  chimera::library::virtual_machine::GlobalContext globalContext(options);
  chimera::library::virtual_machine::ProcessContext processContext{
      globalContext};
  auto module = processContext.parse_file("raise"sv, "<test>");
  chimera::library::virtual_machine::ThreadContext threadContext{
      processContext, processContext.make_module("__main__")};
  REQUIRE_THROWS_AS(threadContext.evaluate(module),
                    chimera::library::object::BaseException);
}
