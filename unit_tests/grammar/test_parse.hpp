#pragma once

#include "object/object.hpp"
#include "options.hpp"
#include "virtual_machine/evaluator.hpp"
#include "virtual_machine/global_context.hpp"
#include "virtual_machine/process_context.hpp"
#include "virtual_machine/thread_context.hpp"

#include <sstream>

namespace chimera::library {
  template <typename... Data>
  void test_parse(Data &&...data) {
    const Options options{.chimera = "chimera",
                          .exec = options::Script{"unit_test.py"}};
    auto globalContext = virtual_machine::make_global(options);
    auto processContext = virtual_machine::make_process(globalContext);
    std::istringstream input(std::forward<Data>(data)...);
    auto module = processContext->parse_file(input, "<unit_test>");
    auto threadContext = virtual_machine::make_thread(
        processContext, processContext->make_module("__main__"));
    virtual_machine::Evaluator(threadContext).evaluate(module);
  }
} // namespace chimera::library
