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

//! main interpreter start

#include <exception> // for exception
#include <iterator>  // for distance

#include <gsl/gsl> // for make_span

#include "object/object.hpp" // for Object
#include "options.hpp"       // for Options
#include "version.hpp"       // for CHIMERA_GIT_HEAD, CHIMERA_VE...
#include "virtual_machine/virtual_machine.hpp" // for init

using namespace std::literals;

namespace chimera {
  namespace library {
    using Span = gsl::span<const char *>;

    template <typename InputIt>
    auto forward_args(InputIt &&begin, const InputIt &end) {
      if (auto distance = std::distance(begin, end); distance > 0) {
        return gsl::make_span(&*begin, static_cast<Span::size_type>(distance));
      }
      return Span{};
    }

    static int print_help(const Span &args) {
      std::cout << args.at(0)
                << " [-bBdEhiIOqsSuvVWx?]"
                   " [-c command | -m module-name | script | - ]"
                   " [args]"
                << std::endl;
      return 0;
    }

    static int print_version(const Span &args) {
      std::cout << args.at(0)
                << " " CHIMERA_VERSION " (" CHIMERA_GIT_HEAD ")\n"
                   "[" __VERSION__ "]"
                << std::endl;
      return 0;
    }

    static int main(Span &&args) noexcept {
      std::cerr.exceptions(std::ios_base::failbit | std::ios_base::badbit);
      std::cin.exceptions(std::ios_base::failbit | std::ios_base::badbit);
      std::cout.exceptions(std::ios_base::failbit | std::ios_base::badbit);
      try {
        try {
          Options options{};
          object::Object builtins;
          virtual_machine::modules::init(builtins);

          auto arg = args.begin();
          options.chimera = *arg++;

          for (; arg != args.end(); ++arg) {
            auto argLen = std::strlen(*arg);
            gsl::cstring_span<> argCStr{
                *arg, static_cast<gsl::cstring_span<>::index_type>(argLen)};
            auto argChar = argCStr.begin();
            if (*argChar != '-') {
              options.script = *arg;
              options.argv = forward_args(std::next(arg), args.end());
              return virtual_machine::VirtualMachine(options, builtins)
                  .global_context.execute_script();
            }
            if (argCStr.length() == 1) {
              options.argv = forward_args(std::next(arg), args.end());
              return virtual_machine::VirtualMachine(options, builtins)
                  .global_context.execute_script_input();
            }
            ++argChar;
            if (*argChar == '-') {
              if (argLen == 9 && std::strncmp(*arg, "--version", 9) == 0) {
                return print_version(args);
              }
              if (argLen == 6 && std::strncmp(*arg, "--help", 6) == 0) {
                return print_help(args);
              }
              throw std::runtime_error("unrecognized option "s.append(*arg)
                                           .append(" at position ")
                                           .append(std::to_string(std::distance(
                                               args.begin(), arg))));
            }
            for (; argChar != argCStr.cend(); ++argChar) {
              switch (*argChar) {
                case '?':
                case 'h':
                  return print_help(args);
                case 'b':
                  options.bytes_compare =
                      options.bytes_compare == BytesCompare::NONE
                          ? BytesCompare::WARN
                          : BytesCompare::ERROR;
                  break;
                case 'B':
                  options.dont_write_byte_code = true;
                  break;
                case 'c':
                  ++argChar;
                  if (argChar != argCStr.cend()) {
                    options.command = &*argChar;
                  } else {
                    ++arg;
                    if (arg == args.end()) {
                      throw std::runtime_error("missing command argument");
                    }
                    options.command = *arg;
                  }
                  options.argv = forward_args(std::next(arg), args.end());
                  return virtual_machine::VirtualMachine(options, builtins)
                      .global_context.execute_script_string();
                case 'd':
                  options.debug = true;
                  break;
                case 'E':
                  options.ignore_environment = true;
                  break;
                case 'i':
                  options.interactive = true;
                  break;
                case 'I':
                  options.isolated_mode = true;
                  break;
                case 'm':
                  ++argChar;
                  if (argChar != argCStr.cend()) {
                    options.module_name = &*argChar;
                  } else {
                    ++arg;
                    if (arg == args.end()) {
                      throw std::runtime_error("missing module name argument");
                    }
                    options.module_name = *arg;
                  }
                  options.argv = forward_args(std::next(arg), args.end());
                  return virtual_machine::VirtualMachine(options, builtins)
                      .global_context.execute_module();
                case 'O':
                  options.optimize = options.optimize == Optimize::NONE
                                         ? Optimize::BASIC
                                         : Optimize::DISCARD_DOCS;
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
                      options.verbose_init == VerboseInit::NONE
                          ? VerboseInit::LOAD
                          : VerboseInit::SEARCH;
                  break;
                case 'V':
                  return print_version(args);
                case 'W':
                  ++argChar;
                  if (argChar != argCStr.cend()) {
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
                  if (argChar != argCStr.cend()) {
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
                      "unrecognized option -"s.append(1, *argChar)
                          .append(" at position ")
                          .append(std::to_string(
                              std::distance(args.begin(), arg))));
              }
            }
          }
          return virtual_machine::VirtualMachine(options, builtins)
              .global_context.interactive();
        } catch (const std::exception &error) {
          std::cerr << error.what() << std::endl;
          return 1;
        }
      } catch (...) {
        return 254;
      }
    }
  } // namespace library
} // namespace chimera

int main(int argc, const char *argv[]) {
  return chimera::library::main(gsl::make_span(argv, argc));
}
