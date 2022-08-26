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

namespace chimera::library::object::internal {
  struct BaseWhat {
    auto operator()(const std::string_view &what) const noexcept -> const
        char * {
      return what.cbegin();
    }
    template <typename Type>
    auto operator()(const Type & /*what*/) const noexcept -> const char * {
      return "BaseException";
    }
  };
  BaseException::BaseException(std::string &&anException)
      : exception(ObjectRef(std::move(anException), {})) {}
  BaseException::BaseException(WeakObject &&anException)
      : exception(std::move(anException)) {}
  BaseException::BaseException(const BaseException &anException,
                               const BaseException &context)
      : exception(anException.exception) {
    ObjectRef object = anException.exception;
    object.set_attribute("__context__"s, context.exception);
  }
  BaseException::BaseException(const BaseException &other) noexcept
      : exception(other.exception) {}
  auto BaseException::operator=(const BaseException &other) noexcept
      -> BaseException & {
    exception = other.exception;
    return *this;
  }
  auto BaseException::class_id() const noexcept -> Id {
    return exception.get_attribute("__class__"s).id();
  }
  auto BaseException::id() const noexcept -> Id { return exception.id(); }
  auto BaseException::what() const noexcept -> const char * {
    return exception.visit(BaseWhat{});
  }
} // namespace chimera::library::object::internal
