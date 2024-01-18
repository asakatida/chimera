//! global context singleton

#include "virtual_machine/global_context.hpp"

#include "object/object.hpp"
#include "version.hpp"
#include "virtual_machine/process_context.hpp"
#include "virtual_machine/thread_context.hpp"

#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

using namespace std::literals;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace)
static std::atomic_flag SIG_INT{};

extern "C" void interupt_handler(int /*signal*/) { SIG_INT.clear(); }

namespace chimera::library::virtual_machine {
  GlobalContext::GlobalContext(Options options)
      : options(std::move(options)), sig_int(&SIG_INT) {
    SIG_INT.test_and_set();
    std::ignore = std::signal(SIGINT, interupt_handler);
  }
  [[nodiscard]] auto GlobalContext::debug() const -> bool {
    return options.debug;
  }
  [[nodiscard]] auto GlobalContext::interactive() -> int {
    if (!options.dont_display_copyright) {
      std::cout << "chimera " << CHIMERA_VERSION
                << " (default, " __DATE__ ", " __TIME__ ")\n"
                   "[" __VERSION__ "] on darwin\n"
                   R"(Type "help", "copyright", "credits" or "license" )"
                   "for more information."
                << std::endl;
    }
    ProcessContext processContext{*this};
    auto main = processContext.make_module("__main__");
    while (!std::cin.eof()) {
      std::cout << ">>> ";
      auto interactive = processContext.parse_input(std::cin, "<string>");
      ThreadContext(processContext, main).evaluate(interactive);
    }
    return 0;
  }
  [[nodiscard]] auto GlobalContext::execute(std::istream &istream,
                                            const char *source) -> int {
    ProcessContext processContext{*this};
    auto module = processContext.parse_file(istream, source);
    auto main = processContext.make_module("__main__");
    ThreadContext(processContext, main).evaluate(module);
    return 0;
  }
  [[nodiscard]] auto GlobalContext::execute_script() -> int {
    std::ifstream istream(std::get<options::Script>(options.exec).name);
    return execute(istream, std::get<options::Script>(options.exec).name);
  }
  [[nodiscard]] auto GlobalContext::execute_script_string() -> int {
    std::istringstream istream(std::get<options::Command>(options.exec).script);
    return execute(istream, "<string>");
  }
  [[nodiscard]] auto GlobalContext::execute_script_input() -> int {
    return execute(std::cin, "<input>");
  }
  [[nodiscard]] auto GlobalContext::execute_module() -> int {
    ProcessContext processContext{*this};
    auto module = processContext.import_module(
        std::get<options::Module>(options.exec).name);
    auto main = processContext.make_module("__main__");
    ThreadContext(processContext, main).evaluate(module);
    return 0;
  }
  [[nodiscard]] auto GlobalContext::optimize() const
      -> const options::Optimize & {
    return options.optimize;
  }
  void GlobalContext::process_interrupts() const {
    if (!std::atomic_flag_test_and_set(sig_int)) {
      throw object::KeyboardInterrupt();
    }
  }
  void GlobalContext::sys_argv(const object::Object &module) const {
    auto sys = module;
    object::Tuple argv;
    argv.reserve(gsl::narrow<object::Tuple::size_type>(
        std::distance(options.argv.begin(), options.argv.end())));
    for (const auto &arg : options.argv) {
      argv.emplace_back(object::Object(
          object::String(arg), {{"__class__", module.get_attribute("str")}}));
    }
    sys.set_attribute(
        "argv"s,
        object::Object(argv, {{"__class__", sys.get_attribute("tuple")}}));
  }
  [[nodiscard]] auto GlobalContext::verbose_init() const
      -> const options::VerboseInit & {
    return options.verbose_init;
  }
} // namespace chimera::library::virtual_machine
