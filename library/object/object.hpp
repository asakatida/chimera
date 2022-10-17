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

namespace chimera::library::object {
  struct Object;
  using Id = std::uint64_t;
  struct Instance {};
  using Bytes = std::vector<std::uint8_t>;
  enum class BytesMethod {};
  struct Expr {};
  struct False {};
  using Future = std::unique_ptr<std::future<Object>>;
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
  using Tuple = std::vector<Object>;
  enum class TupleMethod {};
  struct True {};
  struct Object {
    using Value =
        std::variant<Instance, Bytes, BytesMethod, Expr, False, Future, None,
                     NullFunction, Number, NumberMethod, ObjectMethod, Stmt,
                     String, StringMethod, SysCall, True, Tuple, TupleMethod>;
    using Attributes = container::AtomicMap<std::string, Object>;
    Object();
    Object(Value &&value, std::map<std::string, Object> &&attributes);
    void delete_attribute(std::string &&key) noexcept;
    void delete_attribute(const std::string &key) noexcept;
    [[nodiscard]] auto dir() const -> std::vector<std::string>;
    [[nodiscard]] auto get_attribute(const std::string &key) const
        -> const Object &;
    auto has_attribute(std::string &&key) const noexcept -> bool;
    [[nodiscard]] auto has_attribute(const std::string &key) const noexcept
        -> bool;
    template <typename... Args>
    void set_attribute(Args &&...args) {
      object->attributes.insert_or_assign(std::forward<Args>(args)...);
    }
    [[nodiscard]] auto id() const noexcept -> Id;
    template <typename Type>
    [[nodiscard]] auto get() const -> std::optional<Type> {
      if (std::holds_alternative<Type>(object->value)) {
        return std::get<Type>(object->value);
      }
      return {};
    }
    [[nodiscard]] auto get_bool() const noexcept -> bool;
    [[nodiscard]] auto use_count() const noexcept { return object.use_count(); }
    template <typename Visitor>
    void visit(Visitor &&visitor) const {
      std::visit(std::forward<Visitor>(visitor), object->value);
    }

  private:
    struct Impl {
      Value value;
      Attributes attributes;
    };
    std::shared_ptr<Impl> object;
  };
  class BaseException : virtual public std::exception {
  public:
    explicit BaseException(Object anException);
    BaseException(const BaseException &anException,
                  const BaseException &context);
    [[nodiscard]] auto what() const noexcept -> const char * override;
    [[nodiscard]] auto id() const noexcept -> Id;
    [[nodiscard]] auto class_id() const noexcept -> Id;

  private:
    Object exception;
  };
  class AttributeError final : virtual public BaseException {
  public:
    AttributeError(const std::string &type, const std::string &key);
  };
  class KeyboardInterrupt final : virtual public BaseException {
  public:
    KeyboardInterrupt();
  };
} // namespace chimera::library::object
