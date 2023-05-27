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

#include "object/object.hpp" // for SysCall, Object, Id, Obj...

#include <gsl/assert> // for Expects

#include <algorithm> // for sort
#include <iomanip>   // for quoted
#include <map>       // for map
#include <optional>  // for optional
#include <ostream>   // for endl
#include <queue>     // for priority_queue
#include <string>    // for basic_string, to_string
#include <vector>    // for vector

namespace chimera::library {
  namespace object::number {
    class Number;
  } // namespace object::number
  struct PrintState;
} // namespace chimera::library

namespace chimera::library {
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
    [[nodiscard]] auto operator()(const SetAttribute &left,
                                  const SetAttribute &right) const -> bool;
    [[nodiscard]] auto operator()(const Work &left, const Work &right) const
        -> bool;
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
    [[nodiscard]] auto id(const object::Object &object) -> object::Id;
    void remap(const object::Object &module, const object::Object &previous);
    [[nodiscard]] auto is_printed(const object::Object &object) -> bool;
    template <typename OStream>
    auto print(OStream &ostream, const object::Instance & /*instance*/)
        -> OStream & {
      return ostream;
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::Bytes &bytes) -> OStream & {
      ostream << "object::Bytes{";
      bool first = true;
      for (const auto &byte : bytes) {
        if (!first) {
          ostream << ",";
        } else {
          first = false;
        }
        ostream << byte;
      }
      return ostream << "},";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::BytesMethod &bytesMethod)
        -> OStream & {
      ostream << "object::BytesMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (bytesMethod) {}
      return ostream << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::Expr &expr) -> OStream & {
      return ostream << &expr << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::False & /*false*/) -> OStream & {
      return ostream << "object::False{},";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::Future & /*future*/)
        -> OStream & {
      Expects(false);
      return ostream;
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::None & /*none*/) -> OStream & {
      return ostream << "object::None{},";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::NullFunction & /*nullFunction*/)
        -> OStream & {
      return ostream << "object::NullFunction{},";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::Number &number) -> OStream & {
      return number.repr(ostream) << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::NumberMethod &numberMethod)
        -> OStream & {
      ostream << "object::NumberMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (numberMethod) {}
      return ostream << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::ObjectMethod &objectMethod)
        -> OStream & {
      ostream << "object::ObjectMethod::";
      switch (objectMethod) {
        case object::ObjectMethod::DELATTR:
          return ostream << "DELATTR";
        case object::ObjectMethod::DIR:
          return ostream << "DIR";
        case object::ObjectMethod::GETATTRIBUTE:
          return ostream << "GETATTRIBUTE";
        case object::ObjectMethod::SETATTR:
          return ostream << "SETATTR";
      }
      return ostream << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::Stmt &stmt) -> OStream & {
      return ostream << &stmt << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::String &string) -> OStream & {
      return ostream << "object::String(" << std::quoted(string) << "s),";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::StringMethod &stringMethod)
        -> OStream & {
      ostream << "object::StringMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (stringMethod) {}
      return ostream << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::True & /*true*/) -> OStream & {
      return ostream << "object::True{},";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::Tuple &tuple) -> OStream & {
      ostream << "object::Tuple{";
      bool first = true;
      for (const auto &object : tuple) {
        if (!first) {
          ostream << ",";
        } else {
          first = false;
        }
        ostream << printed(object);
      }
      return ostream << "},";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::TupleMethod &tupleMethod)
        -> OStream & {
      ostream << "object::TupleMethod::";
      // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
      switch (tupleMethod) {}
      return ostream << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const object::SysCall &sysCall) -> OStream & {
      ostream << "object::SysCall::";
      switch (sysCall) {
        case object::SysCall::COMPILE:
          return ostream << "COMPILE";
        case object::SysCall::EVAL:
          return ostream << "EVAL";
        case object::SysCall::EXEC:
          return ostream << "EXEC";
        case object::SysCall::GLOBALS:
          return ostream << "GLOBALS";
        case object::SysCall::ID:
          return ostream << "ID";
        case object::SysCall::INPUT:
          return ostream << "INPUT";
        case object::SysCall::LOCALS:
          return ostream << "LOCALS";
        case object::SysCall::PRINT:
          return ostream << "PRINT";
        case object::SysCall::OPEN:
          return ostream << "OPEN";
      }
      return ostream << ",";
    }
    template <typename OStream>
    auto print(OStream &ostream, const Work &work) -> OStream & {
      auto baseName = std::string(work.base_name).append("_").append(work.name);
      work.object.visit(IncompleteTuple(this));
      while (tuple_want) {
        print(ostream, Work{this, *tuple_want, baseName,
                            std::to_string(m_printed.size())});
        work.object.visit(IncompleteTuple(this));
      }
      if (is_printed(work.object)) {
        return ostream;
      }
      ostream << "object::Object " << baseName << "(";
      work.object.visit(
          [this, &ostream](auto &&value) { this->print(ostream, value); });
      ostream << "{";
      bool first = true;
      for (const auto &name : work.object.dir()) {
        if (!first) {
          ostream << ",";
        } else {
          first = false;
        }
        ostream << "{" << std::quoted(name) << "s,";
        if (is_printed(work.object.get_attribute(name))) {
          ostream << printed(work.object.get_attribute(name));
        } else {
          ostream << "{/*set below*/}";
          wanted[id(work.object.get_attribute(name))].emplace_back(
              SetAttribute{baseName, name});
          queue.push(
              Work{this, work.object.get_attribute(name), baseName, name});
        }
        ostream << "}";
      }
      m_printed.try_emplace(id(work.object), baseName);
      ostream << "});";
      if (wanted.contains(id(work.object))) {
        auto setAttributes = std::move(wanted.at(id(work.object)));
        wanted.erase(id(work.object));
        std::sort(setAttributes.begin(), setAttributes.end(), Compare{});
        for (const auto &setAttribute : setAttributes) {
          ostream << setAttribute.base_name << ".set_attribute("
                  << std::quoted(setAttribute.name) << "s,"
                  << printed(work.object) << ");";
        }
      }
      return ostream;
    }
    [[nodiscard]] auto main_printed() -> std::string { return printed(main); }
    template <typename OStream>
    void print_all(OStream &ostream) {
      for (const auto &name : main.dir()) {
        if (is_printed(main.get_attribute(name))) {
          ostream << "module.set_attribute(" << std::quoted(name) << "s,"
                  << printed(main.get_attribute(name)) << ");";
        } else {
          wanted[id(main.get_attribute(name))].emplace_back(
              SetAttribute{"module", name});
          queue.push(Work{this, main.get_attribute(name), "module", name});
        }
      }
      while (!queue.empty()) {
        auto work = queue.top();
        queue.pop();
        print(ostream, work);
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
  auto operator<<(OStream &ostream, const Printer &printer) -> OStream & {
    PrintState state(printer.main, printer.module);
    if (printer.remap) {
      state.remap(printer.main, *printer.remap);
    }
    auto moduleName = state.main_printed();
    ostream << "//! generated file see tools/modules/" << moduleName
            << ".cpp\n\n"
               "#include \""
            << moduleName << "/" << moduleName
            << ".hpp\"\n\n"
               "#include \"object/object.hpp\"\n\n"
               "using namespace std::literals;\n\n"
               "namespace chimera::library::virtual_machine::modules {\n"
               // TODO(asakatida): this can be determined at print time
               "// NOLINTBEGIN(misc-const-correctness)\n"
               "void "
            << moduleName << "(const object::Object &" << moduleName
            << ") {auto module = " << moduleName << ";";
    state.print_all(ostream);
    return ostream
           << "// NOLINTEND(misc-const-correctness)\n"
           << "} // namespace chimera::library::virtual_machine::modules"
           << std::endl;
  }
} // namespace chimera::library
