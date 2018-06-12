// Copyright (c) 2017 Adam Grandquist <grandquista@gmail.com>
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

//! helper to evaluate a std container in reverse
//! used as
//! for (auto &&elem : reverse(vector)) {}

#pragma once

#include <type_traits>

namespace chimera {
  namespace library {
    namespace container {
      template <typename Container>
      struct Reverse {
        auto begin() const noexcept { return container->rbegin(); }

        auto cbegin() const noexcept { return container->crbegin(); }

        auto rbegin() const noexcept { return container->begin(); }

        auto crbegin() const noexcept { return container->cbegin(); }

        auto end() const noexcept { return container->rend(); }

        auto cend() const noexcept { return container->crend(); }

        auto rend() const noexcept { return container->end(); }

        auto crend() const noexcept { return container->cend(); }

        std::add_pointer_t<Container> container;
      };

      template <typename Container>
      auto reverse(Container &&container) {
        return Reverse<Container>{&container};
      }
    } // namespace container
  }   // namespace library
} // namespace chimera
