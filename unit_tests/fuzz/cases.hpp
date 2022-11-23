#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>

#include "grammar/grammar.hpp"

namespace chimera::library {
  using Input = grammar::Input<tao::pegtl::istream_input<>>;
  auto fuzz_input(std::istream &in) -> Input;
  auto fuzz_istream(const std::uint8_t *data, std::size_t size)
      -> std::istringstream;
  auto fuzz_options() -> Options;
  template <typename Grammar>
  auto fuzz_eval(const std::uint8_t *data, std::size_t size) -> int {
    return fuzz_eval<Grammar>(fuzz_istream(data, size));
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
    } catch (const chimera::library::grammar::SyntaxError &) {
      return -1;
    }
    return 0;
  }
  template <typename Grammar>
  auto fuzz_parse(const std::uint8_t *data, std::size_t size) -> int {
    auto in = fuzz_istream(data, size);
    return fuzz_parse<Grammar>(fuzz_input(in));
  }
  template <typename Grammar>
  auto fuzz_parse(Input &&in) -> int {
    return fuzz_parse<Grammar>(std::move(in));
  }
  auto fuzz_expression_eval(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_expression_eval(std::istream &&in) -> int;
  auto fuzz_file_eval(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_file_eval(std::istream &&in) -> int;
  auto fuzz_interactive_eval(const std::uint8_t *data, std::size_t size) -> int;
  auto fuzz_interactive_eval(std::istream &&in) -> int;
} // namespace chimera::library
