#include "fuzz/cases.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "builtins/builtins.hpp"
#include "grammar/number.hpp"
#include "grammar/rules.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"
#include "virtual_machine/thread_context.hpp"

namespace chimera::library {
  using NumericLimits = std::numeric_limits<std::uint16_t>;
  constexpr static auto bufferSize = NumericLimits::max();
  [[nodiscard]] auto fuzz_input(std::istream &input) -> Input {
    return {input, bufferSize, "fuzz"};
  }
  [[nodiscard]] auto fuzz_istream(const std::uint8_t *data, std::size_t size)
      -> std::istringstream {
    std::istringstream input(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        std::string(reinterpret_cast<const char *>(data), size));
    return input;
  }
  [[nodiscard]] auto fuzz_options() -> Options {
    Options options{};
    options.chimera = "chimera";
    options.script = "fuzzer.py";
    return options;
  }
  [[nodiscard]] auto fuzz_expression_eval(const std::uint8_t *data,
                                          std::size_t size) -> int {
    return fuzz_expression_eval(fuzz_istream(data, size));
  }
  [[nodiscard]] auto fuzz_expression_eval(std::istream &&input) -> int {
    auto options = fuzz_options();
    virtual_machine::GlobalContext globalContext(options);
    virtual_machine::ProcessContext processContext{globalContext};
    std::optional<asdl::Expression> expression;
    try {
      expression = processContext.parse_expression(std::move(input), "<fuzz>");
    } catch (const tao::pegtl::parse_error &) {
      return -1;
    } catch (const chimera::library::grammar::SyntaxError &) {
      return -1;
    }
    Ensures(expression.has_value());
    try {
      auto main = processContext.make_module("__main__");
      virtual_machine::ThreadContext threadContext{processContext, main};
      threadContext.evaluate(*expression);
    } catch (const object::BaseException &) {
      return -1;
    }
    return 0;
  }
  [[nodiscard]] auto fuzz_file_eval(const std::uint8_t *data, std::size_t size)
      -> int {
    return fuzz_file_eval(fuzz_istream(data, size));
  }
  [[nodiscard]] auto fuzz_file_eval(std::istream &&input) -> int {
    auto options = fuzz_options();
    virtual_machine::GlobalContext globalContext(options);
    virtual_machine::ProcessContext processContext{globalContext};
    std::optional<asdl::Module> module;
    try {
      module = processContext.parse_file(std::move(input), "<fuzz>");
    } catch (const tao::pegtl::parse_error &) {
      return -1;
    } catch (const chimera::library::grammar::SyntaxError &) {
      return -1;
    }
    Ensures(module.has_value());
    try {
      auto main = processContext.make_module("__main__");
      virtual_machine::ThreadContext threadContext{processContext, main};
      threadContext.evaluate(*module);
    } catch (const object::BaseException &) {
      return -1;
    }
    return 0;
  }
  [[nodiscard]] auto fuzz_interactive_eval(const std::uint8_t *data,
                                           std::size_t size) -> int {
    return fuzz_interactive_eval(fuzz_istream(data, size));
  }
  [[nodiscard]] auto fuzz_interactive_eval(std::istream &&input) -> int {
    auto options = fuzz_options();
    virtual_machine::GlobalContext globalContext(options);
    virtual_machine::ProcessContext processContext{globalContext};
    std::optional<asdl::Interactive> interactive;
    try {
      interactive = processContext.parse_input(std::move(input), "<fuzz>");
    } catch (const tao::pegtl::parse_error &) {
      return -1;
    } catch (const chimera::library::grammar::SyntaxError &) {
      return -1;
    }
    Ensures(interactive.has_value());
    try {
      auto main = processContext.make_module("__main__");
      virtual_machine::ThreadContext threadContext{processContext, main};
      threadContext.evaluate(*interactive);
    } catch (const object::BaseException &) {
      return -1;
    }
    return 0;
  }
} // namespace chimera::library
