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

#include <csignal>   // for signal
#include <exception> // for exception
#include <fstream>   // for ifstream
#include <iostream>
#include <string>      // for string
#include <string_view> // for string_view

#include <gsl/gsl> // for Ensures

#include "asdl/asdl.hpp" // for Constant, Module (ptr only)
#include "object/object.hpp"
#include "virtual_machine/modules/marshal.hpp"
#include "virtual_machine/modules/sys.hpp"
#include "virtual_machine/thread_context.hpp"

using namespace std::literals;

static const std::string_view CHIMERA_IMPORT_PATH_VIEW
#ifdef CHIMERA_PATH
#define STRINGIFY(ARG) #ARG##sv
    (STRINGIFY(CHIMERA_PATH))
#undef STRINGIFY
#endif
        ;

namespace chimera {
  namespace library {
    namespace virtual_machine {
      object::Object ProcessContext::make_module(std::string_view &&name) {
        auto result = modules.try_emplace("builtins"s, global_context.builtins);
        auto module = result.first->second.copy({});
        module.set_attribute(
            "__name__"s,
            object::Object(object::String(std::string(name)),
                           {{"__class__"s, module.get_attribute("str"s)}}));
        return module;
      }

      std::optional<asdl::Module>
      ProcessContext::import_module(std::string &&module) {
        std::replace(module.begin(), module.end(), '.', '/');
        for (std::string_view::size_type
                 pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':'),
                 prev = 0;
             prev != std::string_view::npos; prev = pos + 1,
                 pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':', prev)) {
          auto path = CHIMERA_IMPORT_PATH_VIEW.substr(prev, pos - 1);
          if (auto importModule = import_module(
                  path, std::string(module).append("/__init__.py"sv));
              importModule) {
            return importModule;
          }
          if (auto importModule =
                  import_module(path, std::string(module).append(".py"sv));
              importModule) {
            return importModule;
          }
        }
        return {};
      }

      object::Object &
      ProcessContext::import_object(std::string_view &&name,
                                    std::string_view &&relativeModule) {
        if (relativeModule.at(0) != '.') {
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

      object::Object &ProcessContext::import_object(std::string_view &&module) {
        auto result = modules.try_emplace(
            std::string(module), make_module(std::string_view{module}));
        if (result.second) {
          auto index = module.find_last_of('.');
          if (index < module.size()) {
            import_object(module.substr(0, index))
                .set_attribute(std::string(module.substr(index + 1)),
                               result.first->second);
          }
          if (auto importModule = import_module(std::string(module));
              importModule) {
            ThreadContext{*this, result.first->second}.evaluate(*importModule);
          } else if (module == "marshal"sv) {
            modules::marshal(global_context.options, result.first->second);
          } else if (module == "sys"sv) {
            modules::sys(global_context.options, result.first->second);
          } else {
            throw std::runtime_error(
                "no module "s.append(module).append(" found"sv));
          }
        }
        return result.first->second;
      }

      std::optional<asdl::Module>
      ProcessContext::import_module(const std::string_view &path,
                                    const std::string &module) {
        if (global_context.options.verbose_init == VerboseInit::SEARCH) {
          std::cout << path << module << '\n';
        }
        auto source = std::string(path).append(module);
        if (std::ifstream ifstream(source,
                                   std::iostream::in | std::iostream::binary);
            ifstream.is_open() && ifstream.good()) {
          if (global_context.options.verbose_init == VerboseInit::LOAD) {
            std::cout << path << module << '\n';
          }
          std::cerr << path << module << '\n';
          return parse_file(ifstream, source.c_str());
        }
        return {};
      }

      asdl::Constant ProcessContext::insert_constant(object::Bytes &&bytes) {
        object::Object object(
            std::move(bytes),
            {{"__class__", global_context.builtins.get_attribute("bytes")}});
        auto result = constants.try_emplace(object.id(), object);
        Ensures(result.second);
        return asdl::Constant{result.first->first};
      }

      asdl::Constant ProcessContext::insert_constant(object::Number &&number) {
        auto cls = global_context.builtins.get_attribute(
            number.is_int() ? "int" : "float");
        object::Object object(std::move(number), {{"__class__", cls}});
        auto result = constants.try_emplace(object.id(), object);
        Ensures(result.second);
        return asdl::Constant{result.first->first};
      }

      asdl::Constant ProcessContext::insert_constant(object::String &&string) {
        object::Object object(
            std::move(string),
            {{"__class__", global_context.builtins.get_attribute("str")}});
        auto result = constants.try_emplace(object.id(), object);
        Ensures(result.second);
        return asdl::Constant{result.first->first};
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
