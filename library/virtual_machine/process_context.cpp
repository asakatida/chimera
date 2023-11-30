//! process context attached to a virtual machine thread

#include "virtual_machine/process_context.hpp"

#include "asdl/asdl.hpp"
#include "builtins/builtins.hpp"
#include "importlib/importlib.hpp"
#include "marshal/marshal.hpp"
#include "object/object.hpp"
#include "sys/sys.hpp"
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/thread_context.hpp"

#include "number-rust.hpp"

#include <gsl/gsl>

#include <csignal>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std::literals;
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define STRINGIFY(ARG) #ARG##s
static const std::string CHIMERA_IMPORT_PATH_VIEW(STRINGIFY(CHIMERA_PATH));
#undef STRINGIFY

namespace chimera::library::virtual_machine {
  void destroy_module(object::Object &module) noexcept {
    std::vector<object::Object> todo = {module};
    while (!todo.empty()) {
      std::vector<object::Object> attributes;
      for (auto &work : todo) {
        attributes.reserve(attributes.size() + work.dir_size());
        for (const auto &key : work.dir()) {
          if (work.has_attribute(key)) {
            attributes.push_back(work.get_attribute(key));
            work.delete_attribute(key);
          }
        }
      }
      todo = attributes;
    }
  }
  ProcessContextImpl::ProcessContextImpl(GlobalContext &global_context)
      : builtins_(std::map<std::string, object::Object>{}),
        global_context(global_context),
        modules(
            std::map<std::string, object::Object>({{"builtins", builtins_}})) {
    modules::builtins(builtins_);
  }
  ProcessContextImpl::~ProcessContextImpl() noexcept {
    auto builtins = builtins_;
    destroy_module(builtins);
    for (auto &module : modules.write().value) {
      destroy_module(module.second);
    }
    r_vm_clear();
  }
  [[nodiscard]] auto ProcessContextImpl::builtins() const
      -> const object::Object & {
    return builtins_;
  }
  [[nodiscard]] auto ProcessContextImpl::make_module(std::string_view &&name)
      -> object::Object {
    auto result = modules.try_emplace(std::string(name));
    if (result.second) {
      modules::builtins(result.first->second);
      result.first->second.set_attribute(
          "__name__"s,
          object::Object(
              object::String(std::string(name)),
              {{"__class__"s, result.first->second.get_attribute("str"s)}}));
    }
    return result.first->second;
  }
  [[nodiscard]] auto
  ProcessContextImpl::find_module(const std::string_view &module)
      -> std::optional<std::ifstream> {
    for (std::string_view::size_type
             pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':'),
             prev = 0;
         prev != std::string_view::npos; prev = pos + 1,
             pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':', prev)) {
      auto path = CHIMERA_IMPORT_PATH_VIEW.substr(prev, pos - 1);
      try {
        std::string pathString = path.append(module).append("/__init__.py"sv);
        if (global_context->verbose_init() == options::VerboseInit::SEARCH) {
          std::cout << pathString << '\n';
        }
        std::ifstream ifstream(pathString,
                               std::iostream::in | std::iostream::binary);
        if (ifstream.is_open() && ifstream.good()) {
          return {std::move(ifstream)};
        }
      } catch (const object::BaseException &) {
      }
      try {
        std::string pathString = path.append(module).append(".py"sv);
        if (global_context->verbose_init() == options::VerboseInit::SEARCH) {
          std::cout << pathString << '\n';
        }
        std::ifstream ifstream(pathString,
                               std::iostream::in | std::iostream::binary);
        if (ifstream.is_open() && ifstream.good()) {
          return {std::move(ifstream)};
        }
      } catch (const object::BaseException &) {
      }
    }
    return {};
  }
  [[nodiscard]] auto ProcessContextImpl::import_module(std::string &&module)
      -> std::optional<asdl::Module> {
    std::replace(module.begin(), module.end(), '.', '/');
    if (auto istream = find_module(module)) {
      return asdl::Module(global_context->optimize(), *istream, module.c_str());
    }
    return {};
  }
  [[nodiscard]] auto
  ProcessContextImpl::import_object(std::string_view &&name,
                                    std::string_view &&relativeModule)
      -> const object::Object & {
    if (relativeModule.empty() || relativeModule.at(0) != '.') {
      return import_object(std::string_view{relativeModule});
    }
    auto parent = name;
    auto index = relativeModule.find_first_not_of('.');
    for (std::size_t size = 1; size < index; ++size) {
      auto ending = parent.find_last_of('.');
      if (ending > parent.size()) {
        throw std::runtime_error("module "s.append(relativeModule)
                                     .append(" import beyond top-level"sv));
      }
      parent.remove_suffix(parent.size() - ending + 1);
    }
    return import_object(std::string(parent).append(1, '.').append(
        relativeModule.substr(index)));
  }
  [[nodiscard]] auto
  ProcessContextImpl::import_object(std::string_view &&request_module)
      -> const object::Object & {
    if (!modules.contains(std::string(request_module))) {
      std::vector<std::string_view> path{request_module};
      path.reserve(
          std::count(request_module.cbegin(), request_module.cend(), '.'));
      for (auto index = path.back().find_last_of('.');
           index != std::string_view::npos;
           index = path[-1].find_last_of('.')) {
        path.emplace_back(path.back().substr(0, index));
      }
      std::reverse(path.begin(), path.end());
      for (auto &module : path) {
        auto result = make_module(std::string_view{module});
        auto index = module.find_last_of('.');
        if (index < module.size()) {
          object::Object(modules.at(std::string(module.substr(0, index))))
              .set_attribute(std::string(module.substr(index + 1)), result);
        }
        if (module == "builtin"sv) {
          modules::builtins(result);
        } else if (module == "importlib"sv) {
          modules::importlib(result);
        } else if (module == "marshal"sv) {
          modules::marshal(result);
        } else if (module == "sys"sv) {
          modules::sys(result);
          global_context->sys_argv(result);
        } else if (auto istream = find_module(module)) {
          auto process = make_process(global_context);
          auto thread = make_thread(process, result);
          Evaluator(thread).evaluate(parse_file(*istream, module.data()));
        } else {
          throw std::runtime_error(
              "no module "s.append(module).append(" found"sv));
        }
      }
    }
    return modules.at(std::string(request_module));
  }
  [[nodiscard]] auto
  ProcessContextImpl::import_module(const std::string_view &path,
                                    const std::string &module) -> asdl::Module {
    auto source = std::string(path).append(module);
    std::ifstream istream(source, std::iostream::in | std::iostream::binary);
    Ensures(istream.is_open() && istream.good());
    if (global_context->verbose_init() == options::VerboseInit::LOAD) {
      std::cout << path << module << '\n';
    }
    std::cerr << path << module << '\n';
    return parse_file(istream, source.c_str());
  }
  [[nodiscard]] auto ProcessContextImpl::parse_expression(
      std::istream &input, const char *source) const -> asdl::Expression {
    return {global_context->optimize(), input, source};
  }
  [[nodiscard]] auto ProcessContextImpl::parse_file(std::istream &input,
                                                    const char *source) const
      -> asdl::Module {
    return {global_context->optimize(), input, source};
  }
  [[nodiscard]] auto ProcessContextImpl::parse_input(std::istream &input,
                                                     const char *source) const
      -> asdl::Interactive {
    return {global_context->optimize(), input, source};
  }
  void ProcessContextImpl::process_interrupts() const {
    global_context->process_interrupts();
  }
  auto make_process(GlobalContext &global_context) -> ProcessContext {
    return std::make_shared<ProcessContextImpl>(global_context);
  }
} // namespace chimera::library::virtual_machine
