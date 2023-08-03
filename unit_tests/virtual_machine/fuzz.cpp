#include "virtual_machine/fuzz.hpp"
#include "fuzz/cases.hpp"

#include "asdl/asdl.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <stdexcept>

namespace chimera::library {
  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  void FuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
    std::ignore = fuzz_parse<grammar::Ellipsis<0>>(data, size);
    std::ignore =
        fuzz_parse<grammar::Expr<0>, grammar::rules::Stack<asdl::ExprImpl>>(
            data, size);
    std::ignore = fuzz_parse<grammar::False<0>>(data, size);
    std::ignore =
        fuzz_parse<grammar::SingleInput, asdl::Interactive>(data, size);
    std::ignore = fuzz_parse<grammar::Keywords>(data, size);
    std::ignore = fuzz_parse<grammar::FileInput, asdl::Module>(data, size);
    std::ignore = fuzz_parse<grammar::Name<0>>(data, size);
    std::ignore = fuzz_parse<grammar::None<0>>(data, size);
    std::ignore = fuzz_parse<grammar::NUMBER<0>>(data, size);
    std::ignore =
        fuzz_parse<grammar::Stmt<0>, grammar::rules::Stack<asdl::StmtImpl>>(
            data, size);
    std::ignore = fuzz_parse<grammar::STRING<0>>(data, size);
    std::ignore = fuzz_parse<grammar::True<0>>(data, size);
    std::ignore = fuzz_expression_eval(data, size);
    std::ignore = fuzz_interactive_eval(data, size);
    std::ignore = fuzz_file_eval(data, size);
  }
} // namespace chimera::library

auto pos_of_char(const char chr) -> std::uint8_t {
  if (chr >= 'A' && chr <= 'Z') {
    return chr - 'A';
  }
  if (chr >= 'a' && chr <= 'z') {
    return chr - 'a' + ('Z' - 'A') + 1;
  }
  if (chr >= '0' && chr <= '9') {
    return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
  }
  if (chr == '+' || chr == '-') {
    return 62;
  }
  if (chr == '/' || chr == '_') {
    return 63;
  }
  Ensures(false);
}

auto base64_decode(const std::string &data) -> std::string {
  if (data.empty()) {
    return {};
  }
  const auto length_of_string = data.length();
  const auto approx_length_of_decoded_string = length_of_string / 4U * 3U;
  std::string ret;
  ret.reserve(approx_length_of_decoded_string);
  for (std::size_t pos = 0; pos < length_of_string; pos += 4) {
    auto pos_of_char_1 = pos_of_char(data.at(pos + 1));
    ret.push_back(static_cast<std::string::value_type>(
        ((pos_of_char(data.at(pos))) << 2U) + ((pos_of_char_1 & 0x30U) >> 4U)));
    if ((pos + 2 < length_of_string) && data.at(pos + 2) != '=' &&
        data.at(pos + 2U) != '.') {
      auto pos_of_char_2 = pos_of_char(data.at(pos + 2U));
      ret.push_back(static_cast<std::string::value_type>(
          ((pos_of_char_1 & 0x0fU) << 4U) + ((pos_of_char_2 & 0x3cU) >> 2U)));
      if ((pos + 3U < length_of_string) && data.at(pos + 3U) != '=' &&
          data.at(pos + 3U) != '.') {
        ret.push_back(static_cast<std::string::value_type>(
            ((pos_of_char_2 & 0x03U) << 6U) + pos_of_char(data.at(pos + 3U))));
      }
    }
  }
  return ret;
}

auto json_from_file(const std::string &path) -> tao::json::value {
  static std::map<std::string, tao::json::value> cache;
  if (cache.find(path) == cache.end()) {
    cache[path] = tao::json::from_file(path);
  }
  return cache[path];
}

void TestOne(const std::string &data) {
  chimera::library::FuzzerTestOneInput(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<const std::uint8_t *>(data.data()), data.size());
}
