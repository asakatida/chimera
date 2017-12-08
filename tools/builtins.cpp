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

//! evaluates stdlib/_builtins.py to construct the builtin module.
//! Then prints the module construction.

#include <atomic> // for atomic_flag
#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "asdl/asdl.hpp" // for Module
#include "grammar/grammar.hpp"
#include "object/object.hpp" // for Object
#include "options.hpp"       // for Options
#include "parse.hpp"
#include "virtual_machine/virtual_machine.hpp" // for VirtualMachine

static std::atomic_flag SIG_INT;

extern "C" void interupt_handler(int signal);

extern "C" void interupt_handler(int /*signal*/) { SIG_INT.clear(); }

namespace chimera {
  namespace library {
    struct SetAttribute {
      std::string base_name;
      std::string name;
    };
    struct Work {
      const object::Object &object;
      std::string base_name;
      std::string name;
    };
    struct Printer {
      Printer(const object::Object &object, const std::string &baseName) {
        printed.try_emplace(object.id(), baseName);
        for (const auto &name : object.dir()) {
          wanted[object.get_attribute(name).id()].push_back(
              SetAttribute{baseName, name});
          work.push(Work{object.get_attribute(name), baseName, name});
        }
      }
      void operator()(const object::Instance & /*instance*/) {
        std::cout << "object::Instance{}";
      }
      void operator()(const object::Bytes &bytes) {
        std::cout << "object::Bytes{";
        bool first = true;
        for (const auto &byte : bytes) {
          if (!first) {
            std::cout << ",";
          } else {
            first = false;
          }
          std::cout << byte;
        }
        std::cout << "}";
      }
      [[noreturn]] void operator()(const object::BytesMethod &bytesMethod) {
        std::cout << "object::BytesMethod::";
        switch (bytesMethod) {}
      }
      void operator()(const object::False & /*false*/) {
        std::cout << "object::False{}";
      }
      [[noreturn]] void operator()(const object::Future & /*future*/) {
        Ensures(false);
      }
      void operator()(const object::None & /*none*/) {
        std::cout << "object::None{}";
      }
      void operator()(const object::NullFunction & /*nullFunction*/) {
        std::cout << "object::NullFunction{}";
      }
      void operator()(const object::Number & /*number*/) {
        std::cout << "object::Number{}";
      }
      [[noreturn]] void operator()(const object::NumberMethod &numberMethod) {
        std::cout << "object::NumberMethod::";
        switch (numberMethod) {}
      }
      void operator()(const object::ObjectMethod &typeMethod) {
        std::cout << "object::ObjectMethod::";
        switch (typeMethod) {
          case object::ObjectMethod::DELATTR:
            std::cout << "DELATTR";
            break;
          case object::ObjectMethod::DIR:
            std::cout << "DIR";
            break;
          case object::ObjectMethod::GETATTRIBUTE:
            std::cout << "GETATTRIBUTE";
            break;
          case object::ObjectMethod::SETATTR:
            std::cout << "SETATTR";
            break;
        }
      }
      void operator()(const object::String &string) {
        std::cout << "object::String{" << std::quoted(string) << "}";
      }
      [[noreturn]] void operator()(const object::StringMethod &stringMethod) {
        std::cout << "object::StringMethod::";
        switch (stringMethod) {}
      }
      void operator()(const object::True & /*true*/) {
        std::cout << "object::True{}";
      }
      void operator()(const object::Tuple &tuple) {
        std::cout << "object::Tuple{";
        bool first = true;
        for (const auto &object : tuple) {
          if (!first) {
            std::cout << ",";
          } else {
            first = false;
          }
          std::cout << printed[object.id()];
        }
        std::cout << "}";
      }
      [[noreturn]] void operator()(const object::TupleMethod &tupleMethod) {
        std::cout << "object::TupleMethod::";
        switch (tupleMethod) {}
      }
      void operator()(const object::SysCall &sysCall) {
        std::cout << "object::SysCall::";
        switch (sysCall) {
          case object::SysCall::COMPILE:
            std::cout << "COMPILE";
            break;
          case object::SysCall::EVAL:
            std::cout << "EVAL";
            break;
          case object::SysCall::EXEC:
            std::cout << "EXEC";
            break;
          case object::SysCall::GLOBALS:
            std::cout << "GLOBALS";
            break;
          case object::SysCall::ID:
            std::cout << "ID";
            break;
          case object::SysCall::INPUT:
            std::cout << "INPUT";
            break;
          case object::SysCall::LOCALS:
            std::cout << "LOCALS";
            break;
          case object::SysCall::PRINT:
            std::cout << "PRINT";
            break;
          case object::SysCall::OPEN:
            std::cout << "OPEN";
            break;
        }
      }
      void operator()(const library::asdl::StmtImpl &stmtImpl) {
        std::cout << &stmtImpl;
      }
      void operator()(const library::asdl::ExprImpl &exprImpl) {
        std::cout << &exprImpl;
      }
      void operator()(const object::Object &object,
                      const std::string &baseName) {
        std::cout << "object::Object " << baseName << "(";
        std::visit(*this, object.value());
        std::cout << ",{";
        bool first = true;
        for (const auto &name : object.dir()) {
          if (!first) {
            std::cout << ",";
          } else {
            first = false;
          }
          std::cout << "{" << std::quoted(name) << ",";
          if (is_printed(object.get_attribute(name))) {
            std::cout << printed[object.get_attribute(name).id()];
          } else {
            std::cout << "{/*set below*/}";
            wanted[object.get_attribute(name).id()].push_back(
                SetAttribute{baseName, name});
            work.push(Work{object.get_attribute(name), baseName, name});
          }
          std::cout << "}";
        }
        std::cout << "});";
        printed.try_emplace(object.id(), baseName);
        for (const auto &setAttribute : wanted[object.id()]) {
          std::cout << setAttribute.base_name << ".set_attribute("
                    << std::quoted(setAttribute.name) << "," << baseName
                    << ");";
        }
        wanted.erase(object.id());
      }
      template <typename Object>
      bool is_printed(Object &&object) {
        return printed.count(object.id()) != 0;
      }
      template <typename Front>
      bool incomplete_tuple(Front &&front) {
        if (std::holds_alternative<object::Tuple>(front.object.value())) {
          return incomplete_tuple(
              front, front.object,
              std::get<object::Tuple>(front.object.value()));
        }
        return false;
      }
      template <typename Front, typename Object, typename Tuple>
      bool incomplete_tuple(Front &&front, Object &&object, Tuple &&tuple) {
        if (!std::all_of(tuple.begin(), tuple.end(), [this](const auto &v) {
              return this->is_printed(v);
            })) {
          auto baseName =
              std::string(front.base_name).append("_").append(front.name);
          for (const auto &o : tuple) {
            work.push(Work{object, baseName, std::to_string(o.id())});
          }
          work.push(front);
          return true;
        }
        return false;
      }
      void operator()() {
        while (!work.empty()) {
          auto front = work.front();
          work.pop();
          if (is_printed(front.object)) {
            continue;
          }
          if (incomplete_tuple(front)) {
            continue;
          }
          (*this)(front.object, front.base_name.append("_").append(front.name));
        }
      }
      std::map<std::uint64_t, std::string> printed;
      std::map<std::uint64_t, std::vector<SetAttribute>> wanted;
      std::queue<Work> work;
    };
    static void main() {
      chimera::library::object::Object builtins;
      virtual_machine::modules::init(builtins);
      virtual_machine::GlobalContext globalContext{
          {},
          builtins,
          builtins.get_attribute("type")
              .get_attribute("__dir__")
              .get_attribute("__class__")
              .id(),
          builtins.get_attribute("compile").get_attribute("__class__").id(),
          &SIG_INT};
      virtual_machine::ProcessContext processContext{globalContext};
      asdl::Module module;
      std::vector<chimera::library::object::Object> constants;
      chimera::parse<chimera::library::grammar::FileInput>(
          globalContext.options,
          chimera::library::grammar::Input<tao::pegtl::istream_input<>>(
              constants, std::cin, chimera::BUFFER_SIZE, "<input>"),
          module);
      virtual_machine::ThreadContext{processContext, std::move(constants),
                                     processContext.make_module("builtins")}
          .evaluate(module);
      std::cout
          << "//! generated file see tools/builtins.cpp\n\n"
             "#include \"virtual_machine/builtins.hpp\"\n\n"
             "#include \"asdl/asdl.hpp\"\n"
             "#include \"object.hpp\"\n\n"
             "namespace chimera {\nnamespace library {\nnamespace modules {\n"
             "void init(const object::Object &module) {auto builtins = module;";
      Printer(builtins, "builtins")();
      std::cout << "}\n}\n}\n}" << std::endl;
    }
  } // namespace library
} // namespace chimera

int main() {
  chimera::library::main();
  return 0;
}
