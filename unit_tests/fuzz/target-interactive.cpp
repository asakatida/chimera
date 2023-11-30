#include "fuzz/cases.hpp"

#include "asdl/asdl.hpp"
#include "grammar/grammar.hpp"

#include <cstddef>
#include <cstdint>

extern "C" auto LLVMFuzzerTestOneInput(const std::uint8_t *data,
                                       std::size_t size) -> int {
  return chimera::library::fuzz_parse<chimera::library::grammar::SingleInput,
                                      chimera::library::asdl::Interactive>(
      data, size);
}
