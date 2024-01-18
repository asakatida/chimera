//! used to initialize the sys module.

#pragma once

#include "object/object.hpp"
#include "options.hpp"

namespace chimera::library::virtual_machine::modules {
  void sys(const object::Object &module);
} // namespace chimera::library::virtual_machine::modules
