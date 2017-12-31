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

//! used to initialize the sys module.

#include "virtual_machine/modules/sys.hpp"

#include <algorithm>
#include <vector>

#include "options.hpp"

using namespace std::literals;

namespace chimera {
  namespace library {
    namespace virtual_machine {
      namespace modules {
        void sys(const Options &options, const object::Object &module) {
          auto sys = module;
          std::vector<object::Object> argv;
          argv.resize(static_cast<std::size_t>(
              std::distance(options.argv.begin(), options.argv.end())));
          std::transform(options.argv.begin(), options.argv.end(), argv.begin(),
                         [module](const auto &arg) {
                           return object::Object(
                               object::String(arg),
                               {{"__class__", module.get_attribute("str")}});
                         });
          sys.set_attribute("__displayhook__"s, object::Object());
          sys.set_attribute("__doc__"s, object::Object());
          sys.set_attribute("__excepthook__"s, object::Object());
          sys.set_attribute("__interactivehook__"s, object::Object());
          sys.set_attribute("__loader__"s, object::Object());
          sys.set_attribute("__name__"s, object::Object());
          sys.set_attribute("__package__"s, object::Object());
          sys.set_attribute("__spec__"s, object::Object());
          sys.set_attribute("__stderr__"s, object::Object());
          sys.set_attribute("__stdin__"s, object::Object());
          sys.set_attribute("__stdout__"s, object::Object());
          sys.set_attribute("_clear_type_cache"s, object::Object());
          sys.set_attribute("_current_frames"s, object::Object());
          sys.set_attribute("_debugmallocstats"s, object::Object());
          sys.set_attribute("_getframe"s, object::Object());
          sys.set_attribute("_git"s, object::Object());
          sys.set_attribute("_home"s, object::Object());
          sys.set_attribute("_xoptions"s, object::Object());
          sys.set_attribute("abiflags"s, object::Object());
          sys.set_attribute("api_version"s, object::Object());
          sys.set_attribute(
              "argv"s, object::Object(
                           argv, {{"__class__", sys.get_attribute("tuple")}}));
          sys.set_attribute("base_exec_prefix"s, object::Object());
          sys.set_attribute("base_prefix"s, object::Object());
          sys.set_attribute("builtin_module_names"s, object::Object());
          sys.set_attribute("byteorder"s, object::Object());
          sys.set_attribute("call_tracing"s, object::Object());
          sys.set_attribute("callstats"s, object::Object());
          sys.set_attribute("copyright"s, object::Object());
          sys.set_attribute("displayhook"s, object::Object());
          sys.set_attribute("dont_write_bytecode"s, object::Object());
          sys.set_attribute("exc_info"s, object::Object());
          sys.set_attribute("excepthook"s, object::Object());
          sys.set_attribute("exec_prefix"s, object::Object());
          sys.set_attribute("executable"s, object::Object());
          sys.set_attribute("exit"s, object::Object());
          sys.set_attribute("flags"s, object::Object());
          sys.set_attribute("float_info"s, object::Object());
          sys.set_attribute("float_repr_style"s, object::Object());
          sys.set_attribute("get_asyncgen_hooks"s, object::Object());
          sys.set_attribute("get_coroutine_wrapper"s, object::Object());
          sys.set_attribute("getallocatedblocks"s, object::Object());
          sys.set_attribute("getcheckinterval"s, object::Object());
          sys.set_attribute("getdefaultencoding"s, object::Object());
          sys.set_attribute("getdlopenflags"s, object::Object());
          sys.set_attribute("getfilesystemencodeerrors"s, object::Object());
          sys.set_attribute("getfilesystemencoding"s, object::Object());
          sys.set_attribute("getprofile"s, object::Object());
          sys.set_attribute("getrecursionlimit"s, object::Object());
          sys.set_attribute("getrefcount"s, object::Object());
          sys.set_attribute("getsizeof"s, object::Object());
          sys.set_attribute("getswitchinterval"s, object::Object());
          sys.set_attribute("gettrace"s, object::Object());
          sys.set_attribute("hash_info"s, object::Object());
          sys.set_attribute("hexversion"s, object::Object());
          sys.set_attribute("implementation"s, object::Object());
          sys.set_attribute("int_info"s, object::Object());
          sys.set_attribute("intern"s, object::Object());
          sys.set_attribute("is_finalizing"s, object::Object());
          sys.set_attribute("maxsize"s, object::Object());
          sys.set_attribute("maxunicode"s, object::Object());
          sys.set_attribute("meta_path"s, object::Object());
          sys.set_attribute("modules"s, object::Object());
          sys.set_attribute("path"s, object::Object());
          sys.set_attribute("path_hooks"s, object::Object());
          sys.set_attribute("path_importer_cache"s, object::Object());
          sys.set_attribute("platform"s, object::Object());
          sys.set_attribute("prefix"s, object::Object());
          sys.set_attribute("ps1"s, object::Object());
          sys.set_attribute("ps2"s, object::Object());
          sys.set_attribute("set_asyncgen_hooks"s, object::Object());
          sys.set_attribute("set_coroutine_wrapper"s, object::Object());
          sys.set_attribute("setcheckinterval"s, object::Object());
          sys.set_attribute("setdlopenflags"s, object::Object());
          sys.set_attribute("setprofile"s, object::Object());
          sys.set_attribute("setrecursionlimit"s, object::Object());
          sys.set_attribute("setswitchinterval"s, object::Object());
          sys.set_attribute("settrace"s, object::Object());
          sys.set_attribute("stderr"s, object::Object());
          sys.set_attribute("stdin"s, object::Object());
          sys.set_attribute("stdout"s, object::Object());
          sys.set_attribute("thread_info"s, object::Object());
          sys.set_attribute("version"s, object::Object());
          sys.set_attribute("version_info"s, object::Object());
          sys.set_attribute("warnoptions"s, object::Object());
        }
      } // namespace modules
    }   // namespace virtual_machine
  }     // namespace library
} // namespace chimera
