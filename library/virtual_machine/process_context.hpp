//! process context attached to a virtual machine thread

#pragma once

#include "asdl/asdl.hpp"
#include "container/atomic_map.hpp"
#include "object/object.hpp"
#include "virtual_machine/garbage.hpp"
#include "virtual_machine/global_context.hpp"

#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

namespace chimera::library::virtual_machine {
  struct ProcessContext {
    explicit ProcessContext(GlobalContext &global_context);
    ProcessContext(const ProcessContext &) = delete;
    ProcessContext(ProcessContext &&) noexcept = delete;
    ~ProcessContext() noexcept;
    auto operator=(const ProcessContext &) -> ProcessContext & = delete;
    auto operator=(ProcessContext &&) noexcept -> ProcessContext & = delete;
    [[nodiscard]] auto builtins() const -> const object::Object &;
    [[nodiscard]] auto import_object(std::string_view &&name,
                                     std::string_view &&relativeModule)
        -> const object::Object &;
    [[nodiscard]] auto import_module(std::string &&module) -> asdl::Module;
    [[nodiscard]] auto make_module(std::string_view &&name) -> object::Object;
    [[nodiscard]] auto parse_expression(std::istream &input,
                                        const char *source) const
        -> asdl::Expression;
    [[nodiscard]] auto parse_file(std::istream &input, const char *source) const
        -> asdl::Module;
    [[nodiscard]] auto parse_input(std::istream &input,
                                   const char *source) const
        -> asdl::Interactive;
    void process_interrupts() const;

  private:
    [[nodiscard]] auto import_module(const std::string_view &path,
                                     const std::string &module) -> asdl::Module;
    [[nodiscard]] auto import_object(std::string_view &&request_module)
        -> const object::Object &;
    object::Object builtins_;
    gsl::not_null<GlobalContext *> global_context;
    // TODO(asakatida)
    // GarbageCollector garbage_collector{};
    container::AtomicMap<std::string, object::Object> modules;
  };
} // namespace chimera::library::virtual_machine
