#include "virtual_machine/global_context.hpp"
#include "virtual_machine/thread_context.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

using namespace std::literals;

namespace chimera::library::virtual_machine {
  void parse_file(std::string_view &&data) {
    const Options options{.chimera = "chimera",
                          .exec = options::Script{"test.py"}};
    GlobalContext globalContext(options);
    ProcessContext processContext{globalContext};
    std::istringstream istream{std::string{data}};
    auto module = processContext.parse_file(std::move(istream), "<test>");
    ThreadContext threadContext{processContext,
                                processContext.make_module("__main__")};
    threadContext.evaluate(module);
  }
} // namespace chimera::library::virtual_machine

TEST_CASE("grammar VirtualMachine ``") {
  SECTION("destructor") {
    REQUIRE_NOTHROW(chimera::library::virtual_machine::parse_file(""sv));
  }
  REQUIRE(true);
}

TEST_CASE("grammar VirtualMachine `type`") {
  REQUIRE_THROWS_AS(chimera::library::virtual_machine::parse_file("type"sv),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `a@b=c`") {
  REQUIRE_THROWS_AS(chimera::library::virtual_machine::parse_file("a@b=c"sv),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `0x10 & 0x01`") {
  REQUIRE_THROWS_AS(
      chimera::library::virtual_machine::parse_file("0x10 & 0x01"sv),
      chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `()`") {
  REQUIRE_NOTHROW(chimera::library::virtual_machine::parse_file("()"sv));
}

TEST_CASE("grammar VirtualMachine `(' ',)`") {
  REQUIRE_NOTHROW(chimera::library::virtual_machine::parse_file("(' ',)"sv));
}

TEST_CASE("grammar VirtualMachine `(' ', None)`") {
  REQUIRE_NOTHROW(
      chimera::library::virtual_machine::parse_file("(' ', None)"sv));
}

TEST_CASE("grammar VirtualMachine `(' ', None, )`") {
  REQUIRE_NOTHROW(
      chimera::library::virtual_machine::parse_file("(' ', None, )"sv));
}

TEST_CASE("grammar VirtualMachine `'\\xffff'`") {
  REQUIRE_NOTHROW(
      chimera::library::virtual_machine::parse_file(R"('\xffff')"sv));
}

TEST_CASE("grammar VirtualMachine `'\\77'`") {
  REQUIRE_NOTHROW(chimera::library::virtual_machine::parse_file(R"('\77')"sv));
}

TEST_CASE("grammar VirtualMachine `raise`") {
  REQUIRE_THROWS_AS(chimera::library::virtual_machine::parse_file("raise"sv),
                    chimera::library::object::BaseException);
}

TEST_CASE("grammar VirtualMachine `[None,None][None,0]`") {
  REQUIRE_THROWS_AS(
      chimera::library::virtual_machine::parse_file("[None,None][None,0]"sv),
      chimera::library::object::BaseException);
}
