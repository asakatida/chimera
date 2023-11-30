#include "fuzz/cases.hpp"

#include "grammar/grammar.hpp"

#include <cstddef>
#include <cstdint>

extern "C" auto LLVMFuzzerTestOneInput(const std::uint8_t *data,
                                       std::size_t size) -> int {
  return chimera::library::fuzz_parse<chimera::library::grammar::FileInput,
                                      chimera::library::asdl::Module>(data,
                                                                      size);
}
