//! used to initialize the global builtins module.

#pragma once

#include "object/object.hpp"

namespace chimera::library::virtual_machine::modules {
  void builtins(const object::Object &builtins);
} // namespace chimera::library::virtual_machine::modules
