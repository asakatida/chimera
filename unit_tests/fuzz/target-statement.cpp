#include "fuzz/cases.hpp"

extern "C" auto LLVMFuzzerTestOneInput(const std::uint8_t *data,
                                       std::size_t size) -> int {
  return chimera::library::fuzz_parse<chimera::library::grammar::Stmt<0>,
                                      chimera::library::asdl::Stmt>(data,
                                                                         size);
}
