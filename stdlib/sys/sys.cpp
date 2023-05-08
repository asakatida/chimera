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

#include "sys/sys.hpp"

#include "options.hpp"

#include <gsl/gsl>

#include <algorithm>
#include <vector>

using namespace std::literals;

namespace chimera::library::virtual_machine::modules {
  using Argv = std::vector<object::Object>;
  void sys(const object::Object &module) {
    auto sys = module;
    sys.set_attribute("__displayhook__"s, module);
    sys.set_attribute("__doc__"s, module);
    sys.set_attribute("__excepthook__"s, module);
    sys.set_attribute("__interactivehook__"s, module);
    sys.set_attribute("__loader__"s, module);
    sys.set_attribute("__name__"s, module);
    sys.set_attribute("__package__"s, module);
    sys.set_attribute("__spec__"s, module);
    sys.set_attribute("__stderr__"s, module);
    sys.set_attribute("__stdin__"s, module);
    sys.set_attribute("__stdout__"s, module);
    sys.set_attribute("_clear_type_cache"s, module);
    sys.set_attribute("_current_frames"s, module);
    sys.set_attribute("_debugmallocstats"s, module);
    sys.set_attribute("_getframe"s, module);
    sys.set_attribute("_git"s, module);
    sys.set_attribute("_home"s, module);
    sys.set_attribute("_xoptions"s, module);
    sys.set_attribute("abiflags"s, module);
    sys.set_attribute("api_version"s, module);
    sys.set_attribute("argv"s, module);
    sys.set_attribute("base_exec_prefix"s, module);
    sys.set_attribute("base_prefix"s, module);
    sys.set_attribute("builtin_module_names"s, module);
    sys.set_attribute("byteorder"s, module);
    sys.set_attribute("call_tracing"s, module);
    sys.set_attribute("callstats"s, module);
    sys.set_attribute("copyright"s, module);
    sys.set_attribute("displayhook"s, module);
    sys.set_attribute("dont_write_bytecode"s, module);
    sys.set_attribute("exc_info"s, module);
    sys.set_attribute("excepthook"s, module);
    sys.set_attribute("exec_prefix"s, module);
    sys.set_attribute("executable"s, module);
    sys.set_attribute("exit"s, module);
    sys.set_attribute("flags"s, module);
    sys.set_attribute("float_info"s, module);
    sys.set_attribute("float_repr_style"s, module);
    sys.set_attribute("get_asyncgen_hooks"s, module);
    sys.set_attribute("get_coroutine_wrapper"s, module);
    sys.set_attribute("getallocatedblocks"s, module);
    sys.set_attribute("getcheckinterval"s, module);
    sys.set_attribute("getdefaultencoding"s, module);
    sys.set_attribute("getdlopenflags"s, module);
    sys.set_attribute("getfilesystemencodeerrors"s, module);
    sys.set_attribute("getfilesystemencoding"s, module);
    sys.set_attribute("getprofile"s, module);
    sys.set_attribute("getrecursionlimit"s, module);
    sys.set_attribute("getrefcount"s, module);
    sys.set_attribute("getsizeof"s, module);
    sys.set_attribute("getswitchinterval"s, module);
    sys.set_attribute("gettrace"s, module);
    sys.set_attribute("hash_info"s, module);
    sys.set_attribute("hexversion"s, module);
    sys.set_attribute("implementation"s, module);
    sys.set_attribute("int_info"s, module);
    sys.set_attribute("intern"s, module);
    sys.set_attribute("is_finalizing"s, module);
    sys.set_attribute("maxsize"s, module);
    sys.set_attribute("maxunicode"s, module);
    sys.set_attribute("meta_path"s, module);
    sys.set_attribute("modules"s, module);
    sys.set_attribute("path"s, module);
    sys.set_attribute("path_hooks"s, module);
    sys.set_attribute("path_importer_cache"s, module);
    sys.set_attribute("platform"s, module);
    sys.set_attribute("prefix"s, module);
    sys.set_attribute("ps1"s, module);
    sys.set_attribute("ps2"s, module);
    sys.set_attribute("set_asyncgen_hooks"s, module);
    sys.set_attribute("set_coroutine_wrapper"s, module);
    sys.set_attribute("setcheckinterval"s, module);
    sys.set_attribute("setdlopenflags"s, module);
    sys.set_attribute("setprofile"s, module);
    sys.set_attribute("setrecursionlimit"s, module);
    sys.set_attribute("setswitchinterval"s, module);
    sys.set_attribute("settrace"s, module);
    sys.set_attribute("stderr"s, module);
    sys.set_attribute("stdin"s, module);
    sys.set_attribute("stdout"s, module);
    sys.set_attribute("thread_info"s, module);
    sys.set_attribute("version"s, module);
    sys.set_attribute("version_info"s, module);
    sys.set_attribute("warnoptions"s, module);
  }
} // namespace chimera::library::virtual_machine::modules
