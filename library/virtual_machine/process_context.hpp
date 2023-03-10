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

#pragma once

#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

#include "asdl/asdl.hpp"
#include "container/atomic_map.hpp"
#include "object/object.hpp"
#include "virtual_machine/garbage.hpp"
#include "virtual_machine/global_context.hpp"

namespace chimera::library::virtual_machine {
  struct ProcessContext {
    explicit ProcessContext(const GlobalContext &global_context);
    ~ProcessContext() noexcept;
    [[nodiscard]] auto builtins() const -> const object::Object &;
    auto import_object(std::string_view &&name,
                       std::string_view &&relativeModule)
        -> const object::Object &;
    auto import_module(std::string &&module) -> const asdl::Module &;
    auto make_module(std::string_view &&name) -> object::Object;
    auto parse_expression(const std::string_view &data,
                          const char *source) const -> asdl::Expression;
    auto parse_expression(std::istream &&input, const char *source) const
        -> asdl::Expression;
    auto parse_file(const std::string_view &data, const char *source) const
        -> asdl::Module;
    auto parse_file(std::istream &&input, const char *source) -> asdl::Module;
    auto parse_input(const std::string_view &data, const char *source) const
        -> asdl::Interactive;
    auto parse_input(std::istream &&input, const char *source)
        -> asdl::Interactive;
    void process_interrupts() const;

  private:
    const object::Object builtins_;
    const GlobalContext &global_context;
    // TODO(asakatida)
    // GarbageCollector garbage_collector{};
    container::AtomicMap<std::string, object::Object> modules{};
    auto import_module(const std::string_view &path, const std::string &module)
        -> asdl::Module;
    auto import_object(std::string_view &&module) -> const object::Object &;
  };
} // namespace chimera::library::virtual_machine
