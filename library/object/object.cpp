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

#include <string>
#include <utility>

using namespace std::literals;

namespace chimera {
  namespace library {
    namespace object {
      String::String(std::string_view string) : value(string){};

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
        std::vector<std::string> keys;
        keys.reserve(read.value.size());
        for (const auto &pair : read.value) {
          keys.push_back(pair.first);
        }
        return keys;
      }
      Object Object::get_attribute(std::string &&key) const {
        try {
          return object->attributes.at(key);
        } catch (...) {
        }
        return {};
      }
      Object Object::get_attribute(const std::string &key) const {
        try {
          return object->attributes.at(key);
        } catch (...) {
        }
        return {};
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

      BaseException::BaseException(const Object &anException) : exception(anException) {}

      BaseException::BaseException(const BaseException &anException, const std::optional<object::BaseException> &context) : exception(anException.exception) {
        if (context) {
          exception.set_attribute("__context__"s, context->exception);
        }
      }

      const char *BaseException::what() const noexcept {
        return "BaseException";
      }

      Id BaseException::id() const noexcept {
        return exception.id();
      }

      Id BaseException::class_id() const noexcept {
        return exception.get_attribute("__class__"s).id();
      }
    } // namespace object
  }   // namespace library
} // namespace chimera
