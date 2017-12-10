//! Python object as a c++ struct

#include "object.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <string>

namespace chimera {
  namespace library {
    namespace object {
      Object::Object() : object(std::make_shared<Impl>()) {}
      Object::Object(Value &&value, Attributes &&attributes) : Object() {
        object->value = std::move(value);
        object->attributes = std::move(attributes);
      }

      void Object::delete_attribute(std::string &&key) noexcept {
        std::unique_lock<std::shared_mutex> lock(object->mutex);
        object->attributes.erase(key);
      }
      void Object::delete_attribute(const std::string &key) noexcept {
        std::unique_lock<std::shared_mutex> lock(object->mutex);
        object->attributes.erase(key);
      }
      std::vector<std::string> Object::dir() const {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        std::vector<std::string> keys(object->attributes.size());
        std::transform(object->attributes.cbegin(), object->attributes.cend(),
                       keys.begin(),
                       [](const auto &pair) { return pair.first; });
        return keys;
      }
      const Object &Object::get_attribute(std::string &&key) const {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        return object->attributes.at(key);
      }
      const Object &Object::get_attribute(const std::string &key) const {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        return object->attributes.at(key);
      }
      Object &Object::get_attribute(std::string &&key) {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        return object->attributes.at(key);
      }
      Object &Object::get_attribute(const std::string &key) {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        return object->attributes.at(key);
      }
      bool Object::has_attribute(std::string &&key) const noexcept {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        return object->attributes.count(key) != 0;
      }
      bool Object::has_attribute(const std::string &key) const noexcept {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        return object->attributes.count(key) != 0;
      }
      Id Object::id() const noexcept {
        return std::hash<std::shared_ptr<Impl>>{}(
            object)&std::numeric_limits<Id>::max();
      }
      const Object::Value &Object::value() const noexcept {
        return object->value;
      }
      Object Object::copy(Value &&data) const {
        std::shared_lock<std::shared_mutex> lock(object->mutex);
        return Object(std::move(data), Attributes(object->attributes));
      }
      bool Object::get_bool() const noexcept {
        return std::holds_alternative<object::True>(object->value);
      }

      BaseException::BaseException(const Object &anException)
          : exception(anException) {}
      BaseException::BaseException(Object &&anException)
          : exception(std::move(anException)) {}
      const char *BaseException::what() const noexcept {
        return "BaseException";
      }
    } // namespace object
  }   // namespace library
} // namespace chimera
