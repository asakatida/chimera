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

namespace chimera {
  namespace library {
    namespace virtual_machine {
      struct ProcessContext {
        constexpr static auto bufferSize =
            std::numeric_limits<std::uint16_t>::max();

        std::optional<asdl::Module> import_module(std::string &&module);

        object::Object &import_object(std::string_view &&name,
                                      std::string_view &&relativeModule);

        object::Object make_module(std::string_view &&name);

        asdl::Constant insert_constant(object::Bytes &&bytes);
        asdl::Constant insert_constant(object::Number &&number);
        asdl::Constant insert_constant(object::String &&string);

        asdl::Module parse_file(const std::string_view &data,
                                const char *source);
        asdl::Module parse_file(std::istream &input, const char *source);

        asdl::Interactive parse_input(const std::string_view &data,
                                      const char *source);
        asdl::Interactive parse_input(std::istream &input, const char *source);

        const GlobalContext &global_context;
        // TODO(asakatida)
        // GarbageCollector garbage_collector{};
        container::AtomicMap<object::Id, object::Object> constants{};
        container::AtomicMap<std::string, object::Object> modules{};

      private:
        object::Object &import_object(std::string_view &&module);

        std::optional<asdl::Module> import_module(const std::string_view &path,
                                                  const std::string &module);
      };
    } // namespace virtual_machine
  }   // namespace library
} // namespace chimera
