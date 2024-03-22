//! main interpreter start

#include "object/object.hpp"                  // for Object
#include "options.hpp"                        // for Options, BytesCompare
#include "version.hpp"                        // for CHIMERA_GIT_HEAD, CHIM...
#include "virtual_machine/global_context.hpp" // for GlobalContext

#include "number-rust.hpp" // for r_vm_end

#include <gsl/narrow>   // for narrow
#include <gsl/span>     // for span_iterator, span
#include <gsl/span_ext> // for make_span
#include <gsl/util>     // for finally

#include <cstring>   // for size_t, strncmp, strlen
#include <exception> // for exception
#include <iostream>  // for operator<<, char_traits
#include <iterator>  // for distance, literals, next
#include <stdexcept> // for runtime_error
#include <string>    // for basic_string, to_string
#include <vector>    // for vector

// NOLINTBEGIN(misc-use-anonymous-namespace)

using namespace std::literals;

namespace chimera::library {
  using Span = gsl::span<const char *>;
  template <typename InputIt>
  // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
  [[nodiscard]] auto forward_args(InputIt &&begin, const InputIt &end) -> Span {
    if (auto distance = std::distance(begin, end); distance > 0) {
      return gsl::make_span(&*begin, gsl::narrow<Span::size_type>(distance));
    }
    return Span{};
  }
  static auto print_help(const Span &args) -> int {
    std::cout << args[0]
              << " [-bBdEhiIOqsSuvVWx?]"
                 " [-c command | -m module-name | script | - ]"
                 " [args]"
              << '\n';
    return 0;
  }
  static auto print_version(const Span &args) -> int {
    std::cout << args[0] << " " << CHIMERA_VERSION << " (" << CHIMERA_GIT_HEAD
              << ")\n"
                 "[" __VERSION__ "]"
              << '\n';
    return 0;
  }
  using Argv = gsl::span<const char>;
  // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved,readability-function-cognitive-complexity)
  static auto main(Span &&args) noexcept -> int {
    auto finally = gsl::finally([] { r_vm_end(); });
    std::cerr.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    std::cin.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    std::cout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    try {
      try {
        Options options{};
        auto arg = args.begin();
        options.chimera = *arg++;
        for (; arg != args.end(); ++arg) {
          auto argLen = std::strlen(*arg);
          const Argv argCStr{*arg, gsl::narrow<Argv::size_type>(argLen)};
          auto argChar = argCStr.begin();
          if (*argChar != '-') {
            options.exec = options::Script{*arg};
            options.argv = forward_args(std::next(arg), args.end());
            return virtual_machine::make_global(options)->execute_script();
          }
          if (argLen == 1) {
            options.argv = forward_args(std::next(arg), args.end());
            return virtual_machine::make_global(options)
                ->execute_script_input();
          }
          ++argChar;
          if (*argChar == '-') {
            if (argLen == 9 && std::strncmp(*arg, "--version", 9) == 0) {
              return print_version(args);
            }
            if (argLen == 6 && std::strncmp(*arg, "--help", 6) == 0) {
              return print_help(args);
            }
            throw std::runtime_error(
                // NOLINTNEXTLINE(misc-include-cleaner)
                "unrecognized option "s.append(*arg)
                    .append(" at position ")
                    .append(std::to_string(std::distance(args.begin(), arg))));
          }
          for (; argChar != argCStr.end(); ++argChar) {
            switch (*argChar) {
              case '?':
              case 'h':
                return print_help(args);
              case 'b':
                options.bytes_compare =
                    options.bytes_compare == options::BytesCompare::NONE
                        ? options::BytesCompare::WARN
                        : options::BytesCompare::ERROR;
                break;
              case 'B':
                options.dont_write_byte_code = true;
                break;
              case 'c':
                ++argChar;
                if (argChar != argCStr.end()) {
                  options.exec = options::Command{&*argChar};
                } else {
                  ++arg;
                  if (arg == args.end()) {
                    throw std::runtime_error("missing command argument");
                  }
                  options.exec = options::Command{*arg};
                }
                options.argv = forward_args(std::next(arg), args.end());
                return virtual_machine::make_global(options)
                    ->execute_script_string();
              case 'd':
                options.debug = true;
                break;
              case 'E':
                options.ignore_environment = true;
                break;
              case 'i':
                return virtual_machine::make_global(options)->interactive();
              case 'I':
                options.isolated_mode = true;
                break;
              case 'm':
                ++argChar;
                if (argChar != argCStr.end()) {
                  options.exec = options::Module{&*argChar};
                } else {
                  ++arg;
                  if (arg == args.end()) {
                    throw std::runtime_error("missing module name argument");
                  }
                  options.exec = options::Module{*arg};
                }
                options.argv = forward_args(std::next(arg), args.end());
                return virtual_machine::make_global(options)->execute_module();
              case 'O':
                options.optimize = options.optimize == options::Optimize::NONE
                                       ? options::Optimize::BASIC
                                       : options::Optimize::DISCARD_DOCS;
                break;
              case 'q':
                options.dont_display_copyright = true;
                break;
              case 'R':
                break;
              case 's':
                options.dont_add_site = true;
                break;
              case 'S':
                options.disable_site = true;
                break;
              case 'u':
                options.unbuffered_output = true;
                break;
              case 'v':
                options.verbose_init =
                    options.verbose_init == options::VerboseInit::NONE
                        ? options::VerboseInit::LOAD
                        : options::VerboseInit::SEARCH;
                break;
              case 'V':
                return print_version(args);
              case 'W':
                ++argChar;
                if (argChar != argCStr.end()) {
                  options.warnings.emplace_back(&*argChar);
                  break;
                }
                ++arg;
                if (arg == args.end()) {
                  throw std::runtime_error("missing warning argument");
                }
                options.warnings.emplace_back(*arg);
                break;
              case 'x':
                options.skip_first_line = true;
                break;
              case 'X':
                ++argChar;
                if (argChar != argCStr.end()) {
                  options.extensions.emplace_back(&*argChar);
                  break;
                }
                ++arg;
                if (arg == args.end()) {
                  throw std::runtime_error("missing extension argument");
                }
                options.extensions.emplace_back(*arg);
                break;
              default:
                throw std::runtime_error(
                    // NOLINTNEXTLINE(misc-include-cleaner)
                    "unrecognized option -"s.append(1, *argChar)
                        .append(" at position ")
                        .append(
                            std::to_string(std::distance(args.begin(), arg))));
            }
          }
        }
      } catch (const object::BaseException &error) {
        std::cerr << error << '\n';
        return 1;
      } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
      }
    } catch (...) {
      return 254;
    }
    return 1;
  }
} // namespace chimera::library

auto main(int argc, const char *argv[]) -> int {
  return chimera::library::main(gsl::make_span(argv, argc));
}

// NOLINTEND(misc-use-anonymous-namespace)
