// Copyright (c) 2017 Asa Katida <github@holomaplefeline.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//! Python object as a c++ struct

#include "object.hpp"

#include <limits>
#include <string>
#include <utility>

using namespace std::literals;

namespace chimera::library::object {
  Object::Object() : object(std::make_shared<Impl>()) {}
  Object::Object(std::map<std::string, Object> &&attributes)
      : object(std::make_shared<Impl>(
            Impl{{}, Attributes{std::move(attributes)}})) {}
  void Object::delete_attribute(std::string &&key) noexcept {
    object->attributes.erase(key);
  }
  void Object::delete_attribute(const std::string &key) noexcept {
    object->attributes.erase(key);
  }
  auto Object::dir() const -> std::vector<std::string> {
    auto read = object->attributes.read();
    std::vector<std::string> keys;
    keys.reserve(read.value.size());
    for (const auto &pair : read.value) {
      keys.emplace_back(pair.first);
    }
    return keys;
  }
  auto Object::dir_size() const -> std::vector<std::string>::size_type {
    return object->attributes.read().value.size();
  }
  auto Object::get_attribute(const std::string &key) const -> const Object & {
    if (object->attributes.contains(key)) {
      return object->attributes.at(key);
    }
    throw AttributeError("object", key);
  }
  auto Object::has_attribute(std::string &&key) const noexcept -> bool {
    return object->attributes.count(key) != 0;
  }
  auto Object::has_attribute(const std::string &key) const noexcept -> bool {
    return object->attributes.count(key) != 0;
  }
  auto Object::id() const noexcept -> Id {
    using NumericLimits = std::numeric_limits<Id>;
    return std::hash<std::shared_ptr<Impl>>{}(object)&NumericLimits::max();
  }
  auto Object::get_bool() const noexcept -> bool {
    return std::holds_alternative<True>(object->value);
  }
  struct BaseWhat {
    auto operator()(const object::String &what) const noexcept -> const char * {
      return what.c_str();
    }
    template <typename Type>
    auto operator()(const Type & /*what*/) const noexcept -> const char * {
      return "BaseException";
    }
  };
  BaseException::BaseException(String anException)
      : exception(Object(std::move(anException), {})) {}
  BaseException::BaseException(Object anException)
      : exception(std::move(anException)) {}
  BaseException::BaseException(const BaseException &anException,
                               const BaseException &context)
      : exception(anException.exception) {
    Object object(anException.exception);
    object.set_attribute("__context__"s, context.exception);
  }
  auto BaseException::class_id() const noexcept -> Id {
    return exception.get_attribute("__class__"s).id();
  }
  auto BaseException::id() const noexcept -> Id { return exception.id(); }
  auto BaseException::what() const noexcept -> const char * {
    return exception.visit(BaseWhat{});
  }
  // NOLINTBEGIN(bugprone-throw-keyword-missing)
  AttributeError::AttributeError(const std::string &type,
                                 const std::string &key)
      : BaseException(Object{"AttributeError: '"s + type +
                                 "' has no attribute '" + key + "'",
                             {}}) {}
  KeyboardInterrupt::KeyboardInterrupt()
      : BaseException(Object{"KeyboardInterrupt"s, {}}) {}
  // NOLINTEND(bugprone-throw-keyword-missing)
} // namespace chimera::library::object
