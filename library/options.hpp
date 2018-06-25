// Copyright (c) 2017 Adam Grandquist <grandquista@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//! options from the command line

#pragma once

#include <vector>

#include <gsl/gsl>

namespace chimera {
  namespace library {
    enum class BytesCompare { NONE, WARN, ERROR };
    enum class Optimize { NONE, BASIC, DISCARD_DOCS };
    enum class VerboseInit { NONE, LOAD, SEARCH };
    struct Options {
      bool debug = false;
      bool disable_site = false;
      bool dont_add_site = false;
      bool dont_display_copyright = false;
      bool dont_write_byte_code = false;
      bool ignore_environment = false;
      bool interactive = false;
      bool isolated_mode = false;
      bool skip_first_line = false;
      bool unbuffered_output = false;

      BytesCompare bytes_compare = BytesCompare::NONE;
      Optimize optimize = Optimize::NONE;
      VerboseInit verbose_init = VerboseInit::NONE;

      gsl::span<const char *> argv{};
      std::vector<const char *> extensions{};
      std::vector<const char *> warnings{};

      const char *chimera = nullptr;
      const char *command = nullptr;
      const char *module_name = nullptr;
      const char *script = nullptr;
    };
  } // namespace library
} // namespace chimera
