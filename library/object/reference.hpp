//! smart pointer abstraction for object reference counting

#pragma once

#include <memory>      // for shared_ptr, weak_ptr
#include <type_traits> // for add_lvalue_reference, add_pointer_t, enable_if_t
#include <utility>     // for declval, forward
#include <variant>     // for swap

namespace chimera::library::object {
  namespace internal {
    template <template <typename...> class Pointer, typename Type>
    struct CopyReference {
      [[nodiscard]] auto operator()(const Pointer<Type> &pointer) const noexcept
          -> std::shared_ptr<Type> {
        return pointer;
      }
    };
    template <typename Type>
    struct CopyReference<std::weak_ptr, Type> {
      [[nodiscard]] auto
      operator()(const std::weak_ptr<Type> &pointer) const noexcept
          -> std::shared_ptr<Type> {
        return pointer.lock();
      }
    };
    template <typename L, typename R>
    using EnableIfMismatch = std::enable_if_t<!std::is_same_v<L, R>>;
    template <template <typename...> class Pointer, typename Type>
    struct BaseReference {
      using RawPointer = std::add_pointer_t<Type>;
      BaseReference() : pointer(std::make_shared<Type>()) {}
      template <typename... Args>
      explicit BaseReference(Args &&...args)
          : pointer(std::make_shared<Type>(std::forward<Args>(args)...)) {}
      template <
          template <typename...> class InterPointer, typename InterType,
          typename = EnableIfMismatch<Pointer<Type>, InterPointer<InterType>>>
      explicit BaseReference(
          const BaseReference<InterPointer, InterType> &other)
          : pointer(other.pointer) {}
      template <
          template <typename...> class InterPointer, typename InterType,
          typename = EnableIfMismatch<Pointer<Type>, InterPointer<InterType>>>
      explicit BaseReference(
          BaseReference<InterPointer, InterType> &&other) noexcept
          : pointer(std::move(other.pointer)) {}
      template <template <typename...> class InterPointer, typename InterType>
      auto operator=(const BaseReference<InterPointer, InterType> &other)
          -> BaseReference & {
        pointer = CopyReference<InterPointer, InterType>{}(other.pointer);
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
      [[nodiscard]] auto operator*() const
          noexcept(noexcept(*std::declval<Pointer<Type>>())) ->
          typename std::add_lvalue_reference<Type>::type {
        return *pointer;
      }
      [[nodiscard]] auto operator->() const noexcept -> RawPointer {
        return CopyReference<Pointer, Type>{}(pointer).get();
      }

    private:
      friend BaseReference<std::shared_ptr, Type>;
      friend BaseReference<std::weak_ptr, Type>;
      Pointer<Type> pointer;
    };
  } // namespace internal
  template <typename Type>
  using Reference = internal::BaseReference<std::shared_ptr, Type>;
  template <typename Type>
  using WeakReference = internal::BaseReference<std::weak_ptr, Type>;
} // namespace chimera::library::object
