// Copyright (c) 2022 Asa Katida <github@holomaplefeline.net>
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

//! smart pointer abstraction for object reference counting

#pragma once

#include <memory>

#include <metal.hpp>

namespace chimera::library::object {
  namespace internal {
    template <typename Pointer>
    struct CopyReference {
      auto operator()(const Pointer &pointer) const noexcept
          -> const Pointer & {
        return pointer;
      }
    };
    template <typename Type>
    struct CopyReference<std::weak_ptr<Type>> {
      auto operator()(const std::weak_ptr<Type> &pointer) const noexcept
          -> std::shared_ptr<Type> {
        return pointer.lock();
      }
    };
    template <typename L, typename R>
    using EnableIfMismatch = std::enable_if_t<!std::is_same_v<L, R>>;
    template <template <typename...> class Pointer, typename Type>
    struct BaseReference {
      using RawPointer = std::add_pointer_t<Type>;
      BaseReference() = default;
      template <typename... Args>
      BaseReference(Args &&...args)
          : pointer(std::make_shared<Type>(std::forward<Args>(args)...)) {}
      template <
          template <typename...> class InterPointer, typename InterType,
          typename = EnableIfMismatch<Pointer<Type>, InterPointer<InterType>>>
      BaseReference(const BaseReference<InterPointer, InterType> &other)
          : pointer(other.pointer) {}
      template <
          template <typename...> class InterPointer, typename InterType,
          typename = EnableIfMismatch<Pointer<Type>, InterPointer<InterType>>>
      BaseReference(BaseReference<InterPointer, InterType> &&other) noexcept
          : pointer(std::move(other.pointer)) {}
      template <template <typename...> class InterPointer, typename InterType>
      auto operator=(const BaseReference<InterPointer, InterType> &other)
          -> BaseReference & {
        pointer = CopyReference<decltype(other.pointer)>{}(other.pointer);
        return *this;
      }
      template <
          template <typename...> class InterPointer, typename InterType,
          typename = EnableIfMismatch<Pointer<Type>, InterPointer<InterType>>>
      auto operator=(BaseReference<InterPointer, InterType> &&other) noexcept
          -> BaseReference & {
        pointer = std::move(other.pointer);
        return *this;
      }
      template <
          template <typename...> class InterPointer, typename InterType,
          typename = EnableIfMismatch<Pointer<Type>, InterPointer<InterType>>>
      void swap(BaseReference<InterPointer, InterType> &other) {
        using std::swap;
        swap(pointer, other.pointer);
      }
      auto operator*() const noexcept(noexcept(*std::declval<Pointer<Type>>()))
          -> typename std::add_lvalue_reference<Type>::type {
        return *CopyReference<Pointer<Type>>{}(pointer);
      }
      auto operator->() const noexcept -> RawPointer {
        return CopyReference<Pointer<Type>>{}(pointer).get();
      }
      Pointer<Type> pointer;
    };
  } // namespace internal
  template <typename Type>
  using Reference = internal::BaseReference<std::shared_ptr, Type>;
  template <typename Type>
  using StrongReference = internal::BaseReference<std::unique_ptr, Type>;
  template <typename Type>
  using WeakReference = internal::BaseReference<std::weak_ptr, Type>;
} // namespace chimera::library::object
