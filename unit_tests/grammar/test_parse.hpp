#pragma once

#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"
#include "virtual_machine/thread_context.hpp"

namespace chimera::library {
  template <typename... Data>
  void test_parse(Data &&...data) {
    const Options options{.chimera = "chimera",
                          .exec = options::Script{"unit_test.py"}};
    virtual_machine::GlobalContext globalContext(options);
    virtual_machine::ProcessContext processContext{globalContext};
    std::istringstream input(std::forward<Data>(data)...);
    auto module = processContext.parse_file(std::move(input), "<unit_test>");
    virtual_machine::ThreadContext threadContext{
        processContext, processContext.make_module("__main__")};
    threadContext.evaluate(module);
  }
} // namespace chimera::library
