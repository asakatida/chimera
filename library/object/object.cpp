//! Python object as a c++ struct

#include "object.hpp"

#include <limits>
#include <string>
#include <utility>

using namespace std::literals;

namespace chimera::library::object::internal {
  BaseException::BaseException(std::string anException)
      : exception(ObjectRef(std::move(anException), {})) {}
  BaseException::BaseException(ObjectRef anException)
      : exception(std::move(anException)) {}
  BaseException::BaseException(const BaseException &anException,
                               const BaseException &context)
      : exception(anException.exception) {
    ObjectRef object(anException.exception);
    object.set_attribute("__context__"s, context.exception);
  }
  auto BaseException::class_id() const noexcept -> Id {
    return exception.get_attribute("__class__"s).id();
  }
  auto BaseException::id() const noexcept -> Id { return exception.id(); }
  auto BaseException::what() const noexcept -> const char * {
    return "BaseException";
  }
  // NOLINTBEGIN(bugprone-throw-keyword-missing)
  AttributeError::AttributeError(const std::string &type,
                                 const std::string &key)
      : BaseException("AttributeError: '"s + type + "' has no attribute '" +
                      key + "'") {}
  KeyboardInterrupt::KeyboardInterrupt()
      : BaseException("KeyboardInterrupt"s) {}
  // NOLINTEND(bugprone-throw-keyword-missing)
} // namespace chimera::library::object::internal
