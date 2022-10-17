// Copyright (c) 2018 Asa Katida <github@holomaplefeline.net>
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

//! evaluates importlib to construct the importlib module.
//! Then prints the module construction.

#include <algorithm>
#include <iomanip>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "asdl/asdl.hpp"
#include "object/number/repr.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"

namespace chimera::library {
  struct PrintState;
  struct SetAttribute {
    std::string base_name{};
    std::string name{};
  };
  struct Work {
    PrintState *printer{};
    object::Object object;
    std::string base_name{};
    std::string name{};
  };
  struct Compare {
    auto operator()(const SetAttribute &a, const SetAttribute &b) const -> bool;
    auto operator()(const Work &a, const Work &b) const -> bool;
  };
  struct IncompleteTuple {
    explicit IncompleteTuple(PrintState *printer) : printer(printer) {}
    void operator()(const object::Tuple &tuple);
    template <typename Type>
    void operator()(const Type & /*type*/) const {}

  private:
    PrintState *printer;
  };
  struct PrintState {
    PrintState(const object::Object &main, const std::string &module)
        : main(main) {
      m_printed.try_emplace(id(main), module);
    }
    auto printed(const object::Object &object) -> std::string;
    auto id(const object::Object &object) -> object::Id;
    void remap(const object::Object &module, const object::Object &previous);
    auto is_printed(const object::Object &object) -> bool;
    template <typename OStream>
    auto print(OStream &os, const object::Instance & /*instance*/)
        -> OStream & {
      return os;
    }
    template <typename OStream>
    auto print(OStream &os, const object::Bytes &bytes) -> OStream & {
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
      return os << "},";
    }
    template <typename OStream>
    auto print(OStream &os, const object::BytesMethod &bytesMethod)
        -> OStream & {
      os << "object::BytesMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (bytesMethod) {}
      return os << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::Expr &expr) -> OStream & {
      return os << &expr << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::False & /*false*/) -> OStream & {
      return os << "object::False{},";
    }
    template <typename OStream>
    auto print(OStream &os, const object::Future & /*future*/) -> OStream & {
      Expects(false);
      return os;
    }
    template <typename OStream>
    auto print(OStream &os, const object::None & /*none*/) -> OStream & {
      return os << "object::None{},";
    }
    template <typename OStream>
    auto print(OStream &os, const object::NullFunction & /*nullFunction*/)
        -> OStream & {
      return os << "object::NullFunction{},";
    }
    template <typename OStream>
    auto print(OStream &os, const object::Number &number) -> OStream & {
      return number.repr(os) << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::NumberMethod &numberMethod)
        -> OStream & {
      os << "object::NumberMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (numberMethod) {}
      return os << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::ObjectMethod &objectMethod)
        -> OStream & {
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
      return os << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::Stmt &stmt) -> OStream & {
      return os << &stmt << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::String &string) -> OStream & {
      return os << "object::String(" << std::quoted(string) << "s),";
    }
    template <typename OStream>
    auto print(OStream &os, const object::StringMethod &stringMethod)
        -> OStream & {
      os << "object::StringMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (stringMethod) {}
      return os << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::True & /*true*/) -> OStream & {
      return os << "object::True{},";
    }
    template <typename OStream>
    auto print(OStream &os, const object::Tuple &tuple) -> OStream & {
      os << "object::Tuple{";
      bool first = true;
      for (const auto &object : tuple) {
        if (!first) {
          os << ",";
        } else {
          first = false;
        }
        os << printed(object);
      }
      return os << "},";
    }
    template <typename OStream>
    auto print(OStream &os, const object::TupleMethod &tupleMethod)
        -> OStream & {
      os << "object::TupleMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (tupleMethod) {}
      return os << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const object::SysCall &sysCall) -> OStream & {
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
      return os << ",";
    }
    template <typename OStream>
    auto print(OStream &os, const Work &work) -> OStream & {
      auto baseName = std::string(work.base_name).append("_").append(work.name);
      work.object.visit(IncompleteTuple(this));
      while (tuple_want) {
        print(os, Work{this, *tuple_want, baseName,
                       std::to_string(m_printed.size())});
        work.object.visit(IncompleteTuple(this));
      }
      if (is_printed(work.object)) {
        return os;
      }
      os << "object::Object " << baseName << "(";
      work.object.visit([this, &os](auto &&value) { this->print(os, value); });
      os << "{";
      bool first = true;
      for (const auto &name : work.object.dir()) {
        if (!first) {
          os << ",";
        } else {
          first = false;
        }
        os << "{" << std::quoted(name) << "s,";
        if (is_printed(work.object.get_attribute(name))) {
          os << printed(work.object.get_attribute(name));
        } else {
          os << "{/*set below*/}";
          wanted[id(work.object.get_attribute(name))].emplace_back(
              SetAttribute{baseName, name});
          queue.push(
              Work{this, work.object.get_attribute(name), baseName, name});
        }
        os << "}";
      }
      m_printed.try_emplace(id(work.object), baseName);
      os << "});";
      if (wanted.contains(id(work.object))) {
        auto setAttributes = std::move(wanted.at(id(work.object)));
        wanted.erase(id(work.object));
        std::sort(setAttributes.begin(), setAttributes.end(), Compare{});
        for (const auto &setAttribute : setAttributes) {
          os << setAttribute.base_name << ".set_attribute("
             << std::quoted(setAttribute.name) << "s," << printed(work.object)
             << ");";
        }
      }
      return os;
    }
    auto main_printed() -> std::string { return printed(main); }
    template <typename OStream>
    void print_all(OStream &os, const std::string &moduleName) {
      for (const auto &name : main.dir()) {
        if (is_printed(main.get_attribute(name))) {
          os << moduleName << ".set_attribute(" << std::quoted(name) << "s,"
             << printed(main.get_attribute(name)) << ");";
        } else {
          wanted[id(main.get_attribute(name))].emplace_back(
              SetAttribute{moduleName, name});
          queue.push(Work{this, main.get_attribute(name), moduleName, name});
        }
      }
      while (!queue.empty()) {
        auto work = queue.top();
        queue.pop();
        print(os, work);
      }
    }

  private:
    friend IncompleteTuple;
    std::map<object::Id, object::Id> m_remap{};
    std::map<object::Id, std::string> m_printed{};
    std::priority_queue<Work, std::vector<Work>, Compare> queue{};
    std::map<object::Id, std::vector<SetAttribute>> wanted{};
    std::optional<object::Object> tuple_want{};
    object::Object main;
  };
  struct Printer {
    object::Object main;
    std::string module{};
    std::optional<object::Object> remap{};
  };
  template <typename OStream>
  auto operator<<(OStream &os, const Printer &printer) -> OStream & {
    PrintState state(printer.main, printer.module);
    if (printer.remap) {
      state.remap(printer.main, *printer.remap);
    }
    auto moduleName = state.main_printed();
    os << "//! generated file see tools/" << moduleName
       << ".cpp\n\n"
          "#include \""
       << moduleName << "/" << moduleName
       << ".hpp\"\n\n"
          "#include \"object/object.hpp\"\n\n"
          "using namespace std::literals;\n\n"
          "namespace chimera::library::virtual_machine::modules {\n"
          "void "
       << moduleName << "(const object::Object &module) {auto " << moduleName
       << " = module;";
    state.print_all(os, moduleName);
    return os << "} // namespace chimera::library::virtual_machine::modules"
              << std::endl;
  }
} // namespace chimera::library
