//! global context singleton

#include "virtual_machine/global_context.hpp"

#include "object/object.hpp"
#include "options.hpp"
#include "version.hpp"
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/process_context.hpp"
#include "virtual_machine/thread_context.hpp"

#include <gsl/narrow>

#include <atomic>
#include <csignal>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <tuple>
#include <utility>

using namespace std::literals;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace)
static std::atomic_flag SIG_INT{};

extern "C" void interupt_handler(int /*signal*/) { SIG_INT.clear(); }

namespace chimera::library::virtual_machine {
  GlobalContextImpl::GlobalContextImpl(Options options)
      : options(std::move(options)), sig_int(&SIG_INT) {
    SIG_INT.test_and_set();
    std::ignore = std::signal(SIGINT, interupt_handler);
  }
  [[nodiscard]] auto GlobalContextImpl::debug() const -> bool {
    return options.debug;
  }
  [[nodiscard]] auto GlobalContextImpl::interactive() -> int {
    if (!options.dont_display_copyright) {
      std::cout << "chimera " << CHIMERA_VERSION
                << " (default, " __DATE__ ", " __TIME__ ")\n"
                   "[" __VERSION__ "] on darwin\n"
                   R"(Type "help", "copyright", "credits" or "license" )"
                   "for more information."
                << '\n';
    }
    auto global = shared_from_this();
    auto processContext = make_process(global);
    auto main = processContext->make_module("__main__");
    while (!std::cin.eof()) {
      std::cout << ">>> ";
      auto interactive = processContext->parse_input(std::cin, "<string>");
      auto thread = make_thread(processContext, main);
      Evaluator(thread).evaluate(interactive);
    }
    return 0;
  }
  [[nodiscard]] auto GlobalContextImpl::execute(std::istream &istream,
                                                const char *source) -> int {
    auto global = shared_from_this();
    auto processContext = make_process(global);
    auto module = processContext->parse_file(istream, source);
    auto main = processContext->make_module("__main__");
    auto thread = make_thread(processContext, main);
    Evaluator(thread).evaluate(module);
    return 0;
  }
  [[nodiscard]] auto GlobalContextImpl::execute_script() -> int {
    std::ifstream istream(std::get<options::Script>(options.exec).name);
    return execute(istream, std::get<options::Script>(options.exec).name);
  }
  [[nodiscard]] auto GlobalContextImpl::execute_script_string() -> int {
    std::istringstream istream(std::get<options::Command>(options.exec).script);
    return execute(istream, "<string>");
  }
  [[nodiscard]] auto GlobalContextImpl::execute_script_input() -> int {
    return execute(std::cin, "<input>");
  }
  [[nodiscard]] auto GlobalContextImpl::execute_module() -> int {
    auto global = shared_from_this();
    auto processContext = make_process(global);
    const auto *name = std::get<options::Module>(options.exec).name;
    auto module = processContext->import_module(name);
    if (!module) {
      std::cout << options.chimera << "No module named " << name << '\n';
      return 1;
    }
    auto main = processContext->make_module("__main__");
    auto thread = make_thread(processContext, main);
    Evaluator(thread).evaluate(*module);
    return 0;
  }
  [[nodiscard]] auto
  GlobalContextImpl::optimize() const -> const options::Optimize & {
    return options.optimize;
  }
  void GlobalContextImpl::process_interrupts() const {
    if (!std::atomic_flag_test_and_set(sig_int)) {
      throw object::KeyboardInterrupt();
    }
  }
  void GlobalContextImpl::sys_argv(const object::Object &module) const {
    auto sys = module;
    object::Tuple argv;
    argv.reserve(gsl::narrow<object::Tuple::size_type>(
        std::distance(options.argv.begin(), options.argv.end())));
    for (const auto &arg : options.argv) {
      argv.emplace_back(object::Object(
          object::String(arg), {{"__class__", module.get_attribute("str")}}));
    }
    sys.set_attribute(
        // NOLINTNEXTLINE(misc-include-cleaner)
        "argv"s,
        object::Object(argv, {{"__class__", sys.get_attribute("tuple")}}));
  }
  [[nodiscard]] auto
  GlobalContextImpl::verbose_init() const -> const options::VerboseInit & {
    return options.verbose_init;
  }
  auto make_global(Options options) -> GlobalContext {
    return std::make_shared<GlobalContextImpl>(std::move(options));
  }
} // namespace chimera::library::virtual_machine
