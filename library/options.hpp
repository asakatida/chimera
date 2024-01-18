//! options from the command line

#pragma once

#include <gsl/gsl>

#include <variant>
#include <vector>

namespace chimera::library {
  namespace options {
    enum class BytesCompare { NONE, WARN, ERROR };
    enum class Optimize { NONE, BASIC, DISCARD_DOCS };
    enum class VerboseInit { NONE, LOAD, SEARCH };
    struct Command {
      const char *script = nullptr;
    };
    struct Module {
      const char *name = nullptr;
    };
    struct Script {
      const char *name = nullptr;
    };
  } // namespace options
  struct Options {
    gsl::span<const char *> argv{};
    options::BytesCompare bytes_compare = options::BytesCompare::NONE;
    const char *chimera = nullptr;
    bool debug = false;
    bool disable_site = false;
    bool dont_add_site = false;
    bool dont_display_copyright = false;
    bool dont_write_byte_code = false;
    std::vector<const char *> extensions{};
    bool ignore_environment = false;
    bool interactive = false;
    bool isolated_mode = false;
    options::Optimize optimize = options::Optimize::NONE;
    bool skip_first_line = false;
    bool unbuffered_output = false;
    options::VerboseInit verbose_init = options::VerboseInit::NONE;
    std::vector<const char *> warnings{};
    std::variant<options::Command, options::Module, options::Script> exec{};
  };
} // namespace chimera::library
