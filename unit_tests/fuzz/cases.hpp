#pragma once

#define FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION

#include "grammar/grammar.hpp"
#include "grammar/input.hpp"

#include <cstddef>
#include <cstdint>
#include <iosfwd>

namespace chimera::library {
  [[nodiscard]] auto fuzz_istream(const std::uint8_t *data, std::size_t size)
      -> std::istringstream;
  [[nodiscard]] auto fuzz_options() -> Options;
  template <typename Grammar, typename... Args>
  [[nodiscard]] auto fuzz_parse(grammar::Input &&input, Args &&...args) -> int {
    const auto options = fuzz_options();
    return fuzz_parse<Grammar>(options, std::move(input),
                               std::forward<Args>(args)...);
  }
  template <typename Grammar, typename... Args>
  [[nodiscard]] auto fuzz_parse(const std::uint8_t *data, std::size_t size,
                                Args &&...args) -> int {
    return fuzz_parse<Grammar>(
        grammar::Input(fuzz_istream(data, size), "<fuzz>"),
        std::forward<Args>(args)...);
  }
  template <typename Grammar, typename... Args>
  [[nodiscard]] auto fuzz_parse(const Options & /*options*/,
                                grammar::Input &&input, Args &&...args) -> int {
    try {
      tao::pegtl::parse<Grammar>(std::move(input), std::forward<Args>(args)...);
    } catch (const tao::pegtl::parse_error &) {
      return -1;
    } catch (const chimera::library::grammar::SyntaxError &) {
      return -1;
    }
    return 0;
  }
  template <typename Grammar>
  [[nodiscard]] auto fuzz_parse(const std::uint8_t *data, std::size_t size)
      -> int {
    return fuzz_parse<Grammar>(
        grammar::Input(fuzz_istream(data, size), "<fuzz>"));
  }
  template <typename Grammar>
  [[nodiscard]] auto fuzz_parse(grammar::Input &&input) -> int {
    return fuzz_parse<Grammar>(std::move(input));
  }
  template <typename Grammar, typename ASDL>
  [[nodiscard]] auto fuzz_parse(const std::uint8_t *data, std::size_t size)
      -> int {
    grammar::Input input(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        std::string_view(reinterpret_cast<const char *>(data), size), "<fuzz>");
    try {
      grammar::parse<Grammar>(options::Optimize::NONE, std::move(input),
                              ASDL{});
    } catch (const tao::pegtl::parse_error &) {
      return -1;
    } catch (const chimera::library::grammar::SyntaxError &) {
      return -1;
    }
    return 0;
  }
  [[nodiscard]] auto fuzz_expression_eval(const std::uint8_t *data,
                                          std::size_t size) -> int;
  [[nodiscard]] auto fuzz_expression_eval(std::istream &&input) -> int;
  [[nodiscard]] auto fuzz_file_eval(const std::uint8_t *data, std::size_t size)
      -> int;
  [[nodiscard]] auto fuzz_file_eval(std::istream &&input) -> int;
  [[nodiscard]] auto fuzz_interactive_eval(const std::uint8_t *data,
                                           std::size_t size) -> int;
  [[nodiscard]] auto fuzz_interactive_eval(std::istream &&input) -> int;
} // namespace chimera::library
