#include <tao/pegtl.hpp>

#include "grammar/number.hpp"

// NOLINTNEXTLINE
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

// NOLINTNEXTLINE
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  try {
    tao::pegtl::parse<chimera::library::grammar::NUMBER<0>>(
        // NOLINTNEXTLINE
        tao::pegtl::memory_input<>(reinterpret_cast<const char *>(data), size,
                                   "<fuzz>"));
  } catch (const tao::pegtl::parse_error &) {
  }
  return 0;
}
