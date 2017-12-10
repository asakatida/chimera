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

//! process context attached to a virtual machine thread

#include "virtual_machine/process_context.hpp"

#include <csignal>     // for signal
#include <exception>   // for exception
#include <fstream>     // for ifstream
#include <string>      // for string
#include <string_view> // for string_view

#include <gsl/gsl>

#include "asdl/asdl.hpp" // for ExprImpl (ptr only), StmtImp...
#include "grammar/grammar.hpp"
#include "options.hpp"
#include "parse.hpp"
#include "virtual_machine/builtins.hpp" // for init
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/modules/marshal.hpp"
#include "virtual_machine/modules/sys.hpp"

static const std::string_view CHIMERA_IMPORT_PATH_VIEW
#ifdef CHIMERA_PATH
#define STRINGIFY(ARG) #ARG
    (STRINGIFY(CHIMERA_PATH))
#undef STRINGIFY
#endif
        ;

namespace chimera {
  namespace library {
    namespace virtual_machine {
      object::Object ProcessContext::make_module(std::string_view &&name) {
        auto result = modules.try_emplace("builtins", global_context.builtins);
        auto module = result.first->second.copy({});
        module.set_attribute("__builtins__", result.first->second);
        module.set_attribute(
            "__name__",
            object::Object(object::String(name),
                           {{"__class__", module.get_attribute("str")}}));
        return module;
      }

      std::optional<asdl::Module>
      ProcessContext::import_module(std::string &&module) {
        std::replace(module.begin(), module.end(), '.', '/');
        for (std::string_view::size_type
                 pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':'),
                 prev = 0;
             prev != CHIMERA_IMPORT_PATH_VIEW.npos; prev = pos + 1,
                 pos = CHIMERA_IMPORT_PATH_VIEW.find_first_of(':', prev)) {
          auto path = CHIMERA_IMPORT_PATH_VIEW.substr(prev, pos - 1);
          if (auto importModule = import_module(
                  path, std::string{module}.append("/__init__.py"));
              importModule) {
            return importModule;
          }
          if (auto importModule =
                  import_module(path, std::string{module}.append(".py"));
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
            throw std::runtime_error(std::string{"module "}
                                         .append(relativeModule)
                                         .append(" import beyond top-level"));
          }
          parent.remove_suffix(parent.size() - ending + 1);
        }
        return import_object(std::string{parent}.append(".").append(
            relativeModule.substr(index)));
      }

      object::Object &ProcessContext::import_object(std::string_view &&module) {
        auto result = modules.try_emplace(
            std::string{module}, make_module(std::string_view{module}));
        if (result.second) {
          auto index = module.find_last_of('.');
          if (index < module.size()) {
            import_object(module.substr(0, index))
                .set_attribute(std::string{module.substr(index + 1)},
                               result.first->second);
          }
          if (auto importModule = import_module(std::string{module});
              importModule) {
            ThreadContext{*this, result.first->second}.evaluate(*importModule);
          } else if (module == "marshal") {
            modules::marshal(global_context.options, result.first->second);
          } else if (module == "sys") {
            modules::sys(global_context.options, result.first->second);
          } else {
            throw std::runtime_error(
                std::string{"no module "}.append(module).append(" found"));
          }
        }
        return result.first->second;
      }

      std::optional<asdl::Module>
      ProcessContext::import_module(const std::string_view &path,
                                    const std::string &module) {
        if (global_context.options.verbose_init == VerboseInit::SEARCH) {
          std::cout << path << module << "\n";
        }
        if (std::ifstream ifstream(std::string{path}.append(module),
                                   std::iostream::in | std::iostream::binary);
            ifstream.is_open() && ifstream.good()) {
          if (global_context.options.verbose_init == VerboseInit::LOAD) {
            std::cout << path << module << "\n";
          }
          std::cerr << path << module << "\n";
          asdl::Module importModule;
          parse<grammar::FileInput>(global_context.options,
                                    grammar::Input<tao::pegtl::istream_input<>>(
                                        *this, ifstream, BUFFER_SIZE,
                                        std::string{path}.append(module)),
                                    importModule);
          return importModule;
        }
        return {};
      }

      object::Id ProcessContext::insert_constant(object::Bytes &&bytes) {
        object::Object object(
            std::move(bytes),
            {{"__class__", global_context.builtins.get_attribute("bytes")}});
        auto result = constants.try_emplace(object.id(), object);
        Ensures(result.second);
        return result.first->first;
      }

      object::Id ProcessContext::insert_constant(object::Number &&number) {
        object::Object object(
            std::move(number),
            {{"__class__", global_context.builtins.get_attribute(
                               number.is_int() ? "int" : "float")}});
        auto result = constants.try_emplace(object.id(), object);
        Ensures(result.second);
        return result.first->first;
      }

      object::Id ProcessContext::insert_constant(object::String &&string) {
        object::Object object(
            std::move(string),
            {{"__class__", global_context.builtins.get_attribute("str")}});
        auto result = constants.try_emplace(object.id(), object);
        Ensures(result.second);
        return result.first->first;
      }
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
