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

#include <chimera.h>

#include <cstdlib>

#include <gsl/narrow>

#undef Py_TYPE

// maintain some level of API compatability with upstream
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-macro-usage,cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,cppcoreguidelines-pro-type-reinterpret-cast,hicpp-no-malloc)

static auto Py_TYPE(PyObject *o) -> auto & { return o->ob_type; }

static auto Py_TYPE(PyVarObject *o) -> auto & { return Py_TYPE(&o->ob_base); }

static auto Py_TYPE(PyTypeObject *o) -> auto & { return Py_TYPE(&o->ob_base); }

#undef Py_REFCNT
static auto Py_REFCNT(PyObject *o) -> auto & { return o->ob_refcnt; }

static auto Py_REFCNT(PyVarObject *o) -> auto & {
  return Py_REFCNT(&o->ob_base);
}

// static auto &Py_REFCNT(PyTypeObject *o) { return Py_REFCNT(&o->ob_base); }

#undef Py_SIZE
static auto Py_SIZE(PyVarObject *o) -> auto & { return o->ob_size; }

// static auto &Py_SIZE(PyTypeObject *o) { return Py_SIZE(&o->ob_base); }

#undef PyObject_HEAD_INIT
#ifdef Py_TRACE_REFS
#define PyObject_HEAD_INIT(type) _PyObject_EXTRA_INIT nullptr, nullptr, 1, type,
#else
#define PyObject_HEAD_INIT(type) _PyObject_EXTRA_INIT 1, type,
#endif

#undef PyObject_New
// template <typename Type>
// Type *PyObject_New(PyTypeObject *type) { return nullptr; }

#undef PyObject_NewVar
// template <typename Type>
// Type *PyObject_NewVar(PyTypeObject *type, Py_ssize_t size) {
//   return nullptr;
// }

#undef PyObject_GC_New
#undef PyObject_GC_NewVar
#undef PyObject_GC_Resize
#undef PyMem_New
#undef PyMem_Resize
#undef PyMem_Del

PyObject PyType_Type{PyObject_HEAD_INIT(&PyType_Type)};

extern "C" auto PyType_Check(PyObject *o) -> int {
  return gsl::narrow<int>(Py_TYPE(o) == &PyType_Type);
}

extern "C" auto PyType_CheckExact(PyObject *o) -> int {
  return gsl::narrow<int>(Py_TYPE(o) == &PyType_Type);
}

extern "C" auto PyType_ClearCache() -> unsigned int { return 0; }

extern "C" auto PyType_GetFlags(PyTypeObject *type) -> long {
  return gsl::narrow<long>(type->tp_flags);
}

extern "C" void PyType_Modified(PyTypeObject * /*type*/) {}

extern "C" auto PyType_HasFeature(PyTypeObject *o, int feature) -> int {
  return gsl::narrow<int>((o->tp_flags & gsl::narrow<size_t>(feature)) != 0);
}

extern "C" auto PyType_IS_GC(PyTypeObject *o) -> int {
  return PyType_HasFeature(o, Py_TPFLAGS_HAVE_GC);
}

extern "C" auto PyType_IsSubtype(PyTypeObject *a, PyTypeObject *b) -> int {
  for (; a != &PyBaseObject_Type; a = a->tp_base) {
    if (a == b) {
      return 1;
    }
    if (a->tp_base == nullptr) {
      return 0;
    }
  }
  return 0;
}

extern "C" auto PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems)
    -> PyObject * {
  if (type->tp_itemsize != 0) {
    auto *object = reinterpret_cast<PyVarObject *>(std::calloc(
        (gsl::narrow<size_t>(type->tp_basicsize) +
         gsl::narrow<size_t>(nitems) * gsl::narrow<size_t>(type->tp_itemsize)) /
                sizeof(PyVarObject) +
            1,
        sizeof(PyVarObject)));
    if (object == nullptr) {
      return nullptr;
    }
    Py_SIZE(object) = gsl::narrow<size_t>(nitems);
    Py_REFCNT(object) = 1;
    Py_TYPE(object) = reinterpret_cast<PyObject *>(type);
    return reinterpret_cast<PyObject *>(object);
  }
  auto *object = reinterpret_cast<PyObject *>(
      std::calloc(1, gsl::narrow<size_t>(type->tp_basicsize)));
  if (object == nullptr) {
    return nullptr;
  }
  Py_REFCNT(object) = 1;
  Py_TYPE(object) = reinterpret_cast<PyObject *>(type);
  return object;
}

extern "C" auto PyType_GenericNew(PyTypeObject *type, PyObject * /*args*/,
                                  PyObject * /*kwds*/) -> PyObject * {
  return type->tp_alloc(type, 0);
}

extern "C" auto PyType_Ready(PyTypeObject *type) -> int {
  if (Py_TYPE(type) == nullptr) {
    Py_TYPE(type) = reinterpret_cast<PyObject *>(&PyType_Type);
  }
  return 0;
}

extern "C" auto PyType_FromSpec(PyType_Spec * /*spec*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyType_FromSpecWithBases(PyType_Spec * /*spec*/,
                                         PyObject * /*bases*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyType_GetSlot(PyTypeObject * /*type*/, int /*slot*/)
    -> void * {
  return nullptr;
}

PyTypeObject PyBaseObject_Type{
    {PyVarObject_HEAD_INIT(&PyType_Type, sizeof(PyTypeObject))},
    "object",
    sizeof(PyTypeObject),
    0,
    reinterpret_cast<void (*)(PyObject *)>(PyObject_GC_Del),
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    PyObject_Repr,
    nullptr,
    nullptr,
    nullptr,
    PyObject_Hash,
    PyObject_Call,
    nullptr,
    PyObject_GenericGetAttr,
    PyObject_GenericSetAttr,
    nullptr,
    0,
    "object doc",
    nullptr,
    nullptr,
    nullptr,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    &PyBaseObject_Type,
    nullptr,
    nullptr,
    nullptr,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    0,
    0,
    0,
    nullptr};

extern "C" auto _PyObject_New(PyTypeObject * /*type*/) -> PyObject * {
  return nullptr;
}

extern "C" auto _PyObject_NewVar(PyTypeObject * /*type*/, Py_ssize_t /*size*/)
    -> PyVarObject * {
  return nullptr;
}

extern "C" auto PyObject_Init(PyObject *op, PyTypeObject *type) -> PyObject * {
  Py_TYPE(op) = reinterpret_cast<PyObject *>(type);
  return op;
}

extern "C" auto PyObject_InitVar(PyVarObject *op, PyTypeObject *type,
                                 Py_ssize_t size) -> PyVarObject * {
  op = reinterpret_cast<PyVarObject *>(
      PyObject_Init(reinterpret_cast<PyObject *>(op), type));
  Py_SIZE(op) = gsl::narrow<size_t>(size);
  return op;
}

extern "C" void PyObject_Del(PyObject *op) { std::free(op); }

extern "C" auto PyArg_ParseTuple(PyObject * /*args*/, const char * /*format*/,
                                 ...) -> int {
  return 0;
}

extern "C" auto PyArg_VaParse(PyObject * /*args*/, const char * /*format*/,
                              va_list /*vargs*/) -> int {
  return 0;
}

extern "C" auto PyArg_ParseTupleAndKeywords(PyObject * /*args*/,
                                            PyObject * /*kw*/,
                                            const char * /*format*/,
                                            char * /*keywords*/[], ...) -> int {
  return 0;
}

extern "C" auto PyArg_VaParseTupleAndKeywords(PyObject * /*args*/,
                                              PyObject * /*kw*/,
                                              const char * /*format*/,
                                              char * /*keywords*/[],
                                              va_list /*vargs*/) -> int {
  return 0;
}

extern "C" auto PyArg_ValidateKeywordArguments(PyObject * /*unused*/) -> int {
  return 0;
}

extern "C" auto PyArg_Parse(PyObject * /*args*/, const char * /*format*/, ...)
    -> int {
  return 0;
}

extern "C" auto PyArg_UnpackTuple(PyObject * /*args*/, const char * /*name*/,
                                  Py_ssize_t /*min*/, Py_ssize_t /*max*/, ...)
    -> int {
  return 0;
}

extern "C" auto Py_BuildValue(const char * /*format*/, ...) -> PyObject * {
  return nullptr;
}

extern "C" auto Py_VaBuildValue(const char * /*format*/, va_list /*vargs*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyBool_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyBool_FromLong(long /*v*/) -> PyObject * { return nullptr; }

extern "C" auto PyObject_CheckBuffer(PyObject * /*obj*/) -> int { return 0; }

extern "C" auto PyObject_GetBuffer(PyObject * /*exporter*/,
                                   Py_buffer * /*view*/, int /*flags*/) -> int {
  return 0;
}

extern "C" void PyBuffer_Release(Py_buffer * /*view*/) {}

extern "C" auto PyBuffer_SizeFromFormat(const char * /*unused*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyBuffer_IsContiguous(Py_buffer * /*view*/, char /*order*/)
    -> int {
  return 0;
}

extern "C" void PyBuffer_FillContiguousStrides(int /*ndim*/,
                                               Py_ssize_t * /*shape*/,
                                               Py_ssize_t * /*strides*/,
                                               Py_ssize_t /*itemsize*/,
                                               char /*order*/) {}

extern "C" auto PyBuffer_FillInfo(Py_buffer * /*view*/, PyObject * /*exporter*/,
                                  void * /*buf*/, Py_ssize_t /*len*/,
                                  int /*readonly*/, int /*flags*/) -> int {
  return 0;
}

extern "C" auto PyByteArray_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyByteArray_CheckExact(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyByteArray_FromObject(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyByteArray_FromStringAndSize(const char * /*string*/,
                                              Py_ssize_t /*len*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyByteArray_Concat(PyObject * /*a*/, PyObject * /*b*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyByteArray_Size(PyObject * /*bytearray*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyByteArray_AsString(PyObject * /*bytearray*/) -> char * {
  return nullptr;
}

extern "C" auto PyByteArray_Resize(PyObject * /*bytearray*/, Py_ssize_t /*len*/)
    -> int {
  return 0;
}

extern "C" auto PyByteArray_AS_STRING(PyObject * /*bytearray*/) -> char * {
  return nullptr;
}

extern "C" auto PyByteArray_GET_SIZE(PyObject * /*bytearray*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyBytes_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyBytes_CheckExact(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyBytes_FromString(const char * /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyBytes_FromStringAndSize(const char * /*v*/,
                                          Py_ssize_t /*len*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyBytes_FromFormat(const char * /*format*/, ...) -> PyObject * {
  return nullptr;
}

extern "C" auto PyBytes_FromFormatV(const char * /*format*/, va_list /*vargs*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyBytes_FromObject(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyBytes_Size(PyObject * /*o*/) -> Py_ssize_t { return 0; }

extern "C" auto PyBytes_GET_SIZE(PyObject * /*o*/) -> Py_ssize_t { return 0; }

extern "C" auto PyBytes_AsString(PyObject * /*o*/) -> char * { return nullptr; }

extern "C" auto PyBytes_AS_STRING(PyObject * /*string*/) -> char * {
  return nullptr;
}

extern "C" auto PyBytes_AsStringAndSize(PyObject * /*obj*/, char ** /*buffer*/,
                                        Py_ssize_t * /*length*/) -> int {
  return 0;
}

extern "C" void PyBytes_Concat(PyObject ** /*bytes*/, PyObject * /*newpart*/) {}

extern "C" void PyBytes_ConcatAndDel(PyObject ** /*bytes*/,
                                     PyObject * /*newpart*/) {}

extern "C" auto _PyBytes_Resize(PyObject ** /*bytes*/, Py_ssize_t /*newsize*/)
    -> int {
  return 0;
}

extern "C" auto PyCapsule_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyCapsule_New(void * /*pointer*/, const char * /*name*/,
                              PyCapsule_Destructor /*destructor*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyCapsule_GetPointer(PyObject * /*capsule*/,
                                     const char * /*name*/) -> void * {
  return nullptr;
}

extern "C" auto PyCapsule_GetDestructor(PyObject * /*capsule*/)
    -> PyCapsule_Destructor {
  return nullptr;
}

extern "C" auto PyCapsule_GetContext(PyObject * /*capsule*/) -> void * {
  return nullptr;
}

extern "C" auto PyCapsule_GetName(PyObject * /*capsule*/) -> const char * {
  return nullptr;
}

extern "C" auto PyCapsule_Import(const char * /*name*/, int /*no_block*/)
    -> void * {
  return nullptr;
}

extern "C" auto PyCapsule_IsValid(PyObject * /*capsule*/, const char * /*name*/)
    -> int {
  return 0;
}

extern "C" auto PyCapsule_SetContext(PyObject * /*capsule*/, void * /*context*/)
    -> int {
  return 0;
}

extern "C" auto PyCapsule_SetDestructor(PyObject * /*capsule*/,
                                        PyCapsule_Destructor /*destructor*/)
    -> int {
  return 0;
}

extern "C" auto PyCapsule_SetName(PyObject * /*capsule*/, const char * /*name*/)
    -> int {
  return 0;
}

extern "C" auto PyCapsule_SetPointer(PyObject * /*capsule*/, void * /*pointer*/)
    -> int {
  return 0;
}

extern "C" auto PyCell_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyCell_New(PyObject * /*ob*/) -> PyObject * { return nullptr; }

extern "C" auto PyCell_Get(PyObject * /*cell*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCell_GET(PyObject * /*cell*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCell_Set(PyObject * /*cell*/, PyObject * /*value*/) -> int {
  return 0;
}

extern "C" void PyCell_SET(PyObject * /*cell*/, PyObject * /*value*/) {}

extern "C" auto PyCode_Check(PyObject * /*co*/) -> int { return 0; }

extern "C" auto PyCode_GetNumFree(PyCodeObject * /*co*/) -> int { return 0; }

extern "C" auto

PyCode_New(int /*argcount*/, int /*kwonlyargcount*/, int /*nlocals*/,
           int /*stacksize*/, int /*flags*/, PyObject * /*code*/,
           PyObject * /*consts*/, PyObject * /*names*/, PyObject * /*varnames*/,
           PyObject * /*freevars*/, PyObject * /*cellvars*/,
           PyObject * /*filename*/, PyObject * /*name*/, int /*firstlineno*/,
           PyObject * /*lnotab*/) -> PyCodeObject * {
  return nullptr;
}

extern "C" auto PyCode_NewEmpty(const char * /*filename*/,
                                const char * /*funcname*/, int /*firstlineno*/)
    -> PyCodeObject * {
  return nullptr;
}

extern "C" auto PyCodec_Register(PyObject * /*search_function*/) -> int {
  return 0;
}

extern "C" auto PyCodec_KnownEncoding(const char * /*encoding*/) -> int {
  return 0;
}

extern "C" auto PyCodec_Encode(PyObject * /*object*/, const char * /*encoding*/,
                               const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_Decode(PyObject * /*object*/, const char * /*encoding*/,
                               const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_Encoder(const char * /*encoding*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_Decoder(const char * /*encoding*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_IncrementalEncoder(const char * /*encoding*/,
                                           const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_IncrementalDecoder(const char * /*encoding*/,
                                           const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_StreamReader(const char * /*encoding*/,
                                     PyObject * /*stream*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_StreamWriter(const char * /*encoding*/,
                                     PyObject * /*stream*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_RegisterError(const char * /*name*/,
                                      PyObject * /*error*/) -> int {
  return 0;
}

extern "C" auto PyCodec_LookupError(const char * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_StrictErrors(PyObject * /*exc*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_IgnoreErrors(PyObject * /*exc*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_ReplaceErrors(PyObject * /*exc*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_XMLCharRefReplaceErrors(PyObject * /*exc*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_BackslashReplaceErrors(PyObject * /*exc*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyCodec_NameReplaceErrors(PyObject * /*exc*/) -> PyObject * {
  return nullptr;
}

extern "C" auto _Py_c_sum(Py_complex /*left*/, Py_complex /*right*/)
    -> Py_complex {
  return {};
}

extern "C" auto _Py_c_diff(Py_complex /*left*/, Py_complex /*right*/)
    -> Py_complex {
  return {};
}

extern "C" auto _Py_c_neg(Py_complex /*complex*/) -> Py_complex { return {}; }

extern "C" auto _Py_c_prod(Py_complex /*left*/, Py_complex /*right*/)
    -> Py_complex {
  return {};
}

extern "C" auto _Py_c_quot(Py_complex /*dividend*/, Py_complex /*divisor*/)
    -> Py_complex {
  return {};
}

extern "C" auto _Py_c_pow(Py_complex /*num*/, Py_complex /*exp*/)
    -> Py_complex {
  return {};
}

extern "C" auto PyComplex_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyComplex_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyComplex_FromCComplex(Py_complex /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyComplex_FromDoubles(double /*real*/, double /*imag*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyComplex_RealAsDouble(PyObject * /*op*/) -> double {
  return 0.0;
}

extern "C" auto PyComplex_ImagAsDouble(PyObject * /*op*/) -> double {
  return 0.0;
}

extern "C" auto PyComplex_AsCComplex(PyObject * /*op*/) -> Py_complex {
  return {};
}

extern "C" auto PyOS_snprintf(char * /*str*/, size_t /*size*/,
                              const char * /*format*/, ...) -> int {
  return 0;
}

extern "C" auto PyOS_vsnprintf(char * /*str*/, size_t /*size*/,
                               const char * /*format*/, va_list /*va*/) -> int {
  return 0;
}

extern "C" auto PyOS_string_to_double(const char * /*s*/, char ** /*endptr*/,
                                      PyObject * /*overflow_exception*/)
    -> double {
  return 0.0;
}

extern "C" auto PyOS_double_to_string(double /*val*/, char /*format_code*/,
                                      int /*precision*/, int /*flags*/,
                                      int * /*ptype*/) -> char * {
  return nullptr;
}

extern "C" auto PyOS_stricmp(const char * /*s1*/, const char * /*s2*/) -> int {
  return 0;
}

extern "C" auto PyOS_strnicmp(const char * /*s1*/, const char * /*s2*/,
                              Py_ssize_t /*size*/) -> int {
  return 0;
}

extern "C" auto PyCoro_CheckExact(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyCoro_New(PyFrameObject * /*frame*/, PyObject * /*name*/,
                           PyObject * /*qualname*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDate_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyDate_CheckExact(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyDateTime_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyDateTime_CheckExact(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyTime_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyTime_CheckExact(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyDelta_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyDelta_CheckExact(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyTZInfo_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyTZInfo_CheckExact(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyDate_FromDate(int /*year*/, int /*month*/, int /*day*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyDateTime_FromDateAndTime(int /*year*/, int /*month*/,
                                           int /*day*/, int /*hour*/,
                                           int /*minute*/, int /*second*/,
                                           int /*usecond*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyTime_FromTime(int /*hour*/, int /*minute*/, int /*second*/,
                                int /*usecond*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDelta_FromDSU(int /*days*/, int /*seconds*/, int /*useconds*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyDateTime_GET_YEAR(PyDateTime_Date * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_GET_MONTH(PyDateTime_Date * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_GET_DAY(PyDateTime_Date * /*o*/) -> int { return 0; }

extern "C" auto PyDateTime_DATE_GET_HOUR(PyDateTime_DateTime * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_DATE_GET_MINUTE(PyDateTime_DateTime * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_DATE_GET_SECOND(PyDateTime_DateTime * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_DATE_GET_MICROSECOND(PyDateTime_DateTime * /*o*/)
    -> int {
  return 0;
}

extern "C" auto PyDateTime_TIME_GET_HOUR(PyDateTime_Time * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_TIME_GET_MINUTE(PyDateTime_Time * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_TIME_GET_SECOND(PyDateTime_Time * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_TIME_GET_MICROSECOND(PyDateTime_Time * /*o*/)
    -> int {
  return 0;
}

extern "C" auto PyDateTime_DELTA_GET_DAYS(PyDateTime_Delta * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_DELTA_GET_SECONDS(PyDateTime_Delta * /*o*/) -> int {
  return 0;
}

extern "C" auto PyDateTime_DELTA_GET_MICROSECOND(PyDateTime_Delta * /*o*/)
    -> int {
  return 0;
}

extern "C" auto PyDateTime_FromTimestamp(PyObject * /*args*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDate_FromTimestamp(PyObject * /*args*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDescr_NewGetSet(PyTypeObject * /*type*/,
                                  struct PyGetSetDef * /*getset*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyDescr_NewMember(PyTypeObject * /*type*/,
                                  struct PyMemberDef * /*meth*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDescr_NewMethod(PyTypeObject * /*type*/,
                                  struct PyMethodDef * /*meth*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDescr_NewWrapper(PyTypeObject * /*type*/,
                                   struct wrapperbase * /*wrapper*/,
                                   void * /*wrapped*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDescr_NewClassMethod(PyTypeObject * /*type*/,
                                       PyMethodDef * /*method*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDescr_IsData(PyObject * /*descr*/) -> int { return 0; }

extern "C" auto PyWrapper_New(PyObject * /*unused*/, PyObject * /*unused*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyDict_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyDict_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyDict_New() -> PyObject * { return nullptr; }

extern "C" auto PyDictProxy_New(PyObject * /*mapping*/) -> PyObject * {
  return nullptr;
}

extern "C" void PyDict_Clear(PyObject * /*p*/) {}

extern "C" auto PyDict_Contains(PyObject * /*p*/, PyObject * /*key*/) -> int {
  return 0;
}

extern "C" auto PyDict_Copy(PyObject * /*p*/) -> PyObject * { return nullptr; }

extern "C" auto PyDict_SetItem(PyObject * /*p*/, PyObject * /*key*/,
                               PyObject * /*val*/) -> int {
  return 0;
}

extern "C" auto PyDict_SetItemString(PyObject * /*p*/, const char * /*key*/,
                                     PyObject * /*val*/) -> int {
  return 0;
}

extern "C" auto PyDict_DelItem(PyObject * /*p*/, PyObject * /*key*/) -> int {
  return 0;
}

extern "C" auto PyDict_DelItemString(PyObject * /*p*/, const char * /*key*/)
    -> int {
  return 0;
}

extern "C" auto PyDict_GetItem(PyObject * /*p*/, PyObject * /*key*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyDict_GetItemWithError(PyObject * /*p*/, PyObject * /*key*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyDict_GetItemString(PyObject * /*p*/, const char * /*key*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyDict_SetDefault(PyObject * /*p*/, PyObject * /*key*/,
                                  PyObject * /*d*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDict_Items(PyObject * /*p*/) -> PyObject * { return nullptr; }

extern "C" auto PyDict_Keys(PyObject * /*p*/) -> PyObject * { return nullptr; }

extern "C" auto PyDict_Values(PyObject * /*p*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyDict_Size(PyObject * /*p*/) -> Py_ssize_t { return 0; }

extern "C" auto PyDict_Next(PyObject * /*p*/, Py_ssize_t * /*ppos*/,
                            PyObject ** /*pkey*/, PyObject ** /*pvalue*/)
    -> int {
  return 0;
}

extern "C" auto PyDict_Merge(PyObject * /*a*/, PyObject * /*b*/,
                             int /*override*/) -> int {
  return 0;
}

extern "C" auto PyDict_Update(PyObject * /*a*/, PyObject * /*b*/) -> int {
  return 0;
}

extern "C" auto PyDict_MergeFromSeq2(PyObject * /*a*/, PyObject * /*seq2*/,
                                     int /*override*/) -> int {
  return 0;
}

extern "C" auto PyDict_ClearFreeList() -> int { return 0; }

extern "C" void PyErr_Clear() {}

extern "C" void PyErr_PrintEx(int /*set_sys_last_vars*/) {}

extern "C" void PyErr_Print() {}

extern "C" void PyErr_WriteUnraisable(PyObject * /*obj*/) {}

extern "C" void PyErr_SetString(PyObject * /*type*/, const char * /*message*/) {
}

extern "C" void PyErr_SetObject(PyObject * /*type*/, PyObject * /*value*/) {}

extern "C" auto PyErr_Format(PyObject * /*exception*/, const char * /*format*/,
                             ...) -> PyObject * {
  return nullptr;
}

extern "C" auto PyErr_FormatV(PyObject * /*exception*/, const char * /*format*/,
                              va_list /*vargs*/) -> PyObject * {
  return nullptr;
}

extern "C" void PyErr_SetNone(PyObject * /*type*/) {}

extern "C" auto PyErr_BadArgument() -> int { return 0; }

extern "C" auto PyErr_NoMemory() -> PyObject * { return nullptr; }

extern "C" auto PyErr_SetFromErrno(PyObject * /*type*/) -> PyObject * {
  return nullptr;
}

extern "C" auto

PyErr_SetFromErrnoWithFilenameObject(PyObject * /*type*/,
                                     PyObject * /*filenameObject*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto

PyErr_SetFromErrnoWithFilenameObjects(PyObject * /*type*/,
                                      PyObject * /*filenameObject*/,
                                      PyObject * /*filenameObject2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyErr_SetFromErrnoWithFilename(PyObject * /*type*/,
                                               const char * /*filename*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyErr_SetFromWindowsErr(int /*ierr*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyErr_SetExcFromWindowsErr(PyObject * /*type*/, int /*ierr*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto

PyErr_SetFromWindowsErrWithFilename(int /*ierr*/, const char * /*filename*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto

PyErr_SetExcFromWindowsErrWithFilenameObject(PyObject * /*type*/, int /*ierr*/,
                                             PyObject * /*filename*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto

PyErr_SetExcFromWindowsErrWithFilenameObjects(PyObject * /*type*/, int /*ierr*/,
                                              PyObject * /*filename*/,
                                              PyObject * /*filename2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto

PyErr_SetExcFromWindowsErrWithFilename(PyObject * /*type*/, int /*ierr*/,
                                       const char * /*filename*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyErr_SetImportError(PyObject * /*msg*/, PyObject * /*name*/,
                                     PyObject * /*path*/) -> PyObject * {
  return nullptr;
}

extern "C" void PyErr_SyntaxLocationObject(PyObject * /*filename*/,
                                           int /*lineno*/, int /*col_offset*/) {
}

extern "C" void PyErr_SyntaxLocationEx(const char * /*filename*/,
                                       int /*lineno*/, int /*col_offset*/) {}

extern "C" void PyErr_SyntaxLocation(const char * /*filename*/,
                                     int /*lineno*/) {}

extern "C" void PyErr_BadInternalCall() {}

extern "C" auto PyErr_WarnEx(PyObject * /*category*/, const char * /*message*/,
                             Py_ssize_t /*stack_level*/) -> int {
  return 0;
}

extern "C" auto PyErr_SetImportErrorSubclass(PyObject * /*msg*/,
                                             PyObject * /*name*/,
                                             PyObject * /*path*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyErr_WarnExplicitObject(PyObject * /*category*/,
                                         PyObject * /*message*/,
                                         PyObject * /*filename*/,
                                         int /*lineno*/, PyObject * /*module*/,
                                         PyObject * /*registry*/) -> int {
  return 0;
}

extern "C" auto PyErr_WarnExplicit(PyObject * /*category*/,
                                   const char * /*message*/,
                                   const char * /*filename*/, int /*lineno*/,
                                   const char * /*module*/,
                                   PyObject * /*registry*/) -> int {
  return 0;
}

extern "C" auto PyErr_WarnFormat(PyObject * /*category*/,
                                 Py_ssize_t /*stack_level*/,
                                 const char * /*format*/, ...) -> int {
  return 0;
}

extern "C" auto PyErr_ResourceWarning(PyObject * /*source*/,
                                      Py_ssize_t /*stack_level*/,
                                      const char * /*format*/, ...) -> int {
  return 0;
}

extern "C" auto PyErr_Occurred() -> PyObject * { return nullptr; }

extern "C" auto PyErr_ExceptionMatches(PyObject * /*exc*/) -> int { return 0; }

extern "C" auto PyErr_GivenExceptionMatches(PyObject * /*given*/,
                                            PyObject * /*exc*/) -> int {
  return 0;
}

extern "C" void PyErr_Fetch(PyObject ** /*ptype*/, PyObject ** /*pvalue*/,
                            PyObject ** /*ptraceback*/) {}

extern "C" void PyErr_Restore(PyObject * /*type*/, PyObject * /*value*/,
                              PyObject * /*traceback*/) {}

extern "C" void PyErr_NormalizeException(PyObject ** /*exc*/,
                                         PyObject ** /*val*/,
                                         PyObject ** /*tb*/) {}

extern "C" void PyErr_GetExcInfo(PyObject ** /*ptype*/, PyObject ** /*pvalue*/,
                                 PyObject ** /*ptraceback*/) {}

extern "C" void PyErr_SetExcInfo(PyObject * /*type*/, PyObject * /*value*/,
                                 PyObject * /*traceback*/) {}

extern "C" auto PyErr_CheckSignals() -> int { return 0; }

extern "C" void PyErr_SetInterrupt() {}

extern "C" auto PySignal_SetWakeupFd(int /*fd*/) -> int { return 0; }

extern "C" auto PyErr_NewException(const char * /*name*/, PyObject * /*base*/,
                                   PyObject * /*dict*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyErr_NewExceptionWithDoc(const char * /*name*/,
                                          const char * /*doc*/,
                                          PyObject * /*base*/,
                                          PyObject * /*dict*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyException_GetTraceback(PyObject * /*ex*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyException_SetTraceback(PyObject * /*ex*/, PyObject * /*tb*/)
    -> int {
  return 0;
}

extern "C" auto PyException_GetContext(PyObject * /*ex*/) -> PyObject * {
  return nullptr;
}

extern "C" void PyException_SetContext(PyObject * /*ex*/, PyObject * /*ctx*/) {}

extern "C" auto PyException_GetCause(PyObject * /*ex*/) -> PyObject * {
  return nullptr;
}

extern "C" void PyException_SetCause(PyObject * /*ex*/, PyObject * /*cause*/) {}

extern "C" auto

PyUnicodeDecodeError_Create(const char * /*encoding*/, const char * /*object*/,
                            Py_ssize_t /*length*/, Py_ssize_t /*start*/,
                            Py_ssize_t /*end*/, const char * /*reason*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicodeEncodeError_Create(
    const char * /*encoding*/, const Py_UNICODE * /*object*/,
    Py_ssize_t /*length*/, Py_ssize_t /*start*/, Py_ssize_t /*end*/,
    const char * /*reason*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicodeTranslateError_Create(
    const Py_UNICODE * /*object*/, Py_ssize_t /*length*/, Py_ssize_t /*start*/,
    Py_ssize_t /*end*/, const char * /*reason*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicodeDecodeError_GetEncoding(PyObject * /*exc*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicodeDecodeError_GetObject(PyObject * /*exc*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicodeDecodeError_GetStart(PyObject * /*exc*/,
                                              Py_ssize_t * /*start*/) -> int {
  return 0;
}

extern "C" auto PyUnicodeDecodeError_SetStart(PyObject * /*exc*/,
                                              Py_ssize_t /*start*/) -> int {
  return 0;
}

extern "C" auto PyUnicodeDecodeError_GetEnd(PyObject * /*exc*/,
                                            Py_ssize_t * /*end*/) -> int {
  return 0;
}

extern "C" auto PyUnicodeDecodeError_SetEnd(PyObject * /*exc*/,
                                            Py_ssize_t /*end*/) -> int {
  return 0;
}

extern "C" auto PyUnicodeDecodeError_GetReason(PyObject * /*exc*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicodeDecodeError_SetReason(PyObject * /*exc*/,
                                               const char * /*reason*/) -> int {
  return 0;
}

extern "C" auto Py_EnterRecursiveCall(const char * /*where*/) -> int {
  return 0;
}

extern "C" void Py_LeaveRecursiveCall() {}

extern "C" auto Py_ReprEnter(PyObject * /*object*/) -> int { return 0; }

extern "C" void Py_ReprLeave(PyObject * /*object*/) {}

extern "C" auto PyFile_FromFd(int /*fd*/, const char * /*name*/,
                              const char * /*mode*/, int /*buffering*/,
                              const char * /*encoding*/,
                              const char * /*errors*/, const char * /*newline*/,
                              int /*closefd*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_AsFileDescriptor(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyFile_GetLine(PyObject * /*p*/, int /*n*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFile_WriteObject(PyObject * /*obj*/, PyObject * /*p*/,
                                   int /*flags*/) -> int {
  return 0;
}

extern "C" auto PyFile_WriteString(const char * /*s*/, PyObject * /*p*/)
    -> int {
  return 0;
}

extern "C" auto PyFloat_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyFloat_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyFloat_FromString(PyObject * /*str*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFloat_FromDouble(double /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFloat_AsDouble(PyObject * /*pyfloat*/) -> double {
  return 0.0;
}

extern "C" auto PyFloat_AS_DOUBLE(PyObject * /*pyfloat*/) -> double {
  return 0.0;
}

extern "C" auto PyFloat_GetInfo() -> PyObject * { return nullptr; }

extern "C" auto PyFloat_GetMax() -> double { return 0.0; }

extern "C" auto PyFloat_GetMin() -> double { return 0.0; }

extern "C" auto PyFloat_ClearFreeList() -> int { return 0; }

extern "C" auto PyFunction_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyFunction_New(PyObject * /*code*/, PyObject * /*globals*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_NewWithQualName(PyObject * /*code*/,
                                           PyObject * /*globals*/,
                                           PyObject * /*qualname*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_GetCode(PyObject * /*op*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_GetGlobals(PyObject * /*op*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_GetModule(PyObject * /*op*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_GetDefaults(PyObject * /*op*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_SetDefaults(PyObject * /*op*/,
                                       PyObject * /*defaults*/) -> int {
  return 0;
}

extern "C" auto PyFunction_GetClosure(PyObject * /*op*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_SetClosure(PyObject * /*op*/, PyObject * /*closure*/)
    -> int {
  return 0;
}

extern "C" auto PyFunction_GetAnnotations(PyObject * /*op*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFunction_SetAnnotations(PyObject * /*op*/,
                                          PyObject * /*annotations*/) -> int {
  return 0;
}

extern "C" void PyObject_GC_Track(PyObject * /*op*/) {}

extern "C" void _PyObject_GC_TRACK(PyObject * /*op*/) {}

extern "C" void PyObject_GC_Del(void * /*op*/) {}

extern "C" void PyObject_GC_UnTrack(void * /*op*/) {}

extern "C" void _PyObject_GC_UNTRACK(PyObject * /*op*/) {}

extern "C" void Py_VISIT(PyObject * /*o*/) {}

extern "C" auto PyGen_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyGen_CheckExact(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PyGen_New(PyFrameObject * /*frame*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyGen_NewWithQualName(PyFrameObject * /*frame*/,
                                      PyObject * /*name*/,
                                      PyObject * /*qualname*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ImportModule(const char * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ImportModuleNoBlock(const char * /*name*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ImportModuleEx(const char * /*name*/,
                                        PyObject * /*globals*/,
                                        PyObject * /*locals*/,
                                        PyObject * /*fromlist*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ImportModuleLevelObject(PyObject * /*name*/,
                                                 PyObject * /*globals*/,
                                                 PyObject * /*locals*/,
                                                 PyObject * /*fromlist*/,
                                                 int /*level*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ImportModuleLevel(const char * /*name*/,
                                           PyObject * /*globals*/,
                                           PyObject * /*locals*/,
                                           PyObject * /*fromlist*/,
                                           int /*level*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_Import(PyObject * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ReloadModule(PyObject * /*m*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_AddModuleObject(PyObject * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_AddModule(const char * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ExecCodeModule(const char * /*name*/,
                                        PyObject * /*co*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_ExecCodeModuleEx(const char * /*name*/,
                                          PyObject * /*co*/,
                                          const char * /*pathname*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto
PyImport_ExecCodeModuleObject(PyObject * /*name*/, PyObject * /*co*/,
                              PyObject * /*pathname*/, PyObject * /*cpathname*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto

PyImport_ExecCodeModuleWithPathnames(const char * /*name*/, PyObject * /*co*/,
                                     const char * /*pathname*/,
                                     const char * /*cpathname*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyImport_GetMagicNumber() -> long { return 0; }

extern "C" auto PyImport_GetMagicTag() -> const char * { return nullptr; }

extern "C" auto PyImport_GetModuleDict() -> PyObject * { return nullptr; }

extern "C" auto PyImport_GetImporter(PyObject * /*path*/) -> PyObject * {
  return nullptr;
}

extern "C" void _PyImport_Init() {}

extern "C" void PyImport_Cleanup() {}

extern "C" void _PyImport_Fini() {}

extern "C" auto PyImport_ImportFrozenModuleObject(PyObject * /*name*/) -> int {
  return 0;
}

extern "C" auto PyImport_ImportFrozenModule(const char * /*name*/) -> int {
  return 0;
}

extern "C" auto PyImport_AppendInittab(const char * /*name*/,
                                       PyObject *(* /*initfunc*/)()) -> int {
  return 0;
}

extern "C" auto PyImport_ExtendInittab(struct _inittab * /*newtab*/) -> int {
  return 0;
}

extern "C" void Py_Initialize() {}

extern "C" void Py_InitializeEx(int /*initsigs*/) {}

extern "C" auto Py_IsInitialized() -> int { return 0; }

extern "C" auto Py_FinalizeEx() -> int { return 0; }

extern "C" void Py_Finalize() {}

extern "C" auto Py_SetStandardStreamEncoding(const char * /*encoding*/,
                                             const char * /*errors*/) -> int {
  return 0;
}

extern "C" void Py_SetProgramName(wchar_t * /*name*/) {}

extern "C" auto Py_GetProgramName() -> wchar_t * { return nullptr; }

extern "C" auto Py_GetPrefix() -> wchar_t * { return nullptr; }

extern "C" auto Py_GetExecPrefix() -> wchar_t * { return nullptr; }

extern "C" auto Py_GetProgramFullPath() -> wchar_t * { return nullptr; }

extern "C" auto Py_GetPath() -> wchar_t * { return nullptr; }

extern "C" void Py_SetPath(const wchar_t * /*unused*/) {}

extern "C" auto Py_GetVersion() -> const char * { return nullptr; }

extern "C" auto Py_GetPlatform() -> const char * { return nullptr; }

extern "C" auto Py_GetCopyright() -> const char * { return nullptr; }

extern "C" auto Py_GetCompiler() -> const char * { return nullptr; }

extern "C" auto Py_GetBuildInfo() -> const char * { return nullptr; }

extern "C" void PySys_SetArgvEx(int /*argc*/, wchar_t ** /*argv*/,
                                int /*updatepath*/) {}

extern "C" void PySys_SetArgv(int /*argc*/, wchar_t ** /*argv*/) {}

extern "C" void Py_SetPythonHome(wchar_t * /*home*/) {}

extern "C" auto Py_GetPythonHome() -> wchar_t * { return nullptr; }

extern "C" void PyEval_InitThreads() {}

extern "C" auto PyEval_ThreadsInitialized() -> int { return 0; }

extern "C" auto PyEval_SaveThread() -> PyThreadState * { return nullptr; }

extern "C" void PyEval_RestoreThread(PyThreadState * /*tstate*/) {}

extern "C" auto PyThreadState_Get() -> PyThreadState * { return nullptr; }

extern "C" auto PyThreadState_Swap(PyThreadState * /*tstate*/)
    -> PyThreadState * {
  return nullptr;
}

extern "C" void PyEval_ReInitThreads() {}

extern "C" auto PyGILState_Ensure() -> PyGILState_STATE { return nullptr; }

extern "C" void PyGILState_Release(PyGILState_STATE /*unused*/) {}

extern "C" auto PyGILState_GetThisThreadState() -> PyThreadState * {
  return nullptr;
}

extern "C" auto PyGILState_Check() -> int { return 0; }

extern "C" auto PyInterpreterState_New() -> PyInterpreterState * {
  return nullptr;
}

extern "C" void PyInterpreterState_Clear(PyInterpreterState * /*interp*/) {}

extern "C" void PyInterpreterState_Delete(PyInterpreterState * /*interp*/) {}

extern "C" auto PyThreadState_New(PyInterpreterState * /*interp*/)
    -> PyThreadState * {
  return nullptr;
}

extern "C" void PyThreadState_Clear(PyThreadState * /*tstate*/) {}

extern "C" void PyThreadState_Delete(PyThreadState * /*tstate*/) {}

extern "C" auto PyInterpreterState_GetID(PyInterpreterState * /*interp*/)
    -> PY_INT64_T {
  return 0;
}

extern "C" auto PyThreadState_GetDict() -> PyObject * { return nullptr; }

extern "C" auto PyThreadState_SetAsyncExc(unsigned long /*id*/,
                                          PyObject * /*exc*/) -> int {
  return 0;
}

extern "C" void PyEval_AcquireThread(PyThreadState * /*tstate*/) {}

extern "C" void PyEval_ReleaseThread(PyThreadState * /*tstate*/) {}

extern "C" void PyEval_AcquireLock() {}

extern "C" void PyEval_ReleaseLock() {}

extern "C" auto Py_NewInterpreter() -> PyThreadState * { return nullptr; }

extern "C" void Py_EndInterpreter(PyThreadState * /*tstate*/) {}

extern "C" auto Py_AddPendingCall(int (* /*func*/)(void *), void * /*arg*/)
    -> int {
  return 0;
}

extern "C" void PyEval_SetProfile(Py_tracefunc /*func*/, PyObject * /*obj*/) {}

extern "C" void PyEval_SetTrace(Py_tracefunc /*func*/, PyObject * /*obj*/) {}

extern "C" auto PyInterpreterState_Head() -> PyInterpreterState * {
  return nullptr;
}

extern "C" auto

PyInterpreterState_Next(PyInterpreterState * /*interp*/)
    -> PyInterpreterState * {
  return nullptr;
}

extern "C" auto

PyInterpreterState_ThreadHead(PyInterpreterState * /*interp*/)
    -> PyThreadState * {
  return nullptr;
}

extern "C" auto PyThreadState_Next(PyThreadState * /*tstate*/)
    -> PyThreadState * {
  return nullptr;
}

extern "C" auto PyIter_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyIter_Next(PyObject * /*o*/) -> PyObject * { return nullptr; }

extern "C" auto PySeqIter_Check(PyObject * /*op*/) -> int { return 0; }

extern "C" auto PySeqIter_New(PyObject * /*seq*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyCallIter_Check(PyObject * /*op*/) -> int { return 0; }

extern "C" auto PyCallIter_New(PyObject * /*callable*/, PyObject * /*sentinel*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyList_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyList_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyList_New(Py_ssize_t /*len*/) -> PyObject * { return nullptr; }

extern "C" auto PyList_Size(PyObject * /*list*/) -> Py_ssize_t { return 0; }

extern "C" auto PyList_GET_SIZE(PyObject * /*list*/) -> Py_ssize_t { return 0; }

extern "C" auto PyList_GetItem(PyObject * /*list*/, Py_ssize_t /*index*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyList_GET_ITEM(PyObject * /*list*/, Py_ssize_t /*i*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyList_SetItem(PyObject * /*list*/, Py_ssize_t /*index*/,
                               PyObject * /*item*/) -> int {
  return 0;
}

extern "C" void PyList_SET_ITEM(PyObject * /*list*/, Py_ssize_t /*i*/,
                                PyObject * /*o*/) {}

extern "C" auto PyList_Insert(PyObject * /*list*/, Py_ssize_t /*index*/,
                              PyObject * /*item*/) -> int {
  return 0;
}

extern "C" auto PyList_Append(PyObject * /*list*/, PyObject * /*item*/) -> int {
  return 0;
}

extern "C" auto PyList_GetSlice(PyObject * /*list*/, Py_ssize_t /*low*/,
                                Py_ssize_t /*high*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyList_SetSlice(PyObject * /*list*/, Py_ssize_t /*low*/,
                                Py_ssize_t /*high*/, PyObject * /*itemlist*/)
    -> int {
  return 0;
}

extern "C" auto PyList_Sort(PyObject * /*list*/) -> int { return 0; }

extern "C" auto PyList_Reverse(PyObject * /*list*/) -> int { return 0; }

extern "C" auto PyList_AsTuple(PyObject * /*list*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyList_ClearFreeList() -> int { return 0; }

extern "C" auto PyLong_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyLong_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyLong_FromLong(long /*v*/) -> PyObject * { return nullptr; }

extern "C" auto PyLong_FromUnsignedLong(unsigned long /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromSsize_t(Py_ssize_t /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromSize_t(size_t /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromLongLong(long long /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromUnsignedLongLong(unsigned long long /*v*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromDouble(double /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromString(const char * /*str*/, char ** /*pend*/,
                                  int /*base*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromUnicode(Py_UNICODE * /*u*/, Py_ssize_t /*length*/,
                                   int /*base*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromUnicodeObject(PyObject * /*u*/, int /*base*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_FromVoidPtr(void * /*p*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyLong_AsLong(PyObject * /*obj*/) -> long { return 0; }

extern "C" auto PyLong_AsLongAndOverflow(PyObject * /*obj*/, int * /*overflow*/)
    -> long {
  return 0;
}

extern "C" auto PyLong_AsLongLong(PyObject * /*obj*/) -> long long { return 0; }

extern "C" auto PyLong_AsLongLongAndOverflow(PyObject * /*obj*/,
                                             int * /*overflow*/) -> long long {
  return 0;
}

extern "C" auto PyLong_AsSsize_t(PyObject * /*pylong*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyLong_AsUnsignedLong(PyObject * /*pylong*/) -> unsigned long {
  return 0;
}

extern "C" auto PyLong_AsSize_t(PyObject * /*pylong*/) -> size_t { return 0; }

extern "C" auto PyLong_AsUnsignedLongLong(PyObject * /*pylong*/)
    -> unsigned long long {
  return 0;
}

extern "C" auto PyLong_AsUnsignedLongMask(PyObject * /*obj*/) -> unsigned long {
  return 0;
}

extern "C" auto PyLong_AsUnsignedLongLongMask(PyObject * /*obj*/)
    -> unsigned long long {
  return 0;
}

extern "C" auto PyLong_AsDouble(PyObject * /*pylong*/) -> double { return 0.0; }

extern "C" auto PyLong_AsVoidPtr(PyObject * /*pylong*/) -> void * {
  return nullptr;
}

extern "C" auto PyMapping_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyMapping_Size(PyObject * /*o*/) -> Py_ssize_t { return 0; }

extern "C" auto PyMapping_DelItemString(PyObject * /*o*/, const char * /*key*/)
    -> int {
  return 0;
}

extern "C" auto PyMapping_DelItem(PyObject * /*o*/, PyObject * /*key*/) -> int {
  return 0;
}

extern "C" auto PyMapping_HasKeyString(PyObject * /*o*/, const char * /*key*/)
    -> int {
  return 0;
}

extern "C" auto PyMapping_HasKey(PyObject * /*o*/, PyObject * /*key*/) -> int {
  return 0;
}

extern "C" auto PyMapping_Keys(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMapping_Values(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMapping_Items(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMapping_GetItemString(PyObject * /*o*/, const char * /*key*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyMapping_SetItemString(PyObject * /*o*/, const char * /*key*/,
                                        PyObject * /*v*/) -> int {
  return 0;
}

extern "C" void PyMarshal_WriteLongToFile(long /*value*/, FILE * /*file*/,
                                          int /*version*/) {}

extern "C" void PyMarshal_WriteObjectToFile(PyObject * /*value*/,
                                            FILE * /*file*/, int /*version*/) {}

extern "C" auto PyMarshal_WriteObjectToString(PyObject * /*value*/,
                                              int /*version*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMarshal_ReadLongFromFile(FILE * /*file*/) -> long {
  return 0;
}

extern "C" auto PyMarshal_ReadShortFromFile(FILE * /*file*/) -> int {
  return 0;
}

extern "C" auto PyMarshal_ReadObjectFromFile(FILE * /*file*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMarshal_ReadLastObjectFromFile(FILE * /*file*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyMarshal_ReadObjectFromString(const char * /*value*/,
                                               Py_ssize_t /*len*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyMem_RawMalloc(size_t /*n*/) -> void * { return nullptr; }

extern "C" auto PyMem_RawCalloc(size_t /*nelem*/, size_t /*elsize*/) -> void * {
  return nullptr;
}

extern "C" auto PyMem_RawRealloc(void * /*p*/, size_t /*n*/) -> void * {
  return nullptr;
}

extern "C" void PyMem_RawFree(void * /*p*/) {}

extern "C" auto PyMem_Malloc(size_t /*n*/) -> void * { return nullptr; }

extern "C" auto PyMem_Calloc(size_t /*nelem*/, size_t /*elsize*/) -> void * {
  return nullptr;
}

extern "C" auto PyMem_Realloc(void * /*p*/, size_t /*n*/) -> void * {
  return nullptr;
}

extern "C" void PyMem_Free(void * /*p*/) {}

extern "C" void PyMem_GetAllocator(PyMemAllocatorDomain /*domain*/,
                                   PyMemAllocatorEx * /*allocator*/) {}

extern "C" void PyMem_SetAllocator(PyMemAllocatorDomain /*domain*/,
                                   PyMemAllocatorEx * /*allocator*/) {}

extern "C" void PyMem_SetupDebugHooks() {}

extern "C" auto

PyObject_GetArenaAllocator(PyObjectArenaAllocator * /*allocator*/)
    -> PyObjectArenaAllocator * {
  return nullptr;
}

extern "C" auto

PyObject_SetArenaAllocator(PyObjectArenaAllocator * /*allocator*/)
    -> PyObjectArenaAllocator * {
  return nullptr;
}

extern "C" auto PyMemoryView_FromObject(PyObject * /*obj*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMemoryView_FromMemory(char * /*mem*/, Py_ssize_t /*size*/,
                                        int /*flags*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMemoryView_FromBuffer(Py_buffer * /*view*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMemoryView_GetContiguous(PyObject * /*obj*/,
                                           int /*buffertype*/, char /*order*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyMemoryView_Check(PyObject * /*obj*/) -> int { return 0; }

extern "C" auto PyMemoryView_GET_BUFFER(PyObject * /*mview*/) -> Py_buffer * {
  return nullptr;
}

extern "C" auto PyMemoryView_GET_BASE(PyObject * /*mview*/) -> Py_buffer * {
  return nullptr;
}

extern "C" auto PyInstanceMethod_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyInstanceMethod_New(PyObject * /*func*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyInstanceMethod_Function(PyObject * /*im*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyInstanceMethod_GET_FUNCTION(PyObject * /*im*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMethod_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyMethod_New(PyObject * /*func*/, PyObject * /*self*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyMethod_Function(PyObject * /*meth*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMethod_GET_FUNCTION(PyObject * /*meth*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMethod_Self(PyObject * /*meth*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMethod_GET_SELF(PyObject * /*meth*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyMethod_ClearFreeList() -> int { return 0; }

extern "C" auto PyModule_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyModule_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyModule_NewObject(PyObject * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_New(const char * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_GetDict(PyObject * /*module*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_GetNameObject(PyObject * /*module*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_GetName(PyObject * /*module*/) -> const char * {
  return nullptr;
}

extern "C" auto PyModule_GetState(PyObject * /*module*/) -> void * {
  return nullptr;
}

extern "C" auto PyModule_GetDef(PyObject * /*module*/) -> PyModuleDef * {
  return nullptr;
}

extern "C" auto PyModule_GetFilenameObject(PyObject * /*module*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_GetFilename(PyObject * /*module*/) -> const char * {
  return nullptr;
}

extern "C" auto PyModule_Create(PyModuleDef * /*def*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_Create2(PyModuleDef * /*def*/,
                                 int /*module_api_version*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModuleDef_Init(PyModuleDef * /*def*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_FromDefAndSpec(PyModuleDef * /*def*/,
                                        PyObject * /*spec*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_FromDefAndSpec2(PyModuleDef * /*def*/,
                                         PyObject * /*spec*/,
                                         int /*module_api_version*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyModule_ExecDef(PyObject * /*module*/, PyModuleDef * /*def*/)
    -> int {
  return 0;
}

extern "C" auto PyModule_SetDocString(PyObject * /*module*/,
                                      const char * /*docstring*/) -> int {
  return 0;
}

extern "C" auto PyModule_AddFunctions(PyObject * /*module*/,
                                      PyMethodDef * /*functions*/) -> int {
  return 0;
}

extern "C" auto PyModule_AddObject(PyObject * /*module*/, const char * /*name*/,
                                   PyObject * /*value*/) -> int {
  return 0;
}

extern "C" auto PyModule_AddIntConstant(PyObject * /*module*/,
                                        const char * /*name*/, long /*value*/)
    -> int {
  return 0;
}

extern "C" auto PyModule_AddStringConstant(PyObject * /*module*/,
                                           const char * /*name*/,
                                           const char * /*value*/) -> int {
  return 0;
}

extern "C" auto PyState_FindModule(PyModuleDef * /*def*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyState_AddModule(PyObject * /*module*/, PyModuleDef * /*def*/)
    -> int {
  return 0;
}

extern "C" auto PyState_RemoveModule(PyModuleDef * /*def*/) -> int { return 0; }

extern "C" auto PyNumber_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyNumber_Add(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Subtract(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Multiply(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_MatrixMultiply(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_FloorDivide(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_TrueDivide(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Remainder(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Divmod(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Power(PyObject * /*o1*/, PyObject * /*o2*/,
                               PyObject * /*o3*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Negative(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Positive(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Absolute(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Invert(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Lshift(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Rshift(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_And(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Xor(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Or(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceAdd(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceSubtract(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceMultiply(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceMatrixMultiply(PyObject * /*o1*/,
                                               PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceFloorDivide(PyObject * /*o1*/,
                                            PyObject * /*o2*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceTrueDivide(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceRemainder(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlacePower(PyObject * /*o1*/, PyObject * /*o2*/,
                                      PyObject * /*o3*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceLshift(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceRshift(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceAnd(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceXor(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_InPlaceOr(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Long(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Float(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_Index(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_ToBase(PyObject * /*n*/, int /*base*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyNumber_AsSsize_t(PyObject * /*o*/, PyObject * /*exc*/)
    -> Py_ssize_t {
  return 0;
}

extern "C" auto PyIndex_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyObject_AsCharBuffer(PyObject * /*obj*/,
                                      const char ** /*buffer*/,
                                      Py_ssize_t * /*buffer_len*/) -> int {
  return 0;
}

extern "C" auto PyObject_AsReadBuffer(PyObject * /*obj*/,
                                      const void ** /*buffer*/,
                                      Py_ssize_t * /*buffer_len*/) -> int {
  return 0;
}

extern "C" auto PyObject_CheckReadBuffer(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyObject_AsWriteBuffer(PyObject * /*obj*/, void ** /*buffer*/,
                                       Py_ssize_t * /*buffer_len*/) -> int {
  return 0;
}

extern "C" auto PyObject_Print(PyObject * /*o*/, FILE * /*fp*/, int /*flags*/)
    -> int {
  return 0;
}

extern "C" auto PyObject_HasAttr(PyObject * /*o*/, PyObject * /*attr_name*/)
    -> int {
  return 0;
}

extern "C" auto PyObject_HasAttrString(PyObject * /*o*/,
                                       const char * /*attr_name*/) -> int {
  return 0;
}

extern "C" auto PyObject_GetAttr(PyObject * /*o*/, PyObject * /*attr_name*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_GetAttrString(PyObject * /*o*/,
                                       const char * /*attr_name*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_GenericGetAttr(PyObject * /*o*/, PyObject * /*name*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_SetAttr(PyObject * /*o*/, PyObject * /*attr_name*/,
                                 PyObject * /*v*/) -> int {
  return 0;
}

extern "C" auto PyObject_SetAttrString(PyObject * /*o*/,
                                       const char * /*attr_name*/,
                                       PyObject * /*v*/) -> int {
  return 0;
}

extern "C" auto PyObject_GenericSetAttr(PyObject * /*o*/, PyObject * /*name*/,
                                        PyObject * /*value*/) -> int {
  return 0;
}

extern "C" auto PyObject_DelAttr(PyObject * /*o*/, PyObject * /*attr_name*/)
    -> int {
  return 0;
}

extern "C" auto PyObject_DelAttrString(PyObject * /*o*/,
                                       const char * /*attr_name*/) -> int {
  return 0;
}

extern "C" auto PyObject_GenericGetDict(PyObject * /*o*/, void * /*context*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_GenericSetDict(PyObject * /*o*/, void * /*context*/)
    -> int {
  return 0;
}

extern "C" auto PyObject_RichCompare(PyObject * /*o1*/, PyObject * /*o2*/,
                                     int /*opid*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_RichCompareBool(PyObject * /*o1*/, PyObject * /*o2*/,
                                         int /*opid*/) -> int {
  return 0;
}

extern "C" auto PyObject_Repr(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_ASCII(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_Str(PyObject * /*o*/) -> PyObject * { return nullptr; }

extern "C" auto PyObject_Bytes(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_IsSubclass(PyObject * /*derived*/, PyObject * /*cls*/)
    -> int {
  return 0;
}

extern "C" auto PyObject_IsInstance(PyObject * /*inst*/, PyObject * /*cls*/)
    -> int {
  return 0;
}

extern "C" auto PyCallable_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyObject_Call(PyObject * /*callable*/, PyObject * /*args*/,
                              PyObject * /*kwargs*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_CallObject(PyObject * /*callable*/,
                                    PyObject * /*args*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_CallFunction(PyObject * /*callable*/,
                                      const char * /*format*/, ...)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_CallMethod(PyObject * /*obj*/, const char * /*name*/,
                                    const char * /*format*/, ...)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_CallFunctionObjArgs(PyObject * /*callable*/, ...)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_CallMethodObjArgs(PyObject * /*obj*/,
                                           PyObject * /*name*/, ...)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_Hash(PyObject * /*o*/) -> Py_hash_t { return 0; }

extern "C" auto PyObject_HashNotImplemented(PyObject * /*o*/) -> Py_hash_t {
  return 0;
}

extern "C" auto PyObject_IsTrue(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyObject_Not(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyObject_Type(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_TypeCheck(PyObject * /*o*/, PyTypeObject * /*type*/)
    -> int {
  return 0;
}

extern "C" auto PyObject_Length(PyObject * /*o*/) -> Py_ssize_t { return 0; }

extern "C" auto PyObject_LengthHint(PyObject * /*o*/, Py_ssize_t /*d*/)
    -> Py_ssize_t {
  return 0;
}

extern "C" auto PyObject_GetItem(PyObject * /*o*/, PyObject * /*key*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyObject_SetItem(PyObject * /*o*/, PyObject * /*key*/,
                                 PyObject * /*v*/) -> int {
  return 0;
}

extern "C" auto PyObject_DelItem(PyObject * /*o*/, PyObject * /*key*/) -> int {
  return 0;
}

extern "C" auto PyObject_Dir(PyObject * /*o*/) -> PyObject * { return nullptr; }

extern "C" auto PyObject_GetIter(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" void Py_INCREF(PyObject * /*o*/) {}

extern "C" void Py_XINCREF(PyObject * /*o*/) {}

extern "C" void Py_DECREF(PyObject * /*o*/) {}

extern "C" void Py_XDECREF(PyObject * /*o*/) {}

extern "C" void Py_CLEAR(PyObject * /*o*/) {}

extern "C" auto PyEval_GetBuiltins() -> PyObject * { return nullptr; }

extern "C" auto PyEval_GetLocals() -> PyObject * { return nullptr; }

extern "C" auto PyEval_GetGlobals() -> PyObject * { return nullptr; }

extern "C" auto PyEval_GetFrame() -> PyFrameObject * { return nullptr; }

extern "C" auto PyFrame_GetLineNumber(PyFrameObject * /*frame*/) -> int {
  return 0;
}

extern "C" auto PyEval_GetFuncName(PyObject * /*func*/) -> const char * {
  return nullptr;
}

extern "C" auto PyEval_GetFuncDesc(PyObject * /*func*/) -> const char * {
  return nullptr;
}

extern "C" auto PySequence_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PySequence_Size(PyObject * /*o*/) -> Py_ssize_t { return 0; }

extern "C" auto PySequence_Concat(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_Repeat(PyObject * /*o*/, Py_ssize_t /*count*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_InPlaceConcat(PyObject * /*o1*/, PyObject * /*o2*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_InPlaceRepeat(PyObject * /*o*/, Py_ssize_t /*count*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_GetItem(PyObject * /*o*/, Py_ssize_t /*i*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_GetSlice(PyObject * /*o*/, Py_ssize_t /*i1*/,
                                    Py_ssize_t /*i2*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_SetItem(PyObject * /*o*/, Py_ssize_t /*i*/,
                                   PyObject * /*v*/) -> int {
  return 0;
}

extern "C" auto PySequence_DelItem(PyObject * /*o*/, Py_ssize_t /*i*/) -> int {
  return 0;
}

extern "C" auto PySequence_SetSlice(PyObject * /*o*/, Py_ssize_t /*i1*/,
                                    Py_ssize_t /*i2*/, PyObject * /*v*/)
    -> int {
  return 0;
}

extern "C" auto PySequence_DelSlice(PyObject * /*o*/, Py_ssize_t /*i1*/,
                                    Py_ssize_t /*i2*/) -> int {
  return 0;
}

extern "C" auto PySequence_Count(PyObject * /*o*/, PyObject * /*value*/)
    -> Py_ssize_t {
  return 0;
}

extern "C" auto PySequence_Contains(PyObject * /*o*/, PyObject * /*value*/)
    -> int {
  return 0;
}

extern "C" auto PySequence_Index(PyObject * /*o*/, PyObject * /*value*/)
    -> Py_ssize_t {
  return 0;
}

extern "C" auto PySequence_List(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_Tuple(PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_Fast(PyObject * /*o*/, const char * /*m*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_Fast_GET_ITEM(PyObject * /*o*/, Py_ssize_t /*i*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_Fast_ITEMS(PyObject * /*o*/) -> PyObject ** {
  return nullptr;
}

extern "C" auto PySequence_ITEM(PyObject * /*o*/, Py_ssize_t /*i*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PySequence_Fast_GET_SIZE(PyObject * /*o*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PySet_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyFrozenSet_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyAnySet_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyAnySet_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyFrozenSet_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PySet_New(PyObject * /*iterable*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyFrozenSet_New(PyObject * /*iterable*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PySet_Size(PyObject * /*anyset*/) -> Py_ssize_t { return 0; }

extern "C" auto PySet_GET_SIZE(PyObject * /*anyset*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PySet_Contains(PyObject * /*anyset*/, PyObject * /*key*/)
    -> int {
  return 0;
}

extern "C" auto PySet_Add(PyObject * /*set*/, PyObject * /*key*/) -> int {
  return 0;
}

extern "C" auto PySet_Discard(PyObject * /*set*/, PyObject * /*key*/) -> int {
  return 0;
}

extern "C" auto PySet_Pop(PyObject * /*set*/) -> PyObject * { return nullptr; }

extern "C" auto PySet_Clear(PyObject * /*set*/) -> int { return 0; }

extern "C" auto PySet_ClearFreeList() -> int { return 0; }

extern "C" auto PySlice_Check(PyObject * /*ob*/) -> int { return 0; }

extern "C" auto PySlice_New(PyObject * /*start*/, PyObject * /*stop*/,
                            PyObject * /*step*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PySlice_GetIndices(PyObject * /*slice*/, Py_ssize_t /*length*/,
                                   Py_ssize_t * /*start*/,
                                   Py_ssize_t * /*stop*/, Py_ssize_t * /*step*/)
    -> int {
  return 0;
}

extern "C" auto
PySlice_GetIndicesEx(PyObject * /*slice*/, Py_ssize_t /*length*/,
                     Py_ssize_t * /*start*/, Py_ssize_t * /*stop*/,
                     Py_ssize_t * /*step*/, Py_ssize_t * /*slicelength*/)
    -> int {
  return 0;
}

extern "C" auto PySlice_Unpack(PyObject * /*slice*/, Py_ssize_t * /*start*/,
                               Py_ssize_t * /*stop*/, Py_ssize_t * /*step*/)
    -> int {
  return 0;
}

extern "C" auto PySlice_AdjustIndices(Py_ssize_t /*length*/,
                                      Py_ssize_t * /*start*/,
                                      Py_ssize_t * /*stop*/,
                                      Py_ssize_t /*step*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyOS_FSPath(PyObject * /*path*/) -> PyObject * {
  return nullptr;
}

extern "C" auto Py_FdIsInteractive(FILE * /*fp*/, const char * /*filename*/)
    -> int {
  return 0;
}

extern "C" void PyOS_BeforeFork() {}

extern "C" void PyOS_AfterFork_Parent() {}

extern "C" void PyOS_AfterFork_Child() {}

extern "C" void PyOS_AfterFork() {}

extern "C" auto PyOS_CheckStack() -> int { return 0; }

extern "C" auto PyOS_getsig(int /*i*/) -> PyOS_sighandler_t { return nullptr; }

extern "C" auto PyOS_setsig(int /*i*/, PyOS_sighandler_t /*h*/)
    -> PyOS_sighandler_t {
  return nullptr;
}

extern "C" auto Py_DecodeLocale(const char * /*arg*/, size_t * /*size*/)
    -> wchar_t * {
  return nullptr;
}

extern "C" auto Py_EncodeLocale(const wchar_t * /*text*/,
                                size_t * /*error_pos*/) -> char * {
  return nullptr;
}

extern "C" auto PySys_GetObject(const char * /*name*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PySys_SetObject(const char * /*name*/, PyObject * /*v*/)
    -> int {
  return 0;
}

extern "C" void PySys_ResetWarnOptions() {}

extern "C" void PySys_AddWarnOption(const wchar_t * /*s*/) {}

extern "C" void PySys_AddWarnOptionUnicode(PyObject * /*unicode*/) {}

extern "C" void PySys_SetPath(const wchar_t * /*path*/) {}

extern "C" void PySys_WriteStdout(const char * /*format*/, ...) {}

extern "C" void PySys_WriteStderr(const char * /*format*/, ...) {}

extern "C" void PySys_FormatStdout(const char * /*format*/, ...) {}

extern "C" void PySys_FormatStderr(const char * /*format*/, ...) {}

extern "C" void PySys_AddXOption(const wchar_t * /*s*/) {}

extern "C" auto PySys_GetXOptions() -> PyObject * { return nullptr; }

extern "C" void Py_FatalError(const char * /*message*/) {}

extern "C" void Py_Exit(int /*status*/) {}

extern "C" auto Py_AtExit(void (* /*func*/)()) -> int { return 0; }

extern "C" auto PyTuple_Check(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyTuple_CheckExact(PyObject * /*p*/) -> int { return 0; }

extern "C" auto PyTuple_New(Py_ssize_t /*len*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyTuple_Pack(Py_ssize_t /*n*/, ...) -> PyObject * {
  return nullptr;
}

extern "C" auto PyTuple_Size(PyObject * /*p*/) -> Py_ssize_t { return 0; }

extern "C" auto PyTuple_GET_SIZE(PyObject * /*p*/) -> Py_ssize_t { return 0; }

extern "C" auto PyTuple_GetItem(PyObject * /*p*/, Py_ssize_t /*pos*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyTuple_GET_ITEM(PyObject * /*p*/, Py_ssize_t /*pos*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyTuple_GetSlice(PyObject * /*p*/, Py_ssize_t /*low*/,
                                 Py_ssize_t /*high*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyTuple_SetItem(PyObject * /*p*/, Py_ssize_t /*pos*/,
                                PyObject * /*o*/) -> int {
  return 0;
}

extern "C" void PyTuple_SET_ITEM(PyObject * /*p*/, Py_ssize_t /*pos*/,
                                 PyObject * /*o*/) {}

extern "C" auto _PyTuple_Resize(PyObject ** /*p*/, Py_ssize_t /*newsize*/)
    -> int {
  return 0;
}

extern "C" auto PyTuple_ClearFreeList() -> int { return 0; }

extern "C" auto

PyStructSequence_NewType(PyStructSequence_Desc * /*desc*/) -> PyTypeObject * {
  return nullptr;
}

extern "C" void PyStructSequence_InitType(PyTypeObject * /*type*/,
                                          PyStructSequence_Desc * /*desc*/) {}

extern "C" auto PyStructSequence_InitType2(PyTypeObject * /*type*/,
                                           PyStructSequence_Desc * /*desc*/)
    -> int {
  return 0;
}

extern "C" auto PyStructSequence_New(PyTypeObject * /*type*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyStructSequence_GetItem(PyObject * /*p*/, Py_ssize_t /*pos*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyStructSequence_GET_ITEM(PyObject * /*p*/, Py_ssize_t /*pos*/)
    -> PyObject * {
  return nullptr;
}

extern "C" void PyStructSequence_SetItem(PyObject * /*p*/, Py_ssize_t /*pos*/,
                                         PyObject * /*o*/) {}

extern "C" auto PyStructSequence_SET_ITEM(PyObject * /*p*/,
                                          Py_ssize_t * /*pos*/,
                                          PyObject * /*o*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Check(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyUnicode_CheckExact(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyUnicode_READY(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyUnicode_GET_LENGTH(PyObject * /*o*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_1BYTE_DATA(PyObject * /*o*/) -> Py_UCS1 * {
  return nullptr;
}

extern "C" auto PyUnicode_KIND(PyObject * /*o*/) -> int { return 0; }

extern "C" auto PyUnicode_DATA(PyObject * /*o*/) -> void * { return nullptr; }

extern "C" void PyUnicode_WRITE(int /*kind*/, void * /*value*/,
                                Py_ssize_t /*index*/, Py_UCS4 /*value*/) {}

extern "C" auto PyUnicode_READ(int /*kind*/, void * /*value*/,
                               Py_ssize_t /*index*/) -> Py_UCS4 {
  return 0;
}

extern "C" auto PyUnicode_READ_CHAR(PyObject * /*o*/, Py_ssize_t /*index*/)
    -> Py_UCS4 {
  return 0;
}

extern "C" auto PyUnicode_ClearFreeList() -> int { return 0; }

extern "C" auto PyUnicode_GET_SIZE(PyObject * /*o*/) -> Py_ssize_t { return 0; }

extern "C" auto PyUnicode_GET_DATA_SIZE(PyObject * /*o*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_AS_UNICODE(PyObject * /*o*/) -> Py_UNICODE * {
  return nullptr;
}

extern "C" auto Py_UNICODE_ISSPACE(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISLOWER(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISUPPER(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISTITLE(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISLINEBREAK(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISDECIMAL(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISDIGIT(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISNUMERIC(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISALPHA(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISALNUM(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_ISPRINTABLE(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_TOLOWER(Py_UNICODE /*ch*/) -> Py_UNICODE {
  return 0;
}

extern "C" auto Py_UNICODE_TOUPPER(Py_UNICODE /*ch*/) -> Py_UNICODE {
  return 0;
}

extern "C" auto Py_UNICODE_TOTITLE(Py_UNICODE /*ch*/) -> Py_UNICODE {
  return 0;
}

extern "C" auto Py_UNICODE_TODECIMAL(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_TODIGIT(Py_UNICODE /*ch*/) -> int { return 0; }

extern "C" auto Py_UNICODE_TONUMERIC(Py_UNICODE /*ch*/) -> double {
  return 0.0;
}

extern "C" auto PyUnicode_New(Py_ssize_t /*size*/, Py_UCS4 /*maxchar*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FromKindAndData(int /*kind*/, const void * /*buffer*/,
                                          Py_ssize_t /*size*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FromStringAndSize(const char * /*u*/,
                                            Py_ssize_t /*size*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FromString(const char * /*u*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FromFormat(const char * /*format*/, ...)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FromFormatV(const char * /*format*/,
                                      va_list /*vargs*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FromEncodedObject(PyObject * /*obj*/,
                                            const char * /*encoding*/,
                                            const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_GetLength(PyObject * /*unicode*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto
PyUnicode_CopyCharacters(PyObject * /*to*/, Py_ssize_t /*to_start*/,
                         PyObject * /*from*/, Py_ssize_t /*from_start*/,
                         Py_ssize_t /*how_many*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_Fill(PyObject * /*unicode*/, Py_ssize_t /*start*/,
                               Py_ssize_t /*length*/, Py_UCS4 /*fill_char*/)
    -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_WriteChar(PyObject * /*unicode*/,
                                    Py_ssize_t /*index*/, Py_UCS4 /*character*/)
    -> int {
  return 0;
}

extern "C" auto PyUnicode_ReadChar(PyObject * /*unicode*/, Py_ssize_t /*index*/)
    -> Py_UCS4 {
  return 0;
}

extern "C" auto PyUnicode_Substring(PyObject * /*str*/, Py_ssize_t /*start*/,
                                    Py_ssize_t /*end*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUCS4(PyObject * /*u*/, Py_UCS4 * /*buffer*/,
                                 Py_ssize_t /*buflen*/, int /*copy_null*/)
    -> Py_UCS4 * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUCS4Copy(PyObject * /*u*/) -> Py_UCS4 * {
  return nullptr;
}

extern "C" auto PyUnicode_FromUnicode(const Py_UNICODE * /*u*/,
                                      Py_ssize_t /*size*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUnicode(PyObject * /*unicode*/) -> Py_UNICODE * {
  return nullptr;
}

extern "C" auto PyUnicode_TransformDecimalToASCII(Py_UNICODE * /*s*/,
                                                  Py_ssize_t /*size*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUnicodeAndSize(PyObject * /*unicode*/,
                                           Py_ssize_t * /*size*/)
    -> Py_UNICODE * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUnicodeCopy(PyObject * /*unicode*/)
    -> Py_UNICODE * {
  return nullptr;
}

extern "C" auto PyUnicode_GetSize(PyObject * /*unicode*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_FromObject(PyObject * /*obj*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeLocaleAndSize(const char * /*str*/,
                                              Py_ssize_t /*len*/,
                                              const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeLocale(const char * /*str*/,
                                       const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeLocale(PyObject * /*unicode*/,
                                       const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FSConverter(PyObject * /*obj*/, void * /*result*/)
    -> int {
  return 0;
}

extern "C" auto PyUnicode_FSDecoder(PyObject * /*obj*/, void * /*result*/)
    -> int {
  return 0;
}

extern "C" auto PyUnicode_DecodeFSDefaultAndSize(const char * /*s*/,
                                                 Py_ssize_t /*size*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeFSDefault(const char * /*s*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeFSDefault(PyObject * /*unicode*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_FromWideChar(const wchar_t * /*w*/,
                                       Py_ssize_t /*size*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsWideChar(PyUnicodeObject * /*unicode*/,
                                     wchar_t * /*w*/, Py_ssize_t /*size*/)
    -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_AsWideCharString(PyObject * /*unicode*/,
                                           Py_ssize_t * /*size*/) -> wchar_t * {
  return nullptr;
}

extern "C" auto PyUnicode_Decode(const char * /*s*/, Py_ssize_t /*size*/,
                                 const char * /*encoding*/,
                                 const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsEncodedString(PyObject * /*unicode*/,
                                          const char * /*encoding*/,
                                          const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Encode(const Py_UNICODE * /*s*/, Py_ssize_t /*size*/,
                                 const char * /*encoding*/,
                                 const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeUTF8(const char * /*s*/, Py_ssize_t /*size*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto
PyUnicode_DecodeUTF8Stateful(const char * /*s*/, Py_ssize_t /*size*/,
                             const char * /*errors*/, Py_ssize_t * /*consumed*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUTF8String(PyObject * /*unicode*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUTF8AndSize(PyObject * /*unicode*/,
                                        Py_ssize_t * /*size*/) -> const char * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUTF8(PyObject * /*unicode*/) -> const char * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeUTF8(const Py_UNICODE * /*s*/,
                                     Py_ssize_t /*size*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeUTF32(const char * /*s*/, Py_ssize_t /*size*/,
                                      const char * /*errors*/,
                                      int * /*byteorder*/) -> PyObject * {
  return nullptr;
}

extern "C" auto
PyUnicode_DecodeUTF32Stateful(const char * /*s*/, Py_ssize_t /*size*/,
                              const char * /*errors*/, int * /*byteorder*/,
                              Py_ssize_t * /*consumed*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUTF32String(PyObject * /*unicode*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeUTF32(const Py_UNICODE * /*s*/,
                                      Py_ssize_t /*size*/,
                                      const char * /*errors*/,
                                      int /*byteorder*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeUTF16(const char * /*s*/, Py_ssize_t /*size*/,
                                      const char * /*errors*/,
                                      int * /*byteorder*/) -> PyObject * {
  return nullptr;
}

extern "C" auto
PyUnicode_DecodeUTF16Stateful(const char * /*s*/, Py_ssize_t /*size*/,
                              const char * /*errors*/, int * /*byteorder*/,
                              Py_ssize_t * /*consumed*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUTF16String(PyObject * /*unicode*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeUTF16(const Py_UNICODE * /*s*/,
                                      Py_ssize_t /*size*/,
                                      const char * /*errors*/,
                                      int /*byteorder*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeUTF7(const char * /*s*/, Py_ssize_t /*size*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto
PyUnicode_DecodeUTF7Stateful(const char * /*s*/, Py_ssize_t /*size*/,
                             const char * /*errors*/, Py_ssize_t * /*consumed*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeUTF7(const Py_UNICODE * /*s*/,
                                     Py_ssize_t /*size*/, int /*base64SetO*/,
                                     int /*base64WhiteSpace*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeUnicodeEscape(const char * /*s*/,
                                              Py_ssize_t /*size*/,
                                              const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsUnicodeEscapeString(PyObject * /*unicode*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeUnicodeEscape(const Py_UNICODE * /*s*/,
                                              Py_ssize_t /*size*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeRawUnicodeEscape(const char * /*s*/,
                                                 Py_ssize_t /*size*/,
                                                 const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsRawUnicodeEscapeString(PyObject * /*unicode*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeRawUnicodeEscape(const Py_UNICODE * /*s*/,
                                                 Py_ssize_t /*size*/,
                                                 const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeLatin1(const char * /*s*/, Py_ssize_t /*size*/,
                                       const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsLatin1String(PyObject * /*unicode*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeLatin1(const Py_UNICODE * /*s*/,
                                       Py_ssize_t /*size*/,
                                       const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeASCII(const char * /*s*/, Py_ssize_t /*size*/,
                                      const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsASCIIString(PyObject * /*unicode*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeASCII(const Py_UNICODE * /*s*/,
                                      Py_ssize_t /*size*/,
                                      const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeCharmap(const char * /*value*/,
                                        Py_ssize_t /*size*/,
                                        PyObject * /*mapping*/,
                                        const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsCharmapString(PyObject * /*unicode*/,
                                          PyObject * /*mapping*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeCharmap(const Py_UNICODE * /*s*/,
                                        Py_ssize_t /*size*/,
                                        PyObject * /*mapping*/,
                                        const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Translate(PyObject * /*unicode*/,
                                    PyObject * /*mapping*/,
                                    const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto
PyUnicode_TranslateCharmap(const Py_UNICODE * /*s*/, Py_ssize_t /*size*/,
                           PyObject * /*mapping*/, const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeMBCS(const char * /*s*/, Py_ssize_t /*size*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_DecodeMBCSStateful(const char * /*s*/, int /*size*/,
                                             const char * /*errors*/,
                                             int * /*consumed*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_AsMBCSString(PyObject * /*unicode*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeCodePage(int /*code_page*/,
                                         PyObject * /*unicode*/,
                                         const char * /*errors*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_EncodeMBCS(const Py_UNICODE * /*s*/,
                                     Py_ssize_t /*size*/,
                                     const char * /*errors*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Concat(PyObject * /*left*/, PyObject * /*right*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Split(PyObject * /*s*/, PyObject * /*sep*/,
                                Py_ssize_t /*maxsplit*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Splitlines(PyObject * /*s*/, int /*keepend*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Join(PyObject * /*separator*/, PyObject * /*seq*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Tailmatch(PyObject * /*str*/, PyObject * /*substr*/,
                                    Py_ssize_t /*start*/, Py_ssize_t /*end*/,
                                    int /*direction*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_Find(PyObject * /*str*/, PyObject * /*substr*/,
                               Py_ssize_t /*start*/, Py_ssize_t /*end*/,
                               int /*direction*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_FindChar(PyObject * /*str*/, Py_UCS4 /*ch*/,
                                   Py_ssize_t /*start*/, Py_ssize_t /*end*/,
                                   int /*direction*/) -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_Count(PyObject * /*str*/, PyObject * /*substr*/,
                                Py_ssize_t /*start*/, Py_ssize_t /*end*/)
    -> Py_ssize_t {
  return 0;
}

extern "C" auto PyUnicode_Replace(PyObject * /*str*/, PyObject * /*substr*/,
                                  PyObject * /*replstr*/,
                                  Py_ssize_t /*maxcount*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Compare(PyObject * /*left*/, PyObject * /*right*/)
    -> int {
  return 0;
}

extern "C" auto PyUnicode_CompareWithASCIIString(PyObject * /*uni*/,
                                                 const char * /*string*/)
    -> int {
  return 0;
}

extern "C" auto PyUnicode_RichCompare(PyObject * /*left*/, PyObject * /*right*/,
                                      int /*op*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Format(PyObject * /*format*/, PyObject * /*args*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyUnicode_Contains(PyObject * /*container*/,
                                   PyObject * /*element*/) -> int {
  return 0;
}

extern "C" void PyUnicode_InternInPlace(PyObject ** /*string*/) {}

extern "C" auto PyUnicode_InternFromString(const char * /*v*/) -> PyObject * {
  return nullptr;
}

extern "C" auto Py_Main(int /*argc*/, wchar_t ** /*argv*/) -> int { return 0; }

extern "C" auto PyRun_AnyFile(FILE * /*fp*/, const char * /*filename*/) -> int {
  return 0;
}

extern "C" auto PyRun_AnyFileFlags(FILE * /*fp*/, const char * /*filename*/,
                                   PyCompilerFlags * /*flags*/) -> int {
  return 0;
}

extern "C" auto PyRun_AnyFileEx(FILE * /*fp*/, const char * /*filename*/,
                                int /*closeit*/) -> int {
  return 0;
}

extern "C" auto PyRun_AnyFileExFlags(FILE * /*fp*/, const char * /*filename*/,
                                     int /*closeit*/,
                                     PyCompilerFlags * /*flags*/) -> int {
  return 0;
}

extern "C" auto PyRun_SimpleString(const char * /*command*/) -> int {
  return 0;
}

extern "C" auto PyRun_SimpleStringFlags(const char * /*command*/,
                                        PyCompilerFlags * /*flags*/) -> int {
  return 0;
}

extern "C" auto PyRun_SimpleFile(FILE * /*fp*/, const char * /*filename*/)
    -> int {
  return 0;
}

extern "C" auto PyRun_SimpleFileEx(FILE * /*fp*/, const char * /*filename*/,
                                   int /*closeit*/) -> int {
  return 0;
}

extern "C" auto PyRun_SimpleFileExFlags(FILE * /*fp*/,
                                        const char * /*filename*/,
                                        int /*closeit*/,
                                        PyCompilerFlags * /*flags*/) -> int {
  return 0;
}

extern "C" auto PyRun_InteractiveOne(FILE * /*fp*/, const char * /*filename*/)
    -> int {
  return 0;
}

extern "C" auto PyRun_InteractiveOneFlags(FILE * /*fp*/,
                                          const char * /*filename*/,
                                          PyCompilerFlags * /*flags*/) -> int {
  return 0;
}

extern "C" auto PyRun_InteractiveLoop(FILE * /*fp*/, const char * /*filename*/)
    -> int {
  return 0;
}

extern "C" auto PyRun_InteractiveLoopFlags(FILE * /*fp*/,
                                           const char * /*filename*/,
                                           PyCompilerFlags * /*flags*/) -> int {
  return 0;
}

extern "C" auto PyParser_SimpleParseString(const char * /*str*/, int /*start*/)
    -> struct _node * {
  return nullptr;
}

extern "C" auto PyParser_SimpleParseStringFlags(const char * /*str*/,
                                                int /*start*/, int /*flags*/)
    -> struct _node * {
  return nullptr;
}

extern "C" auto PyParser_SimpleParseStringFlagsFilename(
    const char * /*str*/, const char * /*filename*/, int /*start*/,
    int /*flags*/) -> struct _node * {
  return nullptr;
}

extern "C" auto PyParser_SimpleParseFile(FILE * /*fp*/,
                                         const char * /*filename*/,
                                         int /*start*/) -> struct _node * {
  return nullptr;
}

extern "C" auto PyParser_SimpleParseFileFlags(FILE * /*fp*/,
                                              const char * /*filename*/,
                                              int /*start*/, int /*flags*/)
    -> struct _node * {
  return nullptr;
}

extern "C" auto PyRun_String(const char * /*str*/, int /*start*/,
                             PyObject * /*globals*/, PyObject * /*locals*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyRun_StringFlags(const char * /*str*/, int /*start*/,
                                  PyObject * /*globals*/, PyObject * /*locals*/,
                                  PyCompilerFlags * /*flags*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyRun_File(FILE * /*fp*/, const char * /*filename*/,
                           int /*start*/, PyObject * /*globals*/,
                           PyObject * /*locals*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyRun_FileEx(FILE * /*fp*/, const char * /*filename*/,
                             int /*start*/, PyObject * /*globals*/,
                             PyObject * /*locals*/, int /*closeit*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyRun_FileFlags(FILE * /*fp*/, const char * /*filename*/,
                                int /*start*/, PyObject * /*globals*/,
                                PyObject * /*locals*/,
                                PyCompilerFlags * /*flags*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyRun_FileExFlags(FILE * /*fp*/, const char * /*filename*/,
                                  int /*start*/, PyObject * /*globals*/,
                                  PyObject * /*locals*/, int /*closeit*/,
                                  PyCompilerFlags * /*flags*/) -> PyObject * {
  return nullptr;
}

extern "C" auto Py_CompileString(const char * /*str*/,
                                 const char * /*filename*/, int /*start*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto Py_CompileStringFlags(const char * /*str*/,
                                      const char * /*filename*/, int /*start*/,
                                      PyCompilerFlags * /*flags*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto Py_CompileStringObject(const char * /*str*/,
                                       PyObject * /*filename*/, int /*start*/,
                                       PyCompilerFlags * /*flags*/,
                                       int /*optimize*/) -> PyObject * {
  return nullptr;
}

extern "C" auto Py_CompileStringExFlags(const char * /*str*/,
                                        const char * /*filename*/,
                                        int /*start*/,
                                        PyCompilerFlags * /*flags*/,
                                        int /*optimize*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyEval_EvalCode(PyObject * /*co*/, PyObject * /*globals*/,
                                PyObject * /*locals*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyEval_EvalCodeEx(PyObject * /*co*/, PyObject * /*globals*/,
                                  PyObject * /*locals*/, PyObject ** /*args*/,
                                  int /*argcount*/, PyObject ** /*kws*/,
                                  int /*kwcount*/, PyObject ** /*defs*/,
                                  int /*defcount*/, PyObject * /*kwdefs*/,
                                  PyObject * /*closure*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyEval_EvalFrame(PyFrameObject * /*f*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyEval_EvalFrameEx(PyFrameObject * /*f*/, int /*throwflag*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyEval_MergeCompilerFlags(PyCompilerFlags * /*cf*/) -> int {
  return 0;
}

extern "C" auto PyWeakref_NewRef(PyObject * /*ob*/, PyObject * /*callback*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyWeakref_NewProxy(PyObject * /*ob*/, PyObject * /*callback*/)
    -> PyObject * {
  return nullptr;
}

extern "C" auto PyWeakref_GetObject(PyObject * /*ref*/) -> PyObject * {
  return nullptr;
}

extern "C" auto PyWeakref_GET_OBJECT(PyObject * /*ref*/) -> PyObject * {
  return nullptr;
}

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-macro-usage,cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,cppcoreguidelines-pro-type-reinterpret-cast,hicpp-no-malloc)
