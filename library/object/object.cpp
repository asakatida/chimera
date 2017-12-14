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
      Object::Object(Value &&value, std::map<std::string, Object> &&attributes)
          : object(std::make_shared<Impl>(
                Impl{std::move(value), {std::move(attributes)}})) {}

      void Object::delete_attribute(std::string &&key) noexcept {
        object->attributes.erase(key);
      }
      void Object::delete_attribute(const std::string &key) noexcept {
        object->attributes.erase(key);
      }
      std::vector<std::string> Object::dir() const {
        auto read = object->attributes.read();
        std::vector<std::string> keys(read.value.size());
        std::transform(read.value.cbegin(), read.value.cend(), keys.begin(),
                       [](const auto &pair) { return pair.first; });
        return keys;
      }
      const Object &Object::get_attribute(std::string &&key) const {
        return object->attributes.at(key);
      }
      const Object &Object::get_attribute(const std::string &key) const {
        return object->attributes.at(key);
      }
      Object Object::get_attribute(std::string &&key) {
        return object->attributes.at(key);
      }
      Object Object::get_attribute(const std::string &key) {
        return object->attributes.at(key);
      }
      bool Object::has_attribute(std::string &&key) const noexcept {
        return object->attributes.count(key) != 0;
      }
      bool Object::has_attribute(const std::string &key) const noexcept {
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
        auto read = object->attributes.read();
        auto attributes = read.value;
        return Object(std::move(data), std::move(attributes));
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
