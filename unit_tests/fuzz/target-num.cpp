#include <tao/pegtl.hpp>

#include "grammar/number.hpp"

extern "C" auto LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) -> int;

extern "C" auto LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
    -> int {
  try {
    tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        tao::pegtl::memory_input<>(reinterpret_cast<const char *>(data), size,
                                   "<fuzz>"));
  } catch (const tao::pegtl::parse_error &) {
  }
  return 0;
}
