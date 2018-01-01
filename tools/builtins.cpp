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

//! evaluates stdlib/_builtins.py to construct the builtin module.
//! Then prints the module construction.

#include <algorithm>
#include <atomic> // for atomic_flag
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "asdl/asdl.hpp"                       // for Module
#include "object/object.hpp"                   // for Object
#include "options.hpp"                         // for Options
#include "virtual_machine/virtual_machine.hpp" // for VirtualMachine

namespace chimera {
  namespace library {
    struct SetAttribute {
      std::string base_name;
      std::string name;
    };

    struct Work {
      object::Object object;
      std::string base_name;
      std::string name;
    };

    struct Compare {
      bool operator()(const SetAttribute &a, const SetAttribute &b) const;
      bool operator()(const Work &a, const Work &b) const;
    };

    struct Printer {
      std::map<object::Id, object::Id> remap;
      std::map<object::Id, std::string> printed;
      std::priority_queue<Work, std::vector<Work>, Compare> queue;
      std::map<object::Id, std::vector<SetAttribute>> wanted;
    };

    static Printer *PRINTER;

    static object::Id id(const object::Object &object) {
      return PRINTER->remap.try_emplace(object.id(), object.id()).first->second;
    }

    static void remap(const object::Object &module,
                      const object::Object &previous) {
      if (!PRINTER->remap.try_emplace(previous.id(), module.id()).second) {
        return;
      }
      for (const auto &name : module.dir()) {
        if (previous.has_attribute(name)) {
          remap(module.get_attribute(name), previous.get_attribute(name));
        }
      }
      for (const auto &name : previous.dir()) {
        if (module.has_attribute(name)) {
          remap(module.get_attribute(name), previous.get_attribute(name));
        }
      }
    }

    static bool is_printed(const object::Object &object) {
      return PRINTER->printed.count(id(object)) != 0;
    }

    bool Compare::operator()(const SetAttribute &a,
                             const SetAttribute &b) const {
      if (a.base_name == b.base_name) {
        return a.name < b.name;
      }
      return a.base_name < b.base_name;
    }

    bool Compare::operator()(const Work &a, const Work &b) const {
      if (is_printed(b.object) && !is_printed(a.object)) {
        return true;
      }
      if (is_printed(a.object) && !is_printed(b.object)) {
        return false;
      }
      if (a.base_name == b.base_name) {
        return a.name > b.name;
      }
      return a.base_name > b.base_name;
    }

    struct IncompleteTuple {
      std::optional<object::Object>
      operator()(const object::Tuple &tuple) const {
        for (const auto &object : tuple) {
          if (!is_printed(object)) {
            return {object};
          }
        }
        return {};
      }
      template <typename Type>
      std::optional<object::Object> operator()(const Type & /*type*/) const {
        return {};
      }
    };

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::Instance & /*instance*/) {
      return os << "object::Instance{}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::Bytes &bytes) {
      os << "object::Bytes{";
      bool first = true;
      for (const auto &byte : bytes) {
        if (!first) {
          os << ",";
        } else {
          first = false;
        }
        os << byte;
      }
      return os << "}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::BytesMethod &bytesMethod) {
      os << "object::BytesMethod::";
      switch (bytesMethod) {}
      return os;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::False & /*false*/) {
      return os << "object::False{}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::Future & /*future*/) {
      Ensures(false);
      return os;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::None & /*none*/) {
      return os << "object::None{}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::NullFunction & /*nullFunction*/) {
      return os << "object::NullFunction{}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::Number & /*number*/) {
      return os << "object::Number{}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::NumberMethod &numberMethod) {
      os << "object::NumberMethod::";
      switch (numberMethod) {}
      return os;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::ObjectMethod &objectMethod) {
      os << "object::ObjectMethod::";
      switch (objectMethod) {
        case object::ObjectMethod::DELATTR:
          return os << "DELATTR";
        case object::ObjectMethod::DIR:
          return os << "DIR";
        case object::ObjectMethod::GETATTRIBUTE:
          return os << "GETATTRIBUTE";
        case object::ObjectMethod::SETATTR:
          return os << "SETATTR";
      }
      return os;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::String &string) {
      return os << "object::String(" << std::quoted(string.value) << "s)";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::StringMethod &stringMethod) {
      os << "object::StringMethod::";
      switch (stringMethod) {}
      return os;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::True & /*true*/) {
      return os << "object::True{}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::Tuple &tuple) {
      os << "object::Tuple{";
      bool first = true;
      for (const auto &object : tuple) {
        if (!first) {
          os << ",";
        } else {
          first = false;
        }
        os << PRINTER->printed.at(id(object));
      }
      return os << "}";
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::TupleMethod &tupleMethod) {
      os << "object::TupleMethod::";
      switch (tupleMethod) {}
      return os;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const object::SysCall &sysCall) {
      os << "object::SysCall::";
      switch (sysCall) {
        case object::SysCall::COMPILE:
          return os << "COMPILE";
        case object::SysCall::EVAL:
          return os << "EVAL";
        case object::SysCall::EXEC:
          return os << "EXEC";
        case object::SysCall::GLOBALS:
          return os << "GLOBALS";
        case object::SysCall::ID:
          return os << "ID";
        case object::SysCall::INPUT:
          return os << "INPUT";
        case object::SysCall::LOCALS:
          return os << "LOCALS";
        case object::SysCall::PRINT:
          return os << "PRINT";
        case object::SysCall::OPEN:
          return os << "OPEN";
      }
      return os;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const asdl::StmtImpl &stmtImpl) {
      return os << &stmtImpl;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
               const asdl::ExprImpl &exprImpl) {
      return os << &exprImpl;
    }

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const Work &work) {
      auto baseName = std::string(work.base_name).append("_").append(work.name);
      auto object = std::visit(IncompleteTuple{}, work.object.value());
      while (object) {
        os << Work{*object, baseName, std::to_string(PRINTER->printed.size())};
        object = std::visit(IncompleteTuple{}, work.object.value());
      }
      if (is_printed(work.object)) {
        return os;
      }
      os << "object::Object " << baseName << "(";
      std::visit([&os](const auto &value) { os << value; },
                 work.object.value());
      os << ",{";
      bool first = true;
      for (const auto &name : work.object.dir()) {
        if (!first) {
          os << ",";
        } else {
          first = false;
        }
        os << "{" << std::quoted(name) << "s,";
        if (is_printed(work.object.get_attribute(name))) {
          os << PRINTER->printed.at(id(work.object.get_attribute(name)));
        } else {
          os << "{/*set below*/}";
          PRINTER->wanted[id(work.object.get_attribute(name))].push_back(
              SetAttribute{baseName, name});
          PRINTER->queue.push(
              Work{work.object.get_attribute(name), baseName, name});
        }
        os << "}";
      }
      PRINTER->printed.try_emplace(id(work.object), baseName);
      os << "});";
      if (PRINTER->wanted.count(id(work.object)) != 0) {
        auto setAttributes = std::move(PRINTER->wanted.at(id(work.object)));
        PRINTER->wanted.erase(id(work.object));
        std::sort(setAttributes.begin(), setAttributes.end(), Compare{});
        for (const auto &setAttribute : setAttributes) {
          os << setAttribute.base_name << ".set_attribute("
             << std::quoted(setAttribute.name) << "s,"
             << PRINTER->printed.at(id(work.object)) << ");";
        }
      }
      return os;
    }

    static void main() {
      object::Object builtins;
      virtual_machine::modules::init(builtins);
      std::atomic_flag sigInt{};
      sigInt.test_and_set();
      virtual_machine::GlobalContext globalContext{
          {},
          builtins,
          builtins.get_attribute("type")
              .get_attribute("__dir__")
              .get_attribute("__class__")
              .id(),
          builtins.get_attribute("compile").get_attribute("__class__").id(),
          &sigInt};
      virtual_machine::ProcessContext processContext{globalContext};
      auto module = processContext.parse_file(&std::cin, "<input>");
      virtual_machine::ThreadContext threadContext{
          processContext, processContext.make_module("builtins")};
      threadContext.evaluate(module);
      std::cout
          << "//! generated file see tools/builtins.cpp\n\n"
             "#include \"virtual_machine/builtins.hpp\"\n\n"
             "#include \"object/object.hpp\"\n\n"
             "using namespace std::literals;\n\n"
             "namespace chimera {\nnamespace library {\n"
             "namespace virtual_machine {\nnamespace modules {\n"
             "void init(const object::Object &module) {auto builtins = module;";
      Printer mainPrinter{{}, {{id(threadContext.main), "builtins"}}, {}, {}};
      PRINTER = &mainPrinter;
      remap(threadContext.main,
            threadContext.main.get_attribute("__builtins__"));
      for (const auto &name : threadContext.main.dir()) {
        if (is_printed(threadContext.main.get_attribute(name))) {
          std::cout << "builtins.set_attribute(" << std::quoted(name) << "s,"
                    << mainPrinter.printed.at(
                           id(threadContext.main.get_attribute(name)))
                    << ");";
        } else {
          mainPrinter.wanted[id(threadContext.main.get_attribute(name))]
              .push_back(SetAttribute{"builtins", name});
          mainPrinter.queue.push(
              Work{threadContext.main.get_attribute(name), "builtins", name});
        }
      }
      while (!mainPrinter.queue.empty()) {
        auto work = mainPrinter.queue.top();
        mainPrinter.queue.pop();
        std::cout << work;
      }
      std::cout << "}\n}\n}\n}\n}" << std::endl;
    }
  } // namespace library
} // namespace chimera

int main() {
  chimera::library::main();
  return 0;
}
