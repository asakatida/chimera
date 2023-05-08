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

#include "modules.hpp"

#include "asdl/asdl.hpp"
#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"

#include <gsl/gsl>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace chimera::library {
  [[nodiscard]] auto PrintState::printed(const object::Object &object)
      -> std::string {
    if (!m_printed.contains(id(object))) {
      return "";
    }
    return m_printed.at(id(object));
  }
  [[nodiscard]] auto PrintState::id(const object::Object &object)
      -> object::Id {
    return m_remap.try_emplace(object.id(), object.id()).first->second;
  }
  void PrintState::remap(const object::Object &module,
                         const object::Object &previous) {
    std::vector modules{std::tuple{module, previous}};
    while (!modules.empty()) {
      std::vector<std::tuple<object::Object, object::Object>> next_modules{};
      for (const auto &work : modules) {
        if (m_remap.try_emplace(std::get<1>(work).id(), std::get<0>(work).id())
                .second) {
          for (const auto &name : std::get<0>(work).dir()) {
            if (std::get<1>(work).has_attribute(name)) {
              next_modules.emplace_back(std::get<0>(work).get_attribute(name),
                                        std::get<1>(work).get_attribute(name));
            }
          }
          for (const auto &name : std::get<1>(work).dir()) {
            if (std::get<0>(work).has_attribute(name)) {
              next_modules.emplace_back(std::get<0>(work).get_attribute(name),
                                        std::get<1>(work).get_attribute(name));
            }
          }
        }
      }
      modules = next_modules;
    }
  }
  [[nodiscard]] auto PrintState::is_printed(const object::Object &object)
      -> bool {
    return m_printed.contains(id(object));
  }
  [[nodiscard]] auto Compare::operator()(const SetAttribute &left,
                                         const SetAttribute &right) const
      -> bool {
    if (left.base_name == right.base_name) {
      return left.name < right.name;
    }
    return left.base_name < right.base_name;
  }
  [[nodiscard]] auto Compare::operator()(const Work &left,
                                         const Work &right) const -> bool {
    if (left.printer->is_printed(right.object) &&
        !right.printer->is_printed(left.object)) {
      return true;
    }
    if (right.printer->is_printed(left.object) &&
        !left.printer->is_printed(right.object)) {
      return false;
    }
    if (left.base_name == right.base_name) {
      return left.name > right.name;
    }
    return left.base_name > right.base_name;
  }
  void IncompleteTuple::operator()(const object::Tuple &tuple) {
    for (const auto &object : tuple) {
      if (!printer->is_printed(object)) {
        printer->tuple_want = object;
        return;
      }
    }
  }
} // namespace chimera::library
