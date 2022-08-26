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

#pragma once

#include <exception>
#include <future>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "container/atomic_map.hpp"
#include "object/number/number.hpp"
#include "object/reference.hpp"

namespace chimera::library::object::internal {
  using Id = std::uint64_t;
  struct Object;
  template <template <typename...> class Pointer>
  struct ObjectPointer {
    using BasicAttributes = std::map<std::string, ObjectPointer<Reference>>;
    ObjectPointer() = default;
    explicit ObjectPointer(BasicAttributes &&attributes)
        : object(std::move(attributes)) {}
    template <typename Type>
    ObjectPointer(Type &&value, BasicAttributes &&attributes)
        : object(std::move(attributes), std::forward<Type>(value)) {}
    template <template <typename...> class Other,
              typename = EnableIfMismatch<Pointer<void>, Other<void>>>
    ObjectPointer(const ObjectPointer<Other> &other) : object(other.object){};
    template <template <typename...> class Other,
              typename = EnableIfMismatch<Pointer<void>, Other<void>>>
    ObjectPointer(ObjectPointer<Other> &&other) noexcept
        : object(std::move(other.object)){};
    template <template <typename...> class Other,
              typename = EnableIfMismatch<Pointer<void>, Other<void>>>
    auto operator=(const ObjectPointer<Other> &other) -> ObjectPointer & {
      object = other.object;
      return *this;
    };
    template <template <typename...> class Other,
              typename = EnableIfMismatch<Pointer<void>, Other<void>>>
    auto operator=(ObjectPointer<Other> &&other) noexcept -> ObjectPointer & {
      object = std::move(other.object);
      return *this;
    };
    void delete_attribute(std::string &&key) noexcept {
      object->attributes.erase(key);
    }
    void delete_attribute(const std::string &key) noexcept {
      object->attributes.erase(key);
    }
    [[nodiscard]] auto dir() const -> std::vector<std::string> {
      auto read = object->attributes.read();
      std::vector<std::string> keys;
      keys.reserve(read.value.size());
      for (const auto &pair : read.value) {
        keys.emplace_back(pair.first);
      }
      return keys;
    }
    template <typename Type>
    [[nodiscard]] auto get() const noexcept -> std::optional<const Type> {
      if (auto *value = std::get_if<Type>(&object->value); value != nullptr) {
        return *value;
      }
      return {};
    }
    [[nodiscard]] auto get_attribute(std::string &&key) const -> ObjectPointer;
    [[nodiscard]] auto get_attribute(const std::string &key) const
        -> ObjectPointer;
    [[nodiscard]] auto get_bool() const noexcept -> bool;
    [[nodiscard]] auto has_attribute(std::string &&key) const noexcept -> bool {
      return object->attributes.count(key) != 0;
    }
    [[nodiscard]] auto has_attribute(const std::string &key) const noexcept
        -> bool {
      return object->attributes.count(key) != 0;
    }
    [[nodiscard]] auto id() const noexcept -> Id {
      using NumericLimits = std::numeric_limits<Id>;
      return NumericLimits::max();
    }
    template <typename... Args>
    void set_attribute(Args &&...args) {
      object->attributes.insert_or_assign(std::forward<Args>(args)...);
    }
    [[nodiscard]] auto use_count() const noexcept { return object.use_count(); }
    template <typename Visitor>
    auto visit(Visitor &&visitor) const -> decltype(auto) {
      return std::visit(std::forward<Visitor>(visitor), object->value);
    }

  private:
    friend ObjectPointer<Reference>;
    friend ObjectPointer<WeakReference>;
    friend ObjectPointer<StrongReference>;
    Pointer<Object> object;
  };
  using ObjectRef = ObjectPointer<Reference>;
  using WeakObject = ObjectPointer<WeakReference>;
  class BaseException;
  struct Instance {};
  using Bytes = std::vector<std::uint8_t>;
  enum class BytesMethod {};
  struct Expr {};
  struct False {};
  using Future = std::future<ObjectRef>;
  struct None {};
  struct NullFunction {};
  using number::Number;
  enum class NumberMethod {};
  enum class ObjectMethod { DELATTR, DIR, GETATTRIBUTE, SETATTR };
  using String = std::string_view;
  enum class StringMethod {};
  struct Stmt {};
  enum class SysCall {
    COMPILE,
    EVAL,
    EXEC,
    GLOBALS,
    ID,
    INPUT,
    LOCALS,
    PRINT,
    OPEN
  };
  enum class TupleMethod {};
  struct True {};
  using Tuple = std::vector<ObjectRef>;
  struct Object {
    using Value =
        std::variant<Instance, Bytes, BytesMethod, Expr, False, Future, None,
                     NullFunction, Number, NumberMethod, ObjectMethod, Stmt,
                     String, StringMethod, SysCall, True, Tuple, TupleMethod>;
    using BasicAttributes = std::map<std::string, ObjectRef>;
    using Attributes = container::AtomicMap<std::string, ObjectRef>;
    Object() = default;
    explicit Object(BasicAttributes &&attributes)
        : attributes(std::move(attributes)) {}
    template <typename Type>
    Object(BasicAttributes &&attributes, Type &&value)
        : attributes(std::move(attributes)), value(std::forward<Type>(value)) {}
    Object(const Object &other) = delete;
    Object(Object &&other) = delete;
    ~Object() noexcept = default;
    auto operator=(const Object &other) -> Object & = delete;
    auto operator=(Object &&other) noexcept -> Object & = delete;
    Attributes attributes;
    Value value;
  };
  class BaseException final : std::exception {
  public:
    BaseException() = default;
    explicit BaseException(std::string &&anException);
    explicit BaseException(WeakObject &&anException);
    BaseException(const BaseException &anException,
                  const BaseException &context);
    BaseException(const BaseException &other) noexcept;
    BaseException(BaseException &&other) noexcept = default;
    virtual ~BaseException() noexcept = default;
    auto operator=(const BaseException &other) noexcept -> BaseException &;
    auto operator=(BaseException &&other) noexcept -> BaseException & = default;
    [[nodiscard]] auto class_id() const noexcept -> Id;
    [[nodiscard]] auto id() const noexcept -> Id;
    [[nodiscard]] auto what() const noexcept -> const char * override;

  private:
    WeakObject exception;
  };
  template <template <typename...> class Pointer>
  [[nodiscard]] auto
  ObjectPointer<Pointer>::get_attribute(std::string &&key) const
      -> ObjectPointer<Pointer> {
    return get_attribute(key);
  }
  template <template <typename...> class Pointer>
  [[nodiscard]] auto
  ObjectPointer<Pointer>::get_attribute(const std::string &key) const
      -> ObjectPointer<Pointer> {
    if (!object->attributes.contains(key)) {
      throw std::runtime_error("AttributeError: type object '"
                               "object"
                               "' has no attribute '" +
                               key + "'");
    }
    return object->attributes.at(key);
  }
  template <template <typename...> class Pointer>
  [[nodiscard]] auto ObjectPointer<Pointer>::get_bool() const noexcept -> bool {
    return std::holds_alternative<True>(object->value);
  }
} // namespace chimera::library::object::internal
namespace chimera::library::object {
  using internal::BaseException;
  using internal::Bytes;
  using internal::BytesMethod;
  using internal::Expr;
  using internal::False;
  using internal::Future;
  using internal::Id;
  using internal::Instance;
  using internal::None;
  using internal::NullFunction;
  using internal::NumberMethod;
  using internal::ObjectMethod;
  using internal::Stmt;
  using internal::String;
  using internal::StringMethod;
  using internal::SysCall;
  using internal::True;
  using internal::Tuple;
  using internal::TupleMethod;
  using number::Number;
  using Object = internal::ObjectRef;
} // namespace chimera::library::object
namespace chimera {
  using library::object::Object;
} // namespace chimera
