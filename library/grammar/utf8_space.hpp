//! generated file see tools/generate_utf8_space.py

#pragma once

#include "grammar/rules.hpp"

namespace chimera::library::grammar {
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  using Utf8Space = sor<
      tao::pegtl::utf8::ranges<0x9, 0xd, 0x1c, 0x20, 0x85, 0x85, 0xa0, 0xa0>>;
#else
  using Utf8Space = sor<ranges<0x9, 0xd, 0x1c, 0x20, 0x85, 0x85, 0xa0, 0xa0,
                               0x1680, 0x1680, 0x2000, 0x200a, 0x2028, 0x2029,
                               0x202f, 0x202f, 0x205f, 0x205f, 0x3000, 0x3000>>;
#endif
  using Utf8NonLineBreak = minus<Utf8Space, one<'\n', '\r'>>;
} // namespace chimera::library::grammar
