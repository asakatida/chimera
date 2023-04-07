// Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
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

//! process context attached to a virtual machine thread

#include "virtual_machine/process_context.hpp"

#include <csignal>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>

#include <gsl/gsl>

#include "asdl/asdl.hpp"
#include "builtins/builtins.hpp"
#include "importlib/importlib.hpp"
#include "marshal/marshal.hpp"
#include "object/object.hpp"
#include "sys/sys.hpp"
#include "virtual_machine/thread_context.hpp"

using namespace std::literals;
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define STRINGIFY(ARG) #ARG##sv
static const std::string_view CHIMERA_IMPORT_PATH_VIEW(STRINGIFY(CHIMERA_PATH));
#undef STRINGIFY

namespace chimera::library::virtual_machine {
  void destroy_module(object::Object &module,
                      std::unordered_set<object::Id> &visited) noexcept {
    if (visited.contains(module.id())) {
      return;
    }
    visited.reserve(visited.size() + module.dir_size());
    visited.emplace(module.id());
    std::vector<object::Object> attributes;
    attributes.reserve(module.dir_size());
    for (const auto &key : module.dir()) {
      attributes.push_back(module.get_attribute(key));
      module.set_attribute(key, object::Object());
    }
    for (auto &attribute : attributes) {
      destroy_module(attribute, visited);
    }
  }
  ProcessContext::ProcessContext(const GlobalContext &global_context)
      : global_context(global_context) {
    modules::builtins(builtins_);
  }
  ProcessContext::~ProcessContext() noexcept {
    std::unordered_set<object::Id> visited;
    auto builtins = builtins_;
    destroy_module(builtins, visited);
    for (auto &module : modules.write().value) {
      destroy_module(module.second, visited);
    }
  }
  auto ProcessContext::builtins() const -> const object::Object & {
    return builtins_;
  }
  auto ProcessContext::make_module(std::string_view &&name) -> object::Object {
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
  auto ProcessContext::import_module(std::string &&module)
      -> const asdl::Module & {
    std::replace(module.begin(), module.end(), '.', '/');
    for (std::string_view::size_type
             pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':'),
             prev = 0;
         prev != std::string_view::npos; prev = pos + 1,
             pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':', prev)) {
      auto path = CHIMERA_IMPORT_PATH_VIEW.substr(prev, pos - 1);
      try {
        import_module(path, std::string(module).append("/__init__.py"sv));
      } catch (const std::exception &) {
      }
      try {
        import_module(path, std::string(module).append(".py"sv));
      } catch (const std::exception &) {
      }
    }
    Ensures(false);
  }
  auto ProcessContext::import_object(std::string_view &&name,
                                     std::string_view &&relativeModule)
      -> const object::Object & {
    if (relativeModule.empty() || relativeModule.at(0) != '.') {
      return import_object(std::string_view{relativeModule});
    }
    auto parent = name;
    auto index = relativeModule.find_first_not_of('.');
    for (std::size_t i = 1; i < index; ++i) {
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
  auto ProcessContext::import_object(std::string_view &&module)
      -> const object::Object & {
    if (!modules.contains(std::string(module))) {
      auto result = make_module(std::string_view{module});
      auto index = module.find_last_of('.');
      if (index < module.size()) {
        result = import_object(module.substr(0, index));
        result.set_attribute(std::string(module.substr(index + 1)), result);
      }
      try {
        auto importModule = import_module(std::string(module));
        ThreadContext{*this, result}.evaluate(importModule);
      } catch (const std::exception &) {
        if (module == "importlib"sv) {
          modules::importlib(result);
        } else if (module == "marshal"sv) {
          modules::marshal(result);
        } else if (module == "sys"sv) {
          modules::sys(result);
          global_context.sys_argv(result);
        } else {
          throw std::runtime_error(
              "no module "s.append(module).append(" found"sv));
        }
      }
    }
    return modules.at(std::string(module));
  }
  auto ProcessContext::import_module(const std::string_view &path,
                                     const std::string &module)
      -> asdl::Module {
    if (global_context.verbose_init() == VerboseInit::SEARCH) {
      std::cout << path << module << '\n';
    }
    auto source = std::string(path).append(module);
    std::ifstream ifstream(source, std::iostream::in | std::iostream::binary);
    Ensures(ifstream.is_open() && ifstream.good());
    if (global_context.verbose_init() == VerboseInit::LOAD) {
      std::cout << path << module << '\n';
    }
    std::cerr << path << module << '\n';
    return parse_file(std::move(ifstream), source.c_str());
  }
  auto ProcessContext::parse_expression(const std::string_view &data,
                                        const char *source) const
      -> asdl::Expression {
    return {global_context.optimize(), data, source};
  }
  auto ProcessContext::parse_expression(std::istream &&input,
                                        const char *source) const
      -> asdl::Expression {
    return {global_context.optimize(), std::move(input), source};
  }
  auto ProcessContext::parse_file(const std::string_view &data,
                                  const char *source) const -> asdl::Module {
    return {global_context.optimize(), data, source};
  }
  auto ProcessContext::parse_file(std::istream &&input, const char *source)
      -> asdl::Module {
    return {global_context.optimize(), std::move(input), source};
  }
  auto ProcessContext::parse_input(const std::string_view &data,
                                   const char *source) const
      -> asdl::Interactive {
    return {global_context.optimize(), data, source};
  }
  auto ProcessContext::parse_input(std::istream &&input, const char *source)
      -> asdl::Interactive {
    return {global_context.optimize(), std::move(input), source};
  }
  void ProcessContext::process_interrupts() const {
    global_context.process_interrupts();
  }
} // namespace chimera::library::virtual_machine
