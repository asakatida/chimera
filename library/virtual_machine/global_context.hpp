//! global context singleton

#pragma once

#include "object/object.hpp"
#include "options.hpp"

#include <atomic>

namespace chimera::library::virtual_machine {
  struct GlobalContextImpl : std::enable_shared_from_this<GlobalContextImpl> {
    explicit GlobalContextImpl(Options options);
    [[nodiscard]] auto debug() const -> bool;
    [[nodiscard]] auto interactive() -> int;
    [[nodiscard]] auto execute_script() -> int;
    [[nodiscard]] auto execute_script_string() -> int;
    [[nodiscard]] auto execute_script_input() -> int;
    [[nodiscard]] auto execute_module() -> int;
    [[nodiscard]] auto optimize() const -> const options::Optimize &;
    void process_interrupts() const;
    void sys_argv(const object::Object &module) const;
    [[nodiscard]] auto verbose_init() const -> const options::VerboseInit &;

  private:
    [[nodiscard]] auto execute(std::istream &istream, const char *source)
        -> int;
    Options options;
    std::atomic_flag *sig_int;
  };
  using GlobalContext = std::shared_ptr<GlobalContextImpl>;
  auto make_global(Options options) -> GlobalContext;
} // namespace chimera::library::virtual_machine
