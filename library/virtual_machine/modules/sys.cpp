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

//! used to initialize the sys module.

#include "virtual_machine/modules/sys.hpp"

#include <algorithm>
#include <vector>

#include "options.hpp"

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
                         [module](auto &&arg) {
                           return object::Object(
                               object::String(arg),
                               {{"__class__", module.get_attribute("str")}});
                         });
          sys.set_attribute("__displayhook__", object::Object{});
          sys.set_attribute("__doc__", object::Object{});
          sys.set_attribute("__excepthook__", object::Object{});
          sys.set_attribute("__interactivehook__", object::Object{});
          sys.set_attribute("__loader__", object::Object{});
          sys.set_attribute("__name__", object::Object{});
          sys.set_attribute("__package__", object::Object{});
          sys.set_attribute("__spec__", object::Object{});
          sys.set_attribute("__stderr__", object::Object{});
          sys.set_attribute("__stdin__", object::Object{});
          sys.set_attribute("__stdout__", object::Object{});
          sys.set_attribute("_clear_type_cache", object::Object{});
          sys.set_attribute("_current_frames", object::Object{});
          sys.set_attribute("_debugmallocstats", object::Object{});
          sys.set_attribute("_getframe", object::Object{});
          sys.set_attribute("_git", object::Object{});
          sys.set_attribute("_home", object::Object{});
          sys.set_attribute("_xoptions", object::Object{});
          sys.set_attribute("abiflags", object::Object{});
          sys.set_attribute("api_version", object::Object{});
          sys.set_attribute(
              "argv", object::Object(
                          argv, {{"__class__", sys.get_attribute("tuple")}}));
          sys.set_attribute("base_exec_prefix", object::Object{});
          sys.set_attribute("base_prefix", object::Object{});
          sys.set_attribute("builtin_module_names", object::Object{});
          sys.set_attribute("byteorder", object::Object{});
          sys.set_attribute("call_tracing", object::Object{});
          sys.set_attribute("callstats", object::Object{});
          sys.set_attribute("copyright", object::Object{});
          sys.set_attribute("displayhook", object::Object{});
          sys.set_attribute("dont_write_bytecode", object::Object{});
          sys.set_attribute("exc_info", object::Object{});
          sys.set_attribute("excepthook", object::Object{});
          sys.set_attribute("exec_prefix", object::Object{});
          sys.set_attribute("executable", object::Object{});
          sys.set_attribute("exit", object::Object{});
          sys.set_attribute("flags", object::Object{});
          sys.set_attribute("float_info", object::Object{});
          sys.set_attribute("float_repr_style", object::Object{});
          sys.set_attribute("get_asyncgen_hooks", object::Object{});
          sys.set_attribute("get_coroutine_wrapper", object::Object{});
          sys.set_attribute("getallocatedblocks", object::Object{});
          sys.set_attribute("getcheckinterval", object::Object{});
          sys.set_attribute("getdefaultencoding", object::Object{});
          sys.set_attribute("getdlopenflags", object::Object{});
          sys.set_attribute("getfilesystemencodeerrors", object::Object{});
          sys.set_attribute("getfilesystemencoding", object::Object{});
          sys.set_attribute("getprofile", object::Object{});
          sys.set_attribute("getrecursionlimit", object::Object{});
          sys.set_attribute("getrefcount", object::Object{});
          sys.set_attribute("getsizeof", object::Object{});
          sys.set_attribute("getswitchinterval", object::Object{});
          sys.set_attribute("gettrace", object::Object{});
          sys.set_attribute("hash_info", object::Object{});
          sys.set_attribute("hexversion", object::Object{});
          sys.set_attribute("implementation", object::Object{});
          sys.set_attribute("int_info", object::Object{});
          sys.set_attribute("intern", object::Object{});
          sys.set_attribute("is_finalizing", object::Object{});
          sys.set_attribute("maxsize", object::Object{});
          sys.set_attribute("maxunicode", object::Object{});
          sys.set_attribute("meta_path", object::Object{});
          sys.set_attribute("modules", object::Object{});
          sys.set_attribute("path", object::Object{});
          sys.set_attribute("path_hooks", object::Object{});
          sys.set_attribute("path_importer_cache", object::Object{});
          sys.set_attribute("platform", object::Object{});
          sys.set_attribute("prefix", object::Object{});
          sys.set_attribute("ps1", object::Object{});
          sys.set_attribute("ps2", object::Object{});
          sys.set_attribute("set_asyncgen_hooks", object::Object{});
          sys.set_attribute("set_coroutine_wrapper", object::Object{});
          sys.set_attribute("setcheckinterval", object::Object{});
          sys.set_attribute("setdlopenflags", object::Object{});
          sys.set_attribute("setprofile", object::Object{});
          sys.set_attribute("setrecursionlimit", object::Object{});
          sys.set_attribute("setswitchinterval", object::Object{});
          sys.set_attribute("settrace", object::Object{});
          sys.set_attribute("stderr", object::Object{});
          sys.set_attribute("stdin", object::Object{});
          sys.set_attribute("stdout", object::Object{});
          sys.set_attribute("thread_info", object::Object{});
          sys.set_attribute("version", object::Object{});
          sys.set_attribute("version_info", object::Object{});
          sys.set_attribute("warnoptions", object::Object{});
        }
      } // namespace modules
    }   // namespace virtual_machine
  }     // namespace library
} // namespace chimera
