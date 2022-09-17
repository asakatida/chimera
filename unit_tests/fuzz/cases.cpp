#include "fuzz/cases.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include <tao/pegtl.hpp>

#include "asdl/asdl.hpp"
#include "grammar/number.hpp"
#include "grammar/rules.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/parse.hpp"
#include "virtual_machine/virtual_machine.hpp"

namespace chimera::library {
  using NumericLimits = std::numeric_limits<std::uint16_t>;
  constexpr static auto bufferSize = NumericLimits::max();
  auto fuzz_istream(const std::uint8_t *data, std::size_t size)
      -> std::istringstream;
  auto fuzz_input(std::istream &in) -> Input {
    return {in, bufferSize, "fuzz"};
  }
  auto fuzz_istream(const std::uint8_t *data, std::size_t size)
      -> std::istringstream {
    std::istringstream in(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        std::string(reinterpret_cast<const char *>(data), size));
    return in;
  }
  auto fuzz_options() -> Options {
    Options options{};
    options.chimera = "chimera";
    options.script = "fuzzer.py";
    return options;
  }
  template <typename Grammar, typename... Args>
  auto fuzz_parse(Input &&in, Args &&...args) -> int {
    auto options = fuzz_options();
    return fuzz_parse<Grammar>(options, std::move(in),
                               std::forward<Args>(args)...);
  }
  template <typename Grammar, typename... Args>
  auto fuzz_parse(const std::uint8_t *data, std::size_t size, Args &&...args)
      -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse<Grammar>(fuzz_input(in), std::forward<Args>(args)...);
  }
  template <typename Grammar, typename... Args>
  auto fuzz_parse(const Options & /*options*/, Input &&in, Args &&...args)
      -> int {
    try {
      tao::pegtl::parse<Grammar>(std::move(in), std::forward<Args>(args)...);
    } catch (const tao::pegtl::parse_error &) {
      return -1;
    }
    return 0;
  }
  auto fuzz_file_eval(const std::uint8_t *data, std::size_t size) -> int {
    return fuzz_file_eval(fuzz_istream(data, size));
  }
  auto fuzz_file_eval(std::istream &&in) -> int {
    auto options = fuzz_options();
    const object::Object builtins;
    // virtual_machine::modules::init(builtins);
    const virtual_machine::VirtualMachine virtualMachine(options, builtins);
    auto processContext = virtualMachine.process_context();
    asdl::Module module;
    try {
      module = processContext.parse_file(std::move(in), "<fuzz>");
    } catch (const tao::pegtl::parse_error &) {
      return -1;
    }
    virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("__main__")};
    try {
      threadContext.evaluate(module);
    } catch (const object::BaseException &) {
      return -1;
    }
    return 0;
  }
  auto fuzz_parse_ellipsis(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_ellipsis(fuzz_input(in));
  }
  auto fuzz_parse_ellipsis(Input &&in) -> int {
    return fuzz_parse<grammar::Ellipsis<0>>(std::move(in));
  }
  auto fuzz_parse_false(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_false(fuzz_input(in));
  }
  auto fuzz_parse_false(Input &&in) -> int {
    return fuzz_parse<grammar::False<0>>(std::move(in));
  }
  auto fuzz_parse_keywords(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_keywords(fuzz_input(in));
  }
  auto fuzz_parse_keywords(Input &&in) -> int {
    return fuzz_parse<grammar::Keywords>(std::move(in));
  }
  auto fuzz_parse_name(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_name(fuzz_input(in));
  }
  auto fuzz_parse_name(Input &&in) -> int {
    return fuzz_parse<grammar::Name<0>>(std::move(in));
  }
  auto fuzz_parse_none(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_none(fuzz_input(in));
  }
  auto fuzz_parse_none(Input &&in) -> int {
    return fuzz_parse<grammar::None<0>>(std::move(in));
  }
  auto fuzz_parse_number(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_number(fuzz_input(in));
  }
  auto fuzz_parse_number(Input &&in) -> int {
    return fuzz_parse<grammar::NUMBER<0>>(std::move(in));
  }
  auto fuzz_parse_string(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_string(fuzz_input(in));
  }
  auto fuzz_parse_string(Input &&in) -> int {
    return fuzz_parse<grammar::STRING<0>>(std::move(in));
  }
  auto fuzz_parse_true(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse_true(fuzz_input(in));
  }
  auto fuzz_parse_true(Input &&in) -> int {
    return fuzz_parse<grammar::True<0>>(std::move(in));
  }
} // namespace chimera::library
