#include "virtual_machine/fuzz.hpp"

#include <gsl/util>

#include "virtual_machine/fuzz-base64.hpp"
#include "virtual_machine/fuzz-blns.hpp"
#include "virtual_machine/fuzz-cases.hpp"

auto strings_blns_base64() -> tao::json::value {
  return tao::json::from_string(std::string(
      reinterpret_cast<const char *>(
          external_big_list_of_naughty_strings_blns_base64_json),
      gsl::narrow_cast<std::string::size_type>(
          external_big_list_of_naughty_strings_blns_base64_json_len)));
}

auto strings_blns() -> tao::json::value {
  return tao::json::from_string(
      std::string(reinterpret_cast<const char *>(
                      external_big_list_of_naughty_strings_blns_json),
                  gsl::narrow_cast<std::string::size_type>(
                      external_big_list_of_naughty_strings_blns_json_len)));
}

auto fuzz_cases() -> tao::json::value {
  return tao::json::from_string(
      std::string(reinterpret_cast<const char *>(unit_tests_fuzz_cases_json),
                  gsl::narrow_cast<std::string::size_type>(
                      unit_tests_fuzz_cases_json_len)));
}
