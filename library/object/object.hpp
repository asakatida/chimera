//! Python object as a c++ struct

#pragma once

#include <exception>
#include <future>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "asdl/asdl.hpp"
#include "object/number/number.hpp"

namespace chimera {
  namespace library {
    namespace object {
      using number::number;
      struct Object;

      using Id = std::uint64_t;

      struct Instance {};
      using Bytes = std::vector<std::uint8_t>;
      enum class BytesMethod {};
      struct False {};
      using Future = std::future<Object>;
      struct None {};
      struct NullFunction {};
      using number::Number;
      enum class NumberMethod {};
      enum class ObjectMethod { DELATTR, DIR, GETATTRIBUTE, SETATTR };
      using String = std::string;
      enum class StringMethod {};
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
            std::variant<Instance, Bytes, BytesMethod, False, Future, None,
                         NullFunction, Number, NumberMethod, ObjectMethod,
                         String, StringMethod, SysCall, True, Tuple,
                         TupleMethod, asdl::StmtImpl, asdl::ExprImpl>;
        using Attributes = std::map<std::string, Object>;

        Object();
        Object(Value &&value, Attributes &&attributes);

        Object(const Object &other) = default;
        Object(Object &&other) noexcept = default;

        ~Object() noexcept = default;

        Object &operator=(const Object &other) = default;
        Object &operator=(Object &&other) noexcept = default;

        void delete_attribute(std::string &&key) noexcept;
        void delete_attribute(const std::string &key) noexcept;
        std::vector<std::string> dir() const;
        const Object &get_attribute(std::string &&key) const;
        const Object &get_attribute(const std::string &key) const;
        Object &get_attribute(std::string &&key);
        Object &get_attribute(const std::string &key);
        bool has_attribute(std::string &&key) const noexcept;
        bool has_attribute(const std::string &key) const noexcept;
        template <typename Value>
        void set_attribute(std::string &&key, Value &&value) {
          std::unique_lock<std::shared_mutex> lock(object->mutex);
          object->attributes.insert_or_assign(std::move(key),
                                              std::forward<Value>(value));
        }
        template <typename Value>
        void set_attribute(const std::string &key, Value &&value) {
          std::unique_lock<std::shared_mutex> lock(object->mutex);
          object->attributes.insert_or_assign(key, std::forward<Value>(value));
        }

        Id id() const noexcept;

        const Value &value() const noexcept;

        Object copy(Value &&data) const;

        bool get_bool() const noexcept;

        auto use_count() const noexcept { return object.use_count(); }

      private:
        struct Impl {
          Value value;
          Attributes attributes;
          std::shared_mutex mutex{}; //! only needs to guard attributes
        };
        std::shared_ptr<Impl> object;
      };

      struct BaseException final : std::exception {
        explicit BaseException(const Object &anException);
        explicit BaseException(Object &&anException);
        const char *what() const noexcept override;
        Object exception;
      };
    } // namespace object
  }   // namespace library
} // namespace chimera
