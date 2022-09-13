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
  Object::Object(Value &&value, std::map<std::string, Object> &&attributes)
      : object(std::make_shared<Impl>(
            Impl{std::move(value), Attributes{std::move(attributes)}})) {}
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
  auto Object::get_attribute(std::string &&key) const -> Object {
    try {
      return object->attributes.at(key);
    } catch (...) {
    }
    return {};
  }
  auto Object::get_attribute(const std::string &key) const -> Object {
    try {
      return object->attributes.at(key);
    } catch (...) {
    }
    return {};
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
  auto Object::value() const noexcept -> const Object::Value & {
    return object->value;
  }
  auto Object::copy(Value &&data) const -> Object {
    auto read = object->attributes.read();
    auto attributes = read.value;
    return {std::move(data), std::move(attributes)};
  }
  auto Object::get_bool() const noexcept -> bool {
    return std::holds_alternative<True>(object->value);
  }
  BaseException::BaseException(Object anException)
      : exception(std::move(anException)) {}
  BaseException::BaseException(const BaseException &anException,
                               const BaseException &context)
      : exception(anException.exception) {
    exception.set_attribute("__context__"s, context.exception);
  }
  auto BaseException::what() const noexcept -> const char * {
    return "BaseException";
  }
  auto BaseException::id() const noexcept -> Id { return exception.id(); }
  auto BaseException::class_id() const noexcept -> Id {
    return exception.get_attribute("__class__"s).id();
  }
} // namespace chimera::library::object
