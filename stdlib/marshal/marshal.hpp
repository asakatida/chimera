//! used to initialize the marshal module.

#pragma once

#include "object/object.hpp"
#include "options.hpp"

namespace chimera::library::virtual_machine::modules {
  void marshal(const object::Object &module);
} // namespace chimera::library::virtual_machine::modules
