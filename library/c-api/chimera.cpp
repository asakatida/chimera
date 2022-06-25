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

#include <cassert>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#undef Py_TYPE
// NOLINTNEXTLINE
static auto &Py_TYPE(PyObject *o) { return o->ob_type; }

// NOLINTNEXTLINE
static auto &Py_TYPE(PyVarObject *o) { return Py_TYPE(&o->ob_base); }

// NOLINTNEXTLINE
static auto &Py_TYPE(PyTypeObject *o) { return Py_TYPE(&o->ob_base); }

#undef Py_REFCNT
// NOLINTNEXTLINE
static auto &Py_REFCNT(PyObject *o) { return o->ob_refcnt; }

// NOLINTNEXTLINE
static auto &Py_REFCNT(PyVarObject *o) { return Py_REFCNT(&o->ob_base); }

// static auto &Py_REFCNT(PyTypeObject *o) { return Py_REFCNT(&o->ob_base); }

#undef Py_SIZE
// NOLINTNEXTLINE
static auto &Py_SIZE(PyVarObject *o) { return o->ob_size; }

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

// NOLINTNEXTLINE
PyObject PyType_Type{PyObject_HEAD_INIT(&PyType_Type)};

// NOLINTNEXTLINE
extern "C" int PyType_Check(PyObject *o) {
  return static_cast<int>(Py_TYPE(o) == &PyType_Type);
}

// NOLINTNEXTLINE
extern "C" int PyType_CheckExact(PyObject *o) {
  return static_cast<int>(Py_TYPE(o) == &PyType_Type);
}

// NOLINTNEXTLINE
extern "C" unsigned int PyType_ClearCache() { return 0; }

// NOLINTNEXTLINE
extern "C" long PyType_GetFlags(PyTypeObject *type) {
  // NOLINTNEXTLINE
  return static_cast<long>(type->tp_flags);
}

// NOLINTNEXTLINE
extern "C" void PyType_Modified(PyTypeObject * /*type*/) {}

// NOLINTNEXTLINE
extern "C" int PyType_HasFeature(PyTypeObject *o, int feature) {
  return static_cast<int>((o->tp_flags & static_cast<size_t>(feature)) != 0);
}

// NOLINTNEXTLINE
extern "C" int PyType_IS_GC(PyTypeObject *o) {
  return PyType_HasFeature(o, Py_TPFLAGS_HAVE_GC);
}

// NOLINTNEXTLINE
extern "C" int PyType_IsSubtype(PyTypeObject *a, PyTypeObject *b) {
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

// NOLINTNEXTLINE
extern "C" PyObject *PyType_GenericAlloc(PyTypeObject *type,
                                         Py_ssize_t nitems) {
  if (type->tp_itemsize != 0) {
    // NOLINTNEXTLINE
    auto object = reinterpret_cast<PyVarObject *>(std::calloc(
        (static_cast<size_t>(type->tp_basicsize) +
         static_cast<size_t>(nitems) * static_cast<size_t>(type->tp_itemsize)) /
                sizeof(PyVarObject) +
            1,
        sizeof(PyVarObject)));
    if (object == nullptr) {
      return nullptr;
    }
    Py_SIZE(object) = static_cast<size_t>(nitems);
    Py_REFCNT(object) = 1;
    // NOLINTNEXTLINE
    Py_TYPE(object) = reinterpret_cast<PyObject *>(type);
    // NOLINTNEXTLINE
    return reinterpret_cast<PyObject *>(object);
  }
  // NOLINTNEXTLINE
  auto object = reinterpret_cast<PyObject *>(
      // NOLINTNEXTLINE
      std::calloc(1, static_cast<size_t>(type->tp_basicsize)));
  if (object == nullptr) {
    return nullptr;
  }
  Py_REFCNT(object) = 1;
  // NOLINTNEXTLINE
  Py_TYPE(object) = reinterpret_cast<PyObject *>(type);
  return object;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyType_GenericNew(PyTypeObject *type, PyObject * /*args*/,
                                       PyObject * /*kwds*/) {
  return type->tp_alloc(type, 0);
}

// NOLINTNEXTLINE
extern "C" int PyType_Ready(PyTypeObject *type) {
  if (Py_TYPE(type) == nullptr) {
    // NOLINTNEXTLINE
    Py_TYPE(type) = reinterpret_cast<PyObject *>(&PyType_Type);
  }
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyType_FromSpec(PyType_Spec * /*spec*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyType_FromSpecWithBases(PyType_Spec * /*spec*/,
                                              PyObject * /*bases*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyType_GetSlot(PyTypeObject * /*type*/, int /*slot*/) {
  return nullptr;
}

// NOLINTNEXTLINE
PyTypeObject PyBaseObject_Type{
    {PyVarObject_HEAD_INIT(&PyType_Type, sizeof(PyTypeObject))},
    "object",
    sizeof(PyTypeObject),
    0,
    // NOLINTNEXTLINE
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

// NOLINTNEXTLINE
extern "C" PyObject *_PyObject_New(PyTypeObject * /*type*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyVarObject *_PyObject_NewVar(PyTypeObject * /*type*/,
                                         Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_Init(PyObject *op, PyTypeObject *type) {
  // NOLINTNEXTLINE
  Py_TYPE(op) = reinterpret_cast<PyObject *>(type);
  return op;
}

// NOLINTNEXTLINE
extern "C" PyVarObject *PyObject_InitVar(PyVarObject *op, PyTypeObject *type,
                                         Py_ssize_t size) {
  // NOLINTNEXTLINE
  op = reinterpret_cast<PyVarObject *>(
      // NOLINTNEXTLINE
      PyObject_Init(reinterpret_cast<PyObject *>(op), type));
  Py_SIZE(op) = static_cast<size_t>(size);
  return op;
}

// NOLINTNEXTLINE
extern "C" void PyObject_Del(PyObject *op) { std::free(op); }

// NOLINTNEXTLINE
extern "C" int PyArg_ParseTuple(PyObject * /*args*/, const char * /*format*/,
                                ...) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyArg_VaParse(PyObject * /*args*/, const char * /*format*/,
                             va_list /*vargs*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyArg_ParseTupleAndKeywords(PyObject * /*args*/,
                                           PyObject * /*kw*/,
                                           const char * /*format*/,
                                           char * /*keywords*/[], ...) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyArg_VaParseTupleAndKeywords(PyObject * /*args*/,
                                             PyObject * /*kw*/,
                                             const char * /*format*/,
                                             char * /*keywords*/[],
                                             va_list /*vargs*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyArg_ValidateKeywordArguments(PyObject * /*unused*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyArg_Parse(PyObject * /*args*/, const char * /*format*/, ...) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyArg_UnpackTuple(PyObject * /*args*/, const char * /*name*/,
                                 Py_ssize_t /*min*/, Py_ssize_t /*max*/, ...) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *Py_BuildValue(const char * /*format*/, ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *Py_VaBuildValue(const char * /*format*/,
                                     va_list /*vargs*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyBool_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyBool_FromLong(long /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyObject_CheckBuffer(PyObject * /*obj*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyObject_GetBuffer(PyObject * /*exporter*/, Py_buffer * /*view*/,
                                  int /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyBuffer_Release(Py_buffer * /*view*/) {}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyBuffer_SizeFromFormat(const char * /*unused*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyBuffer_IsContiguous(Py_buffer * /*view*/, char /*order*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyBuffer_FillContiguousStrides(int /*ndim*/,
                                               Py_ssize_t * /*shape*/,
                                               Py_ssize_t * /*strides*/,
                                               Py_ssize_t /*itemsize*/,
                                               char /*order*/) {}

// NOLINTNEXTLINE
extern "C" int PyBuffer_FillInfo(Py_buffer * /*view*/, PyObject * /*exporter*/,
                                 void * /*buf*/, Py_ssize_t /*len*/,
                                 int /*readonly*/, int /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyByteArray_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyByteArray_CheckExact(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyByteArray_FromObject(PyObject * /*o*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyByteArray_FromStringAndSize(const char * /*string*/,
                                                   Py_ssize_t /*len*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyByteArray_Concat(PyObject * /*a*/, PyObject * /*b*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyByteArray_Size(PyObject * /*bytearray*/) { return 0; }

// NOLINTNEXTLINE
extern "C" char *PyByteArray_AsString(PyObject * /*bytearray*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyByteArray_Resize(PyObject * /*bytearray*/,
                                  Py_ssize_t /*len*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" char *PyByteArray_AS_STRING(PyObject * /*bytearray*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyByteArray_GET_SIZE(PyObject * /*bytearray*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyBytes_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyBytes_CheckExact(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyBytes_FromString(const char * /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyBytes_FromStringAndSize(const char * /*v*/,
                                               Py_ssize_t /*len*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyBytes_FromFormat(const char * /*format*/, ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyBytes_FromFormatV(const char * /*format*/,
                                         va_list /*vargs*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyBytes_FromObject(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyBytes_Size(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyBytes_GET_SIZE(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" char *PyBytes_AsString(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" char *PyBytes_AS_STRING(PyObject * /*string*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyBytes_AsStringAndSize(PyObject * /*obj*/, char ** /*buffer*/,
                                       Py_ssize_t * /*length*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyBytes_Concat(PyObject ** /*bytes*/, PyObject * /*newpart*/) {}

// NOLINTNEXTLINE
extern "C" void PyBytes_ConcatAndDel(PyObject ** /*bytes*/,
                                     PyObject * /*newpart*/) {}

// NOLINTNEXTLINE
extern "C" int _PyBytes_Resize(PyObject ** /*bytes*/, Py_ssize_t /*newsize*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCapsule_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyCapsule_New(void * /*pointer*/, const char * /*name*/,
                                   PyCapsule_Destructor /*destructor*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyCapsule_GetPointer(PyObject * /*capsule*/,
                                      const char * /*name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyCapsule_Destructor
PyCapsule_GetDestructor(PyObject * /*capsule*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyCapsule_GetContext(PyObject * /*capsule*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" const char *PyCapsule_GetName(PyObject * /*capsule*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyCapsule_Import(const char * /*name*/, int /*no_block*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyCapsule_IsValid(PyObject * /*capsule*/,
                                 const char * /*name*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCapsule_SetContext(PyObject * /*capsule*/,
                                    void * /*context*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCapsule_SetDestructor(PyObject * /*capsule*/,
                                       PyCapsule_Destructor /*destructor*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCapsule_SetName(PyObject * /*capsule*/,
                                 const char * /*name*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCapsule_SetPointer(PyObject * /*capsule*/,
                                    void * /*pointer*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCell_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyCell_New(PyObject * /*ob*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyCell_Get(PyObject * /*cell*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyCell_GET(PyObject * /*cell*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyCell_Set(PyObject * /*cell*/, PyObject * /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyCell_SET(PyObject * /*cell*/, PyObject * /*value*/) {}

// NOLINTNEXTLINE
extern "C" int PyCode_Check(PyObject * /*co*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyCode_GetNumFree(PyCodeObject * /*co*/) { return 0; }

extern "C" PyCodeObject *
// NOLINTNEXTLINE
PyCode_New(int /*argcount*/, int /*kwonlyargcount*/, int /*nlocals*/,
           int /*stacksize*/, int /*flags*/, PyObject * /*code*/,
           PyObject * /*consts*/, PyObject * /*names*/, PyObject * /*varnames*/,
           PyObject * /*freevars*/, PyObject * /*cellvars*/,
           PyObject * /*filename*/, PyObject * /*name*/, int /*firstlineno*/,
           PyObject * /*lnotab*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyCodeObject *PyCode_NewEmpty(const char * /*filename*/,
                                         const char * /*funcname*/,
                                         int /*firstlineno*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyCodec_Register(PyObject * /*search_function*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyCodec_KnownEncoding(const char * /*encoding*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_Encode(PyObject * /*object*/,
                                    const char * /*encoding*/,
                                    const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_Decode(PyObject * /*object*/,
                                    const char * /*encoding*/,
                                    const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_Encoder(const char * /*encoding*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_Decoder(const char * /*encoding*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_IncrementalEncoder(const char * /*encoding*/,
                                                const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_IncrementalDecoder(const char * /*encoding*/,
                                                const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_StreamReader(const char * /*encoding*/,
                                          PyObject * /*stream*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_StreamWriter(const char * /*encoding*/,
                                          PyObject * /*stream*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyCodec_RegisterError(const char * /*name*/,
                                     PyObject * /*error*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_LookupError(const char * /*name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_StrictErrors(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_IgnoreErrors(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_ReplaceErrors(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_XMLCharRefReplaceErrors(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_BackslashReplaceErrors(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyCodec_NameReplaceErrors(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_complex _Py_c_sum(Py_complex /*left*/, Py_complex /*right*/) {
  return {};
}

// NOLINTNEXTLINE
extern "C" Py_complex _Py_c_diff(Py_complex /*left*/, Py_complex /*right*/) {
  return {};
}

// NOLINTNEXTLINE
extern "C" Py_complex _Py_c_neg(Py_complex /*complex*/) { return {}; }

// NOLINTNEXTLINE
extern "C" Py_complex _Py_c_prod(Py_complex /*left*/, Py_complex /*right*/) {
  return {};
}

// NOLINTNEXTLINE
extern "C" Py_complex _Py_c_quot(Py_complex /*dividend*/,
                                 Py_complex /*divisor*/) {
  return {};
}

// NOLINTNEXTLINE
extern "C" Py_complex _Py_c_pow(Py_complex /*num*/, Py_complex /*exp*/) {
  return {};
}

// NOLINTNEXTLINE
extern "C" int PyComplex_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyComplex_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyComplex_FromCComplex(Py_complex /*v*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyComplex_FromDoubles(double /*real*/, double /*imag*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" double PyComplex_RealAsDouble(PyObject * /*op*/) { return 0.0; }

// NOLINTNEXTLINE
extern "C" double PyComplex_ImagAsDouble(PyObject * /*op*/) { return 0.0; }

// NOLINTNEXTLINE
extern "C" Py_complex PyComplex_AsCComplex(PyObject * /*op*/) { return {}; }

// NOLINTNEXTLINE
extern "C" int PyOS_snprintf(char * /*str*/, size_t /*size*/,
                             const char * /*format*/, ...) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyOS_vsnprintf(char * /*str*/, size_t /*size*/,
                              const char * /*format*/, va_list /*va*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" double PyOS_string_to_double(const char * /*s*/, char ** /*endptr*/,
                                        PyObject * /*overflow_exception*/) {
  return 0.0;
}

// NOLINTNEXTLINE
extern "C" char *PyOS_double_to_string(double /*val*/, char /*format_code*/,
                                       int /*precision*/, int /*flags*/,
                                       int * /*ptype*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyOS_stricmp(const char * /*s1*/, const char * /*s2*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyOS_strnicmp(const char * /*s1*/, const char * /*s2*/,
                             Py_ssize_t /*size*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCoro_CheckExact(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyCoro_New(PyFrameObject * /*frame*/, PyObject * /*name*/,
                                PyObject * /*qualname*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyDate_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDate_CheckExact(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_CheckExact(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyTime_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyTime_CheckExact(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDelta_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDelta_CheckExact(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyTZInfo_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyTZInfo_CheckExact(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyDate_FromDate(int /*year*/, int /*month*/, int /*day*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDateTime_FromDateAndTime(int /*year*/, int /*month*/,
                                                int /*day*/, int /*hour*/,
                                                int /*minute*/, int /*second*/,
                                                int /*usecond*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyTime_FromTime(int /*hour*/, int /*minute*/,
                                     int /*second*/, int /*usecond*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDelta_FromDSU(int /*days*/, int /*seconds*/,
                                     int /*useconds*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyDateTime_GET_YEAR(PyDateTime_Date * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_GET_MONTH(PyDateTime_Date * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_GET_DAY(PyDateTime_Date * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_DATE_GET_HOUR(PyDateTime_DateTime * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDateTime_DATE_GET_MINUTE(PyDateTime_DateTime * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDateTime_DATE_GET_SECOND(PyDateTime_DateTime * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDateTime_DATE_GET_MICROSECOND(PyDateTime_DateTime * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDateTime_TIME_GET_HOUR(PyDateTime_Time * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_TIME_GET_MINUTE(PyDateTime_Time * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_TIME_GET_SECOND(PyDateTime_Time * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_TIME_GET_MICROSECOND(PyDateTime_Time * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDateTime_DELTA_GET_DAYS(PyDateTime_Delta * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDateTime_DELTA_GET_SECONDS(PyDateTime_Delta * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDateTime_DELTA_GET_MICROSECOND(PyDateTime_Delta * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDateTime_FromTimestamp(PyObject * /*args*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDate_FromTimestamp(PyObject * /*args*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDescr_NewGetSet(PyTypeObject * /*type*/,
                                       struct PyGetSetDef * /*getset*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDescr_NewMember(PyTypeObject * /*type*/,
                                       struct PyMemberDef * /*meth*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDescr_NewMethod(PyTypeObject * /*type*/,
                                       struct PyMethodDef * /*meth*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDescr_NewWrapper(PyTypeObject * /*type*/,
                                        struct wrapperbase * /*wrapper*/,
                                        void * /*wrapped*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDescr_NewClassMethod(PyTypeObject * /*type*/,
                                            PyMethodDef * /*method*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyDescr_IsData(PyObject * /*descr*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyWrapper_New(PyObject * /*unused*/,
                                   PyObject * /*unused*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyDict_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDict_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_New() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyDictProxy_New(PyObject * /*mapping*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyDict_Clear(PyObject * /*p*/) {}

// NOLINTNEXTLINE
extern "C" int PyDict_Contains(PyObject * /*p*/, PyObject * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_Copy(PyObject * /*p*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyDict_SetItem(PyObject * /*p*/, PyObject * /*key*/,
                              PyObject * /*val*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDict_SetItemString(PyObject * /*p*/, const char * /*key*/,
                                    PyObject * /*val*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDict_DelItem(PyObject * /*p*/, PyObject * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDict_DelItemString(PyObject * /*p*/, const char * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_GetItem(PyObject * /*p*/, PyObject * /*key*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_GetItemWithError(PyObject * /*p*/,
                                             PyObject * /*key*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_GetItemString(PyObject * /*p*/,
                                          const char * /*key*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_SetDefault(PyObject * /*p*/, PyObject * /*key*/,
                                       PyObject * /*d*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_Items(PyObject * /*p*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_Keys(PyObject * /*p*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyDict_Values(PyObject * /*p*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyDict_Size(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDict_Next(PyObject * /*p*/, Py_ssize_t * /*ppos*/,
                           PyObject ** /*pkey*/, PyObject ** /*pvalue*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDict_Merge(PyObject * /*a*/, PyObject * /*b*/,
                            int /*override*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDict_Update(PyObject * /*a*/, PyObject * /*b*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyDict_MergeFromSeq2(PyObject * /*a*/, PyObject * /*seq2*/,
                                    int /*override*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyDict_ClearFreeList() { return 0; }

// NOLINTNEXTLINE
extern "C" void PyErr_Clear() {}

// NOLINTNEXTLINE
extern "C" void PyErr_PrintEx(int /*set_sys_last_vars*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_Print() {}

// NOLINTNEXTLINE
extern "C" void PyErr_WriteUnraisable(PyObject * /*obj*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_SetString(PyObject * /*type*/, const char * /*message*/) {
}

// NOLINTNEXTLINE
extern "C" void PyErr_SetObject(PyObject * /*type*/, PyObject * /*value*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_Format(PyObject * /*exception*/,
                                  const char * /*format*/, ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_FormatV(PyObject * /*exception*/,
                                   const char * /*format*/, va_list /*vargs*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyErr_SetNone(PyObject * /*type*/) {}

// NOLINTNEXTLINE
extern "C" int PyErr_BadArgument() { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_NoMemory() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_SetFromErrno(PyObject * /*type*/) { return nullptr; }

extern "C" PyObject *
// NOLINTNEXTLINE
PyErr_SetFromErrnoWithFilenameObject(PyObject * /*type*/,
                                     PyObject * /*filenameObject*/) {
  return nullptr;
}

extern "C" PyObject *
// NOLINTNEXTLINE
PyErr_SetFromErrnoWithFilenameObjects(PyObject * /*type*/,
                                      PyObject * /*filenameObject*/,
                                      PyObject * /*filenameObject2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_SetFromErrnoWithFilename(PyObject * /*type*/,
                                                    const char * /*filename*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_SetFromWindowsErr(int /*ierr*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_SetExcFromWindowsErr(PyObject * /*type*/,
                                                int /*ierr*/) {
  return nullptr;
}

extern "C" PyObject *
// NOLINTNEXTLINE
PyErr_SetFromWindowsErrWithFilename(int /*ierr*/, const char * /*filename*/) {
  return nullptr;
}

extern "C" PyObject *
// NOLINTNEXTLINE
PyErr_SetExcFromWindowsErrWithFilenameObject(PyObject * /*type*/, int /*ierr*/,
                                             PyObject * /*filename*/) {
  return nullptr;
}

extern "C" PyObject *
// NOLINTNEXTLINE
PyErr_SetExcFromWindowsErrWithFilenameObjects(PyObject * /*type*/, int /*ierr*/,
                                              PyObject * /*filename*/,
                                              PyObject * /*filename2*/) {
  return nullptr;
}

extern "C" PyObject *
// NOLINTNEXTLINE
PyErr_SetExcFromWindowsErrWithFilename(PyObject * /*type*/, int /*ierr*/,
                                       const char * /*filename*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_SetImportError(PyObject * /*msg*/,
                                          PyObject * /*name*/,
                                          PyObject * /*path*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyErr_SyntaxLocationObject(PyObject * /*filename*/,
                                           int /*lineno*/, int /*col_offset*/) {
}

// NOLINTNEXTLINE
extern "C" void PyErr_SyntaxLocationEx(const char * /*filename*/,
                                       int /*lineno*/, int /*col_offset*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_SyntaxLocation(const char * /*filename*/,
                                     int /*lineno*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_BadInternalCall() {}

// NOLINTNEXTLINE
extern "C" int PyErr_WarnEx(PyObject * /*category*/, const char * /*message*/,
                            Py_ssize_t /*stack_level*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_SetImportErrorSubclass(PyObject * /*msg*/,
                                                  PyObject * /*name*/,
                                                  PyObject * /*path*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyErr_WarnExplicitObject(PyObject * /*category*/,
                                        PyObject * /*message*/,
                                        PyObject * /*filename*/, int /*lineno*/,
                                        PyObject * /*module*/,
                                        PyObject * /*registry*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyErr_WarnExplicit(PyObject * /*category*/,
                                  const char * /*message*/,
                                  const char * /*filename*/, int /*lineno*/,
                                  const char * /*module*/,
                                  PyObject * /*registry*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyErr_WarnFormat(PyObject * /*category*/,
                                Py_ssize_t /*stack_level*/,
                                const char * /*format*/, ...) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyErr_ResourceWarning(PyObject * /*source*/,
                                     Py_ssize_t /*stack_level*/,
                                     const char * /*format*/, ...) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_Occurred() { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyErr_ExceptionMatches(PyObject * /*exc*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyErr_GivenExceptionMatches(PyObject * /*given*/,
                                           PyObject * /*exc*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyErr_Fetch(PyObject ** /*ptype*/, PyObject ** /*pvalue*/,
                            PyObject ** /*ptraceback*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_Restore(PyObject * /*type*/, PyObject * /*value*/,
                              PyObject * /*traceback*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_NormalizeException(PyObject ** /*exc*/,
                                         PyObject ** /*val*/,
                                         PyObject ** /*tb*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_GetExcInfo(PyObject ** /*ptype*/, PyObject ** /*pvalue*/,
                                 PyObject ** /*ptraceback*/) {}

// NOLINTNEXTLINE
extern "C" void PyErr_SetExcInfo(PyObject * /*type*/, PyObject * /*value*/,
                                 PyObject * /*traceback*/) {}

// NOLINTNEXTLINE
extern "C" int PyErr_CheckSignals() { return 0; }

// NOLINTNEXTLINE
extern "C" void PyErr_SetInterrupt() {}

// NOLINTNEXTLINE
extern "C" int PySignal_SetWakeupFd(int /*fd*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_NewException(const char * /*name*/,
                                        PyObject * /*base*/,
                                        PyObject * /*dict*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyErr_NewExceptionWithDoc(const char * /*name*/,
                                               const char * /*doc*/,
                                               PyObject * /*base*/,
                                               PyObject * /*dict*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyException_GetTraceback(PyObject * /*ex*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyException_SetTraceback(PyObject * /*ex*/, PyObject * /*tb*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyException_GetContext(PyObject * /*ex*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyException_SetContext(PyObject * /*ex*/, PyObject * /*ctx*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PyException_GetCause(PyObject * /*ex*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyException_SetCause(PyObject * /*ex*/, PyObject * /*cause*/) {}

extern "C" PyObject *
// NOLINTNEXTLINE
PyUnicodeDecodeError_Create(const char * /*encoding*/, const char * /*object*/,
                            Py_ssize_t /*length*/, Py_ssize_t /*start*/,
                            Py_ssize_t /*end*/, const char * /*reason*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicodeEncodeError_Create(const char * /*encoding*/,
                                                 const Py_UNICODE * /*object*/,
                                                 Py_ssize_t /*length*/,
                                                 Py_ssize_t /*start*/,
                                                 Py_ssize_t /*end*/,
                                                 const char * /*reason*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *
PyUnicodeTranslateError_Create(const Py_UNICODE * /*object*/,
                               Py_ssize_t /*length*/, Py_ssize_t /*start*/,
                               Py_ssize_t /*end*/, const char * /*reason*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicodeDecodeError_GetEncoding(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicodeDecodeError_GetObject(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyUnicodeDecodeError_GetStart(PyObject * /*exc*/,
                                             Py_ssize_t * /*start*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyUnicodeDecodeError_SetStart(PyObject * /*exc*/,
                                             Py_ssize_t /*start*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyUnicodeDecodeError_GetEnd(PyObject * /*exc*/,
                                           Py_ssize_t * /*end*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyUnicodeDecodeError_SetEnd(PyObject * /*exc*/,
                                           Py_ssize_t /*end*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicodeDecodeError_GetReason(PyObject * /*exc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyUnicodeDecodeError_SetReason(PyObject * /*exc*/,
                                              const char * /*reason*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int Py_EnterRecursiveCall(const char * /*where*/) { return 0; }

// NOLINTNEXTLINE
extern "C" void Py_LeaveRecursiveCall() {}

// NOLINTNEXTLINE
extern "C" int Py_ReprEnter(PyObject * /*object*/) { return 0; }

// NOLINTNEXTLINE
extern "C" void Py_ReprLeave(PyObject * /*object*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PyFile_FromFd(int /*fd*/, const char * /*name*/,
                                   const char * /*mode*/, int /*buffering*/,
                                   const char * /*encoding*/,
                                   const char * /*errors*/,
                                   const char * /*newline*/, int /*closefd*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyObject_AsFileDescriptor(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFile_GetLine(PyObject * /*p*/, int /*n*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyFile_WriteObject(PyObject * /*obj*/, PyObject * /*p*/,
                                  int /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyFile_WriteString(const char * /*s*/, PyObject * /*p*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyFloat_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyFloat_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFloat_FromString(PyObject * /*str*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFloat_FromDouble(double /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" double PyFloat_AsDouble(PyObject * /*pyfloat*/) { return 0.0; }

// NOLINTNEXTLINE
extern "C" double PyFloat_AS_DOUBLE(PyObject * /*pyfloat*/) { return 0.0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFloat_GetInfo() { return nullptr; }

// NOLINTNEXTLINE
extern "C" double PyFloat_GetMax() { return 0.0; }

// NOLINTNEXTLINE
extern "C" double PyFloat_GetMin() { return 0.0; }

// NOLINTNEXTLINE
extern "C" int PyFloat_ClearFreeList() { return 0; }

// NOLINTNEXTLINE
extern "C" int PyFunction_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_New(PyObject * /*code*/,
                                    PyObject * /*globals*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_NewWithQualName(PyObject * /*code*/,
                                                PyObject * /*globals*/,
                                                PyObject * /*qualname*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_GetCode(PyObject * /*op*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_GetGlobals(PyObject * /*op*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_GetModule(PyObject * /*op*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_GetDefaults(PyObject * /*op*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyFunction_SetDefaults(PyObject * /*op*/,
                                      PyObject * /*defaults*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_GetClosure(PyObject * /*op*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyFunction_SetClosure(PyObject * /*op*/,
                                     PyObject * /*closure*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyFunction_GetAnnotations(PyObject * /*op*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyFunction_SetAnnotations(PyObject * /*op*/,
                                         PyObject * /*annotations*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyObject_GC_Track(PyObject * /*op*/) {}

// NOLINTNEXTLINE
extern "C" void _PyObject_GC_TRACK(PyObject * /*op*/) {}

// NOLINTNEXTLINE
extern "C" void PyObject_GC_Del(void * /*op*/) {}

// NOLINTNEXTLINE
extern "C" void PyObject_GC_UnTrack(void * /*op*/) {}

// NOLINTNEXTLINE
extern "C" void _PyObject_GC_UNTRACK(PyObject * /*op*/) {}

// NOLINTNEXTLINE
extern "C" void Py_VISIT(PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" int PyGen_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyGen_CheckExact(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyGen_New(PyFrameObject * /*frame*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyGen_NewWithQualName(PyFrameObject * /*frame*/,
                                           PyObject * /*name*/,
                                           PyObject * /*qualname*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ImportModule(const char * /*name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ImportModuleNoBlock(const char * /*name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ImportModuleEx(const char * /*name*/,
                                             PyObject * /*globals*/,
                                             PyObject * /*locals*/,
                                             PyObject * /*fromlist*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ImportModuleLevelObject(PyObject * /*name*/,
                                                      PyObject * /*globals*/,
                                                      PyObject * /*locals*/,
                                                      PyObject * /*fromlist*/,
                                                      int /*level*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ImportModuleLevel(const char * /*name*/,
                                                PyObject * /*globals*/,
                                                PyObject * /*locals*/,
                                                PyObject * /*fromlist*/,
                                                int /*level*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_Import(PyObject * /*name*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ReloadModule(PyObject * /*m*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_AddModuleObject(PyObject * /*name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_AddModule(const char * /*name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ExecCodeModule(const char * /*name*/,
                                             PyObject * /*co*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ExecCodeModuleEx(const char * /*name*/,
                                               PyObject * /*co*/,
                                               const char * /*pathname*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_ExecCodeModuleObject(PyObject * /*name*/,
                                                   PyObject * /*co*/,
                                                   PyObject * /*pathname*/,
                                                   PyObject * /*cpathname*/) {
  return nullptr;
}

extern "C" PyObject *
// NOLINTNEXTLINE
PyImport_ExecCodeModuleWithPathnames(const char * /*name*/, PyObject * /*co*/,
                                     const char * /*pathname*/,
                                     const char * /*cpathname*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" long PyImport_GetMagicNumber() { return 0; }

// NOLINTNEXTLINE
extern "C" const char *PyImport_GetMagicTag() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_GetModuleDict() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyImport_GetImporter(PyObject * /*path*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void _PyImport_Init() {}

// NOLINTNEXTLINE
extern "C" void PyImport_Cleanup() {}

// NOLINTNEXTLINE
extern "C" void _PyImport_Fini() {}

// NOLINTNEXTLINE
extern "C" int PyImport_ImportFrozenModuleObject(PyObject * /*name*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyImport_ImportFrozenModule(const char * /*name*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyImport_AppendInittab(const char * /*name*/,
                                      PyObject *(* /*initfunc*/)()) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyImport_ExtendInittab(struct _inittab * /*newtab*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void Py_Initialize() {}

// NOLINTNEXTLINE
extern "C" void Py_InitializeEx(int /*initsigs*/) {}

// NOLINTNEXTLINE
extern "C" int Py_IsInitialized() { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_FinalizeEx() { return 0; }

// NOLINTNEXTLINE
extern "C" void Py_Finalize() {}

// NOLINTNEXTLINE
extern "C" int Py_SetStandardStreamEncoding(const char * /*encoding*/,
                                            const char * /*errors*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void Py_SetProgramName(wchar_t * /*name*/) {}

// NOLINTNEXTLINE
extern "C" wchar_t *Py_GetProgramName() { return nullptr; }

// NOLINTNEXTLINE
extern "C" wchar_t *Py_GetPrefix() { return nullptr; }

// NOLINTNEXTLINE
extern "C" wchar_t *Py_GetExecPrefix() { return nullptr; }

// NOLINTNEXTLINE
extern "C" wchar_t *Py_GetProgramFullPath() { return nullptr; }

// NOLINTNEXTLINE
extern "C" wchar_t *Py_GetPath() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void Py_SetPath(const wchar_t * /*unused*/) {}

// NOLINTNEXTLINE
extern "C" const char *Py_GetVersion() { return nullptr; }

// NOLINTNEXTLINE
extern "C" const char *Py_GetPlatform() { return nullptr; }

// NOLINTNEXTLINE
extern "C" const char *Py_GetCopyright() { return nullptr; }

// NOLINTNEXTLINE
extern "C" const char *Py_GetCompiler() { return nullptr; }

// NOLINTNEXTLINE
extern "C" const char *Py_GetBuildInfo() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PySys_SetArgvEx(int /*argc*/, wchar_t ** /*argv*/,
                                int /*updatepath*/) {}

// NOLINTNEXTLINE
extern "C" void PySys_SetArgv(int /*argc*/, wchar_t ** /*argv*/) {}

// NOLINTNEXTLINE
extern "C" void Py_SetPythonHome(wchar_t * /*home*/) {}

// NOLINTNEXTLINE
extern "C" wchar_t *Py_GetPythonHome() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyEval_InitThreads() {}

// NOLINTNEXTLINE
extern "C" int PyEval_ThreadsInitialized() { return 0; }

// NOLINTNEXTLINE
extern "C" PyThreadState *PyEval_SaveThread() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyEval_RestoreThread(PyThreadState * /*tstate*/) {}

// NOLINTNEXTLINE
extern "C" PyThreadState *PyThreadState_Get() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyThreadState *PyThreadState_Swap(PyThreadState * /*tstate*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyEval_ReInitThreads() {}

// NOLINTNEXTLINE
extern "C" PyGILState_STATE PyGILState_Ensure() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyGILState_Release(PyGILState_STATE /*unused*/) {}

// NOLINTNEXTLINE
extern "C" PyThreadState *PyGILState_GetThisThreadState() { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyGILState_Check() { return 0; }

// NOLINTNEXTLINE
extern "C" PyInterpreterState *PyInterpreterState_New() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyInterpreterState_Clear(PyInterpreterState * /*interp*/) {}

// NOLINTNEXTLINE
extern "C" void PyInterpreterState_Delete(PyInterpreterState * /*interp*/) {}

// NOLINTNEXTLINE
extern "C" PyThreadState *PyThreadState_New(PyInterpreterState * /*interp*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyThreadState_Clear(PyThreadState * /*tstate*/) {}

// NOLINTNEXTLINE
extern "C" void PyThreadState_Delete(PyThreadState * /*tstate*/) {}

// NOLINTNEXTLINE
extern "C" PY_INT64_T
PyInterpreterState_GetID(PyInterpreterState * /*interp*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyThreadState_GetDict() { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyThreadState_SetAsyncExc(unsigned long /*id*/,
                                         PyObject * /*exc*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyEval_AcquireThread(PyThreadState * /*tstate*/) {}

// NOLINTNEXTLINE
extern "C" void PyEval_ReleaseThread(PyThreadState * /*tstate*/) {}

// NOLINTNEXTLINE
extern "C" void PyEval_AcquireLock() {}

// NOLINTNEXTLINE
extern "C" void PyEval_ReleaseLock() {}

// NOLINTNEXTLINE
extern "C" PyThreadState *Py_NewInterpreter() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void Py_EndInterpreter(PyThreadState * /*tstate*/) {}

// NOLINTNEXTLINE
extern "C" int Py_AddPendingCall(int (* /*func*/)(void *), void * /*arg*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyEval_SetProfile(Py_tracefunc /*func*/, PyObject * /*obj*/) {}

// NOLINTNEXTLINE
extern "C" void PyEval_SetTrace(Py_tracefunc /*func*/, PyObject * /*obj*/) {}

// NOLINTNEXTLINE
extern "C" PyInterpreterState *PyInterpreterState_Head() { return nullptr; }

extern "C" PyInterpreterState *
// NOLINTNEXTLINE
PyInterpreterState_Next(PyInterpreterState * /*interp*/) {
  return nullptr;
}

extern "C" PyThreadState *
// NOLINTNEXTLINE
PyInterpreterState_ThreadHead(PyInterpreterState * /*interp*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyThreadState *PyThreadState_Next(PyThreadState * /*tstate*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyIter_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyIter_Next(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PySeqIter_Check(PyObject * /*op*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PySeqIter_New(PyObject * /*seq*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyCallIter_Check(PyObject * /*op*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyCallIter_New(PyObject * /*callable*/,
                                    PyObject * /*sentinel*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyList_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyList_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyList_New(Py_ssize_t /*len*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyList_Size(PyObject * /*list*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyList_GET_SIZE(PyObject * /*list*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyList_GetItem(PyObject * /*list*/, Py_ssize_t /*index*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyList_GET_ITEM(PyObject * /*list*/, Py_ssize_t /*i*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyList_SetItem(PyObject * /*list*/, Py_ssize_t /*index*/,
                              PyObject * /*item*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyList_SET_ITEM(PyObject * /*list*/, Py_ssize_t /*i*/,
                                PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" int PyList_Insert(PyObject * /*list*/, Py_ssize_t /*index*/,
                             PyObject * /*item*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyList_Append(PyObject * /*list*/, PyObject * /*item*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyList_GetSlice(PyObject * /*list*/, Py_ssize_t /*low*/,
                                     Py_ssize_t /*high*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyList_SetSlice(PyObject * /*list*/, Py_ssize_t /*low*/,
                               Py_ssize_t /*high*/, PyObject * /*itemlist*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyList_Sort(PyObject * /*list*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyList_Reverse(PyObject * /*list*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyList_AsTuple(PyObject * /*list*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyList_ClearFreeList() { return 0; }

// NOLINTNEXTLINE
extern "C" int PyLong_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyLong_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromLong(long /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromUnsignedLong(unsigned long /*v*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromSsize_t(Py_ssize_t /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromSize_t(size_t /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromLongLong(long long /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromUnsignedLongLong(unsigned long long /*v*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromDouble(double /*v*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromString(const char * /*str*/, char ** /*pend*/,
                                       int /*base*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromUnicode(Py_UNICODE * /*u*/,
                                        Py_ssize_t /*length*/, int /*base*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromUnicodeObject(PyObject * /*u*/, int /*base*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyLong_FromVoidPtr(void * /*p*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" long PyLong_AsLong(PyObject * /*obj*/) { return 0; }

// NOLINTNEXTLINE
extern "C" long PyLong_AsLongAndOverflow(PyObject * /*obj*/,
                                         int * /*overflow*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" long long PyLong_AsLongLong(PyObject * /*obj*/) { return 0; }

// NOLINTNEXTLINE
extern "C" long long PyLong_AsLongLongAndOverflow(PyObject * /*obj*/,
                                                  int * /*overflow*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyLong_AsSsize_t(PyObject * /*pylong*/) { return 0; }

// NOLINTNEXTLINE
extern "C" unsigned long PyLong_AsUnsignedLong(PyObject * /*pylong*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" size_t PyLong_AsSize_t(PyObject * /*pylong*/) { return 0; }

// NOLINTNEXTLINE
extern "C" unsigned long long PyLong_AsUnsignedLongLong(PyObject * /*pylong*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" unsigned long PyLong_AsUnsignedLongMask(PyObject * /*obj*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" unsigned long long
PyLong_AsUnsignedLongLongMask(PyObject * /*obj*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" double PyLong_AsDouble(PyObject * /*pylong*/) { return 0.0; }

// NOLINTNEXTLINE
extern "C" void *PyLong_AsVoidPtr(PyObject * /*pylong*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyMapping_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyMapping_Size(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyMapping_DelItemString(PyObject * /*o*/, const char * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyMapping_DelItem(PyObject * /*o*/, PyObject * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyMapping_HasKeyString(PyObject * /*o*/, const char * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyMapping_HasKey(PyObject * /*o*/, PyObject * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMapping_Keys(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyMapping_Values(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyMapping_Items(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyMapping_GetItemString(PyObject * /*o*/,
                                             const char * /*key*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyMapping_SetItemString(PyObject * /*o*/, const char * /*key*/,
                                       PyObject * /*v*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyMarshal_WriteLongToFile(long /*value*/, FILE * /*file*/,
                                          int /*version*/) {}

// NOLINTNEXTLINE
extern "C" void PyMarshal_WriteObjectToFile(PyObject * /*value*/,
                                            FILE * /*file*/, int /*version*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PyMarshal_WriteObjectToString(PyObject * /*value*/,
                                                   int /*version*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" long PyMarshal_ReadLongFromFile(FILE * /*file*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyMarshal_ReadShortFromFile(FILE * /*file*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyMarshal_ReadObjectFromFile(FILE * /*file*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMarshal_ReadLastObjectFromFile(FILE * /*file*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMarshal_ReadObjectFromString(const char * /*value*/,
                                                    Py_ssize_t /*len*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyMem_RawMalloc(size_t /*n*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" void *PyMem_RawCalloc(size_t /*nelem*/, size_t /*elsize*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyMem_RawRealloc(void * /*p*/, size_t /*n*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyMem_RawFree(void * /*p*/) {}

// NOLINTNEXTLINE
extern "C" void *PyMem_Malloc(size_t /*n*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" void *PyMem_Calloc(size_t /*nelem*/, size_t /*elsize*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyMem_Realloc(void * /*p*/, size_t /*n*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyMem_Free(void * /*p*/) {}

// NOLINTNEXTLINE
extern "C" void PyMem_GetAllocator(PyMemAllocatorDomain /*domain*/,
                                   PyMemAllocatorEx * /*allocator*/) {}

// NOLINTNEXTLINE
extern "C" void PyMem_SetAllocator(PyMemAllocatorDomain /*domain*/,
                                   PyMemAllocatorEx * /*allocator*/) {}

// NOLINTNEXTLINE
extern "C" void PyMem_SetupDebugHooks() {}

extern "C" PyObjectArenaAllocator *
// NOLINTNEXTLINE
PyObject_GetArenaAllocator(PyObjectArenaAllocator * /*allocator*/) {
  return nullptr;
}

extern "C" PyObjectArenaAllocator *
// NOLINTNEXTLINE
PyObject_SetArenaAllocator(PyObjectArenaAllocator * /*allocator*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMemoryView_FromObject(PyObject * /*obj*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *
PyMemoryView_FromMemory(char * /*mem*/, Py_ssize_t /*size*/, int /*flags*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMemoryView_FromBuffer(Py_buffer * /*view*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMemoryView_GetContiguous(PyObject * /*obj*/,
                                                int /*buffertype*/,
                                                char /*order*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyMemoryView_Check(PyObject * /*obj*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_buffer *PyMemoryView_GET_BUFFER(PyObject * /*mview*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_buffer *PyMemoryView_GET_BASE(PyObject * /*mview*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyInstanceMethod_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyInstanceMethod_New(PyObject * /*func*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyInstanceMethod_Function(PyObject * /*im*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyInstanceMethod_GET_FUNCTION(PyObject * /*im*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyMethod_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyMethod_New(PyObject * /*func*/, PyObject * /*self*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMethod_Function(PyObject * /*meth*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyMethod_GET_FUNCTION(PyObject * /*meth*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyMethod_Self(PyObject * /*meth*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyMethod_GET_SELF(PyObject * /*meth*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyMethod_ClearFreeList() { return 0; }

// NOLINTNEXTLINE
extern "C" int PyModule_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyModule_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_NewObject(PyObject * /*name*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_New(const char * /*name*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_GetDict(PyObject * /*module*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_GetNameObject(PyObject * /*module*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" const char *PyModule_GetName(PyObject * /*module*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void *PyModule_GetState(PyObject * /*module*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyModuleDef *PyModule_GetDef(PyObject * /*module*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_GetFilenameObject(PyObject * /*module*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" const char *PyModule_GetFilename(PyObject * /*module*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_Create(PyModuleDef * /*def*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_Create2(PyModuleDef * /*def*/,
                                      int /*module_api_version*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyModuleDef_Init(PyModuleDef * /*def*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_FromDefAndSpec(PyModuleDef * /*def*/,
                                             PyObject * /*spec*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyModule_FromDefAndSpec2(PyModuleDef * /*def*/,
                                              PyObject * /*spec*/,
                                              int /*module_api_version*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyModule_ExecDef(PyObject * /*module*/, PyModuleDef * /*def*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyModule_SetDocString(PyObject * /*module*/,
                                     const char * /*docstring*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyModule_AddFunctions(PyObject * /*module*/,
                                     PyMethodDef * /*functions*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyModule_AddObject(PyObject * /*module*/, const char * /*name*/,
                                  PyObject * /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyModule_AddIntConstant(PyObject * /*module*/,
                                       const char * /*name*/,
                                       // NOLINTNEXTLINE
                                       long /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyModule_AddStringConstant(PyObject * /*module*/,
                                          const char * /*name*/,
                                          const char * /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyState_FindModule(PyModuleDef * /*def*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyState_AddModule(PyObject * /*module*/, PyModuleDef * /*def*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyState_RemoveModule(PyModuleDef * /*def*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyNumber_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Add(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Subtract(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Multiply(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_MatrixMultiply(PyObject * /*o1*/,
                                             PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_FloorDivide(PyObject * /*o1*/,
                                          PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_TrueDivide(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Remainder(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Divmod(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Power(PyObject * /*o1*/, PyObject * /*o2*/,
                                    PyObject * /*o3*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Negative(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Positive(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Absolute(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Invert(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Lshift(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Rshift(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_And(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Xor(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Or(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceAdd(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceSubtract(PyObject * /*o1*/,
                                              PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceMultiply(PyObject * /*o1*/,
                                              PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceMatrixMultiply(PyObject * /*o1*/,
                                                    PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceFloorDivide(PyObject * /*o1*/,
                                                 PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceTrueDivide(PyObject * /*o1*/,
                                                PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceRemainder(PyObject * /*o1*/,
                                               PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlacePower(PyObject * /*o1*/, PyObject * /*o2*/,
                                           PyObject * /*o3*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceLshift(PyObject * /*o1*/,
                                            PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceRshift(PyObject * /*o1*/,
                                            PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceAnd(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceXor(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_InPlaceOr(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Long(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Float(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_Index(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyNumber_ToBase(PyObject * /*n*/, int /*base*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyNumber_AsSsize_t(PyObject * /*o*/, PyObject * /*exc*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyIndex_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyObject_AsCharBuffer(PyObject * /*obj*/,
                                     const char ** /*buffer*/,
                                     Py_ssize_t * /*buffer_len*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_AsReadBuffer(PyObject * /*obj*/,
                                     const void ** /*buffer*/,
                                     Py_ssize_t * /*buffer_len*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_CheckReadBuffer(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyObject_AsWriteBuffer(PyObject * /*obj*/, void ** /*buffer*/,
                                      Py_ssize_t * /*buffer_len*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_Print(PyObject * /*o*/, FILE * /*fp*/, int /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_HasAttr(PyObject * /*o*/, PyObject * /*attr_name*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_HasAttrString(PyObject * /*o*/,
                                      const char * /*attr_name*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_GetAttr(PyObject * /*o*/,
                                      PyObject * /*attr_name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_GetAttrString(PyObject * /*o*/,
                                            const char * /*attr_name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_GenericGetAttr(PyObject * /*o*/,
                                             PyObject * /*name*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyObject_SetAttr(PyObject * /*o*/, PyObject * /*attr_name*/,
                                PyObject * /*v*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_SetAttrString(PyObject * /*o*/,
                                      const char * /*attr_name*/,
                                      PyObject * /*v*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_GenericSetAttr(PyObject * /*o*/, PyObject * /*name*/,
                                       PyObject * /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_DelAttr(PyObject * /*o*/, PyObject * /*attr_name*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_DelAttrString(PyObject * /*o*/,
                                      const char * /*attr_name*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_GenericGetDict(PyObject * /*o*/,
                                             void * /*context*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyObject_GenericSetDict(PyObject * /*o*/, void * /*context*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_RichCompare(PyObject * /*o1*/, PyObject * /*o2*/,
                                          int /*opid*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyObject_RichCompareBool(PyObject * /*o1*/, PyObject * /*o2*/,
                                        int /*opid*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_Repr(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_ASCII(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_Str(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_Bytes(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyObject_IsSubclass(PyObject * /*derived*/, PyObject * /*cls*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_IsInstance(PyObject * /*inst*/, PyObject * /*cls*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyCallable_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_Call(PyObject * /*callable*/, PyObject * /*args*/,
                                   PyObject * /*kwargs*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_CallObject(PyObject * /*callable*/,
                                         PyObject * /*args*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_CallFunction(PyObject * /*callable*/,
                                           const char * /*format*/, ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_CallMethod(PyObject * /*obj*/,
                                         const char * /*name*/,
                                         const char * /*format*/, ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_CallFunctionObjArgs(PyObject * /*callable*/,
                                                  ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_CallMethodObjArgs(PyObject * /*obj*/,
                                                PyObject * /*name*/, ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_hash_t PyObject_Hash(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_hash_t PyObject_HashNotImplemented(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyObject_IsTrue(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyObject_Not(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_Type(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyObject_TypeCheck(PyObject * /*o*/, PyTypeObject * /*type*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyObject_Length(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyObject_LengthHint(PyObject * /*o*/, Py_ssize_t /*d*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_GetItem(PyObject * /*o*/, PyObject * /*key*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyObject_SetItem(PyObject * /*o*/, PyObject * /*key*/,
                                PyObject * /*v*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyObject_DelItem(PyObject * /*o*/, PyObject * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_Dir(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyObject_GetIter(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" void Py_INCREF(PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" void Py_XINCREF(PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" void Py_DECREF(PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" void Py_XDECREF(PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" void Py_CLEAR(PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PyEval_GetBuiltins() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyEval_GetLocals() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyEval_GetGlobals() { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyFrameObject *PyEval_GetFrame() { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyFrame_GetLineNumber(PyFrameObject * /*frame*/) { return 0; }

// NOLINTNEXTLINE
extern "C" const char *PyEval_GetFuncName(PyObject * /*func*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" const char *PyEval_GetFuncDesc(PyObject * /*func*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PySequence_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PySequence_Size(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_Concat(PyObject * /*o1*/, PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_Repeat(PyObject * /*o*/, Py_ssize_t /*count*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_InPlaceConcat(PyObject * /*o1*/,
                                              PyObject * /*o2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_InPlaceRepeat(PyObject * /*o*/,
                                              Py_ssize_t /*count*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_GetItem(PyObject * /*o*/, Py_ssize_t /*i*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_GetSlice(PyObject * /*o*/, Py_ssize_t /*i1*/,
                                         Py_ssize_t /*i2*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PySequence_SetItem(PyObject * /*o*/, Py_ssize_t /*i*/,
                                  PyObject * /*v*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PySequence_DelItem(PyObject * /*o*/, Py_ssize_t /*i*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PySequence_SetSlice(PyObject * /*o*/, Py_ssize_t /*i1*/,
                                   Py_ssize_t /*i2*/, PyObject * /*v*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PySequence_DelSlice(PyObject * /*o*/, Py_ssize_t /*i1*/,
                                   Py_ssize_t /*i2*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PySequence_Count(PyObject * /*o*/, PyObject * /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PySequence_Contains(PyObject * /*o*/, PyObject * /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PySequence_Index(PyObject * /*o*/, PyObject * /*value*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_List(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_Tuple(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_Fast(PyObject * /*o*/, const char * /*m*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_Fast_GET_ITEM(PyObject * /*o*/,
                                              Py_ssize_t /*i*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject **PySequence_Fast_ITEMS(PyObject * /*o*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySequence_ITEM(PyObject * /*o*/, Py_ssize_t /*i*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PySequence_Fast_GET_SIZE(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PySet_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyFrozenSet_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyAnySet_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyAnySet_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyFrozenSet_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PySet_New(PyObject * /*iterable*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyFrozenSet_New(PyObject * /*iterable*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PySet_Size(PyObject * /*anyset*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PySet_GET_SIZE(PyObject * /*anyset*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PySet_Contains(PyObject * /*anyset*/, PyObject * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PySet_Add(PyObject * /*set*/, PyObject * /*key*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PySet_Discard(PyObject * /*set*/, PyObject * /*key*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySet_Pop(PyObject * /*set*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PySet_Clear(PyObject * /*set*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PySet_ClearFreeList() { return 0; }

// NOLINTNEXTLINE
extern "C" int PySlice_Check(PyObject * /*ob*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PySlice_New(PyObject * /*start*/, PyObject * /*stop*/,
                                 PyObject * /*step*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PySlice_GetIndices(PyObject * /*slice*/, Py_ssize_t /*length*/,
                                  Py_ssize_t * /*start*/, Py_ssize_t * /*stop*/,
                                  Py_ssize_t * /*step*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PySlice_GetIndicesEx(PyObject * /*slice*/, Py_ssize_t /*length*/,
                                    Py_ssize_t * /*start*/,
                                    Py_ssize_t * /*stop*/,
                                    Py_ssize_t * /*step*/,
                                    Py_ssize_t * /*slicelength*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PySlice_Unpack(PyObject * /*slice*/, Py_ssize_t * /*start*/,
                              Py_ssize_t * /*stop*/, Py_ssize_t * /*step*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PySlice_AdjustIndices(Py_ssize_t /*length*/,
                                            Py_ssize_t * /*start*/,
                                            Py_ssize_t * /*stop*/,
                                            Py_ssize_t /*step*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyOS_FSPath(PyObject * /*path*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int Py_FdIsInteractive(FILE * /*fp*/, const char * /*filename*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyOS_BeforeFork() {}

// NOLINTNEXTLINE
extern "C" void PyOS_AfterFork_Parent() {}

// NOLINTNEXTLINE
extern "C" void PyOS_AfterFork_Child() {}

// NOLINTNEXTLINE
extern "C" void PyOS_AfterFork() {}

// NOLINTNEXTLINE
extern "C" int PyOS_CheckStack() { return 0; }

// NOLINTNEXTLINE
extern "C" PyOS_sighandler_t PyOS_getsig(int /*i*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyOS_sighandler_t PyOS_setsig(int /*i*/, PyOS_sighandler_t /*h*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" wchar_t *Py_DecodeLocale(const char * /*arg*/, size_t * /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" char *Py_EncodeLocale(const wchar_t * /*text*/,
                                 size_t * /*error_pos*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PySys_GetObject(const char * /*name*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PySys_SetObject(const char * /*name*/, PyObject * /*v*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PySys_ResetWarnOptions() {}

// NOLINTNEXTLINE
extern "C" void PySys_AddWarnOption(const wchar_t * /*s*/) {}

// NOLINTNEXTLINE
extern "C" void PySys_AddWarnOptionUnicode(PyObject * /*unicode*/) {}

// NOLINTNEXTLINE
extern "C" void PySys_SetPath(const wchar_t * /*path*/) {}

// NOLINTNEXTLINE
extern "C" void PySys_WriteStdout(const char * /*format*/, ...) {}

// NOLINTNEXTLINE
extern "C" void PySys_WriteStderr(const char * /*format*/, ...) {}

// NOLINTNEXTLINE
extern "C" void PySys_FormatStdout(const char * /*format*/, ...) {}

// NOLINTNEXTLINE
extern "C" void PySys_FormatStderr(const char * /*format*/, ...) {}

// NOLINTNEXTLINE
extern "C" void PySys_AddXOption(const wchar_t * /*s*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PySys_GetXOptions() { return nullptr; }

// NOLINTNEXTLINE
extern "C" void Py_FatalError(const char * /*message*/) {}

// NOLINTNEXTLINE
extern "C" void Py_Exit(int /*status*/) {}

// NOLINTNEXTLINE
extern "C" int Py_AtExit(void (* /*func*/)()) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyTuple_Check(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyTuple_CheckExact(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyTuple_New(Py_ssize_t /*len*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyTuple_Pack(Py_ssize_t /*n*/, ...) { return nullptr; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyTuple_Size(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyTuple_GET_SIZE(PyObject * /*p*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyTuple_GetItem(PyObject * /*p*/, Py_ssize_t /*pos*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyTuple_GET_ITEM(PyObject * /*p*/, Py_ssize_t /*pos*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyTuple_GetSlice(PyObject * /*p*/, Py_ssize_t /*low*/,
                                      Py_ssize_t /*high*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyTuple_SetItem(PyObject * /*p*/, Py_ssize_t /*pos*/,
                               PyObject * /*o*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyTuple_SET_ITEM(PyObject * /*p*/, Py_ssize_t /*pos*/,
                                 PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" int _PyTuple_Resize(PyObject ** /*p*/, Py_ssize_t /*newsize*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyTuple_ClearFreeList() { return 0; }

extern "C" PyTypeObject *
// NOLINTNEXTLINE
PyStructSequence_NewType(PyStructSequence_Desc * /*desc*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyStructSequence_InitType(PyTypeObject * /*type*/,
                                          PyStructSequence_Desc * /*desc*/) {}

// NOLINTNEXTLINE
extern "C" int PyStructSequence_InitType2(PyTypeObject * /*type*/,
                                          PyStructSequence_Desc * /*desc*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyStructSequence_New(PyTypeObject * /*type*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyStructSequence_GetItem(PyObject * /*p*/,
                                              Py_ssize_t /*pos*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyStructSequence_GET_ITEM(PyObject * /*p*/,
                                               Py_ssize_t /*pos*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" void PyStructSequence_SetItem(PyObject * /*p*/, Py_ssize_t /*pos*/,
                                         PyObject * /*o*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PyStructSequence_SET_ITEM(PyObject * /*p*/,
                                               Py_ssize_t * /*pos*/,
                                               PyObject * /*o*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_Check(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyUnicode_CheckExact(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyUnicode_READY(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_GET_LENGTH(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_UCS1 *PyUnicode_1BYTE_DATA(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" int PyUnicode_KIND(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" void *PyUnicode_DATA(PyObject * /*o*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" void PyUnicode_WRITE(int /*kind*/, void * /*value*/,
                                Py_ssize_t /*index*/, Py_UCS4 /*value*/) {}

// NOLINTNEXTLINE
extern "C" Py_UCS4 PyUnicode_READ(int /*kind*/, void * /*value*/,
                                  Py_ssize_t /*index*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_UCS4 PyUnicode_READ_CHAR(PyObject * /*o*/, Py_ssize_t /*index*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_ClearFreeList() { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_GET_SIZE(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_GET_DATA_SIZE(PyObject * /*o*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_UNICODE *PyUnicode_AS_UNICODE(PyObject * /*o*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISSPACE(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISLOWER(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISUPPER(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISTITLE(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISLINEBREAK(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISDECIMAL(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISDIGIT(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISNUMERIC(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISALPHA(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISALNUM(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_ISPRINTABLE(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_UNICODE Py_UNICODE_TOLOWER(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_UNICODE Py_UNICODE_TOUPPER(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_UNICODE Py_UNICODE_TOTITLE(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_TODECIMAL(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int Py_UNICODE_TODIGIT(Py_UNICODE /*ch*/) { return 0; }

// NOLINTNEXTLINE
extern "C" double Py_UNICODE_TONUMERIC(Py_UNICODE /*ch*/) { return 0.0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_New(Py_ssize_t /*size*/, Py_UCS4 /*maxchar*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromKindAndData(int /*kind*/,
                                               const void * /*buffer*/,
                                               Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromStringAndSize(const char * /*u*/,
                                                 Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromString(const char * /*u*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromFormat(const char * /*format*/, ...) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromFormatV(const char * /*format*/,
                                           va_list /*vargs*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromEncodedObject(PyObject * /*obj*/,
                                                 const char * /*encoding*/,
                                                 const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_GetLength(PyObject * /*unicode*/) { return 0; }

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_CopyCharacters(PyObject * /*to*/,
                                               Py_ssize_t /*to_start*/,
                                               PyObject * /*from*/,
                                               Py_ssize_t /*from_start*/,
                                               Py_ssize_t /*how_many*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_Fill(PyObject * /*unicode*/,
                                     Py_ssize_t /*start*/,
                                     Py_ssize_t /*length*/,
                                     Py_UCS4 /*fill_char*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_WriteChar(PyObject * /*unicode*/, Py_ssize_t /*index*/,
                                   Py_UCS4 /*character*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_UCS4 PyUnicode_ReadChar(PyObject * /*unicode*/,
                                      Py_ssize_t /*index*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Substring(PyObject * /*str*/,
                                         Py_ssize_t /*start*/,
                                         Py_ssize_t /*end*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_UCS4 *PyUnicode_AsUCS4(PyObject * /*u*/, Py_UCS4 * /*buffer*/,
                                     Py_ssize_t /*buflen*/, int /*copy_null*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_UCS4 *PyUnicode_AsUCS4Copy(PyObject * /*u*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromUnicode(const Py_UNICODE * /*u*/,
                                           Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_UNICODE *PyUnicode_AsUnicode(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_TransformDecimalToASCII(Py_UNICODE * /*s*/,
                                                       Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_UNICODE *PyUnicode_AsUnicodeAndSize(PyObject * /*unicode*/,
                                                  Py_ssize_t * /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_UNICODE *PyUnicode_AsUnicodeCopy(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_GetSize(PyObject * /*unicode*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromObject(PyObject * /*obj*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeLocaleAndSize(const char * /*str*/,
                                                   Py_ssize_t /*len*/,
                                                   const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeLocale(const char * /*str*/,
                                            const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeLocale(PyObject * /*unicode*/,
                                            const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_FSConverter(PyObject * /*obj*/, void * /*result*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_FSDecoder(PyObject * /*obj*/, void * /*result*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeFSDefaultAndSize(const char * /*s*/,
                                                      Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeFSDefault(const char * /*s*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeFSDefault(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_FromWideChar(const wchar_t * /*w*/,
                                            Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_AsWideChar(PyUnicodeObject * /*unicode*/,
                                           wchar_t * /*w*/,
                                           Py_ssize_t /*size*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" wchar_t *PyUnicode_AsWideCharString(PyObject * /*unicode*/,
                                               Py_ssize_t * /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Decode(const char * /*s*/, Py_ssize_t /*size*/,
                                      const char * /*encoding*/,
                                      const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsEncodedString(PyObject * /*unicode*/,
                                               const char * /*encoding*/,
                                               const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Encode(const Py_UNICODE * /*s*/,
                                      Py_ssize_t /*size*/,
                                      const char * /*encoding*/,
                                      const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF8(const char * /*s*/,
                                          Py_ssize_t /*size*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF8Stateful(const char * /*s*/,
                                                  Py_ssize_t /*size*/,
                                                  const char * /*errors*/,
                                                  Py_ssize_t * /*consumed*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsUTF8String(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" const char *PyUnicode_AsUTF8AndSize(PyObject * /*unicode*/,
                                               Py_ssize_t * /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" const char *PyUnicode_AsUTF8(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeUTF8(const Py_UNICODE * /*s*/,
                                          Py_ssize_t /*size*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF32(const char * /*s*/,
                                           Py_ssize_t /*size*/,
                                           const char * /*errors*/,
                                           int * /*byteorder*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF32Stateful(const char * /*s*/,
                                                   Py_ssize_t /*size*/,
                                                   const char * /*errors*/,
                                                   int * /*byteorder*/,
                                                   Py_ssize_t * /*consumed*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsUTF32String(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeUTF32(const Py_UNICODE * /*s*/,
                                           Py_ssize_t /*size*/,
                                           const char * /*errors*/,
                                           int /*byteorder*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF16(const char * /*s*/,
                                           Py_ssize_t /*size*/,
                                           const char * /*errors*/,
                                           int * /*byteorder*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF16Stateful(const char * /*s*/,
                                                   Py_ssize_t /*size*/,
                                                   const char * /*errors*/,
                                                   int * /*byteorder*/,
                                                   Py_ssize_t * /*consumed*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsUTF16String(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeUTF16(const Py_UNICODE * /*s*/,
                                           Py_ssize_t /*size*/,
                                           const char * /*errors*/,
                                           int /*byteorder*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF7(const char * /*s*/,
                                          Py_ssize_t /*size*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUTF7Stateful(const char * /*s*/,
                                                  Py_ssize_t /*size*/,
                                                  const char * /*errors*/,
                                                  Py_ssize_t * /*consumed*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeUTF7(const Py_UNICODE * /*s*/,
                                          Py_ssize_t /*size*/,
                                          int /*base64SetO*/,
                                          int /*base64WhiteSpace*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeUnicodeEscape(const char * /*s*/,
                                                   Py_ssize_t /*size*/,
                                                   const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsUnicodeEscapeString(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeUnicodeEscape(const Py_UNICODE * /*s*/,
                                                   Py_ssize_t /*size*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeRawUnicodeEscape(const char * /*s*/,
                                                      Py_ssize_t /*size*/,
                                                      const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *
PyUnicode_AsRawUnicodeEscapeString(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeRawUnicodeEscape(const Py_UNICODE * /*s*/,
                                                      Py_ssize_t /*size*/,
                                                      const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeLatin1(const char * /*s*/,
                                            Py_ssize_t /*size*/,
                                            const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsLatin1String(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeLatin1(const Py_UNICODE * /*s*/,
                                            Py_ssize_t /*size*/,
                                            const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeASCII(const char * /*s*/,
                                           Py_ssize_t /*size*/,
                                           const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsASCIIString(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeASCII(const Py_UNICODE * /*s*/,
                                           Py_ssize_t /*size*/,
                                           const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeCharmap(const char * /*value*/,
                                             Py_ssize_t /*size*/,
                                             PyObject * /*mapping*/,
                                             const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsCharmapString(PyObject * /*unicode*/,
                                               PyObject * /*mapping*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeCharmap(const Py_UNICODE * /*s*/,
                                             Py_ssize_t /*size*/,
                                             PyObject * /*mapping*/,
                                             const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Translate(PyObject * /*unicode*/,
                                         PyObject * /*mapping*/,
                                         const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_TranslateCharmap(const Py_UNICODE * /*s*/,
                                                Py_ssize_t /*size*/,
                                                PyObject * /*mapping*/,
                                                const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeMBCS(const char * /*s*/,
                                          Py_ssize_t /*size*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_DecodeMBCSStateful(const char * /*s*/,
                                                  int /*size*/,
                                                  const char * /*errors*/,
                                                  int * /*consumed*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_AsMBCSString(PyObject * /*unicode*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeCodePage(int /*code_page*/,
                                              PyObject * /*unicode*/,
                                              const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_EncodeMBCS(const Py_UNICODE * /*s*/,
                                          Py_ssize_t /*size*/,
                                          const char * /*errors*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Concat(PyObject * /*left*/,
                                      PyObject * /*right*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Split(PyObject * /*s*/, PyObject * /*sep*/,
                                     Py_ssize_t /*maxsplit*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Splitlines(PyObject * /*s*/, int /*keepend*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Join(PyObject * /*separator*/,
                                    PyObject * /*seq*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_Tailmatch(PyObject * /*str*/,
                                          PyObject * /*substr*/,
                                          Py_ssize_t /*start*/,
                                          Py_ssize_t /*end*/,
                                          int /*direction*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_Find(PyObject * /*str*/, PyObject * /*substr*/,
                                     Py_ssize_t /*start*/, Py_ssize_t /*end*/,
                                     int /*direction*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_FindChar(PyObject * /*str*/, Py_UCS4 /*ch*/,
                                         Py_ssize_t /*start*/,
                                         Py_ssize_t /*end*/,
                                         int /*direction*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" Py_ssize_t PyUnicode_Count(PyObject * /*str*/, PyObject * /*substr*/,
                                      Py_ssize_t /*start*/,
                                      Py_ssize_t /*end*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Replace(PyObject * /*str*/,
                                       PyObject * /*substr*/,
                                       PyObject * /*replstr*/,
                                       Py_ssize_t /*maxcount*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_Compare(PyObject * /*left*/, PyObject * /*right*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_CompareWithASCIIString(PyObject * /*uni*/,
                                                const char * /*string*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_RichCompare(PyObject * /*left*/,
                                           PyObject * /*right*/, int /*op*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_Format(PyObject * /*format*/,
                                      PyObject * /*args*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyUnicode_Contains(PyObject * /*container*/,
                                  PyObject * /*element*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" void PyUnicode_InternInPlace(PyObject ** /*string*/) {}

// NOLINTNEXTLINE
extern "C" PyObject *PyUnicode_InternFromString(const char * /*v*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int Py_Main(int /*argc*/, wchar_t ** /*argv*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyRun_AnyFile(FILE * /*fp*/, const char * /*filename*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_AnyFileFlags(FILE * /*fp*/, const char * /*filename*/,
                                  PyCompilerFlags * /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_AnyFileEx(FILE * /*fp*/, const char * /*filename*/,
                               int /*closeit*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_AnyFileExFlags(FILE * /*fp*/, const char * /*filename*/,
                                    int /*closeit*/,
                                    PyCompilerFlags * /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_SimpleString(const char * /*command*/) { return 0; }

// NOLINTNEXTLINE
extern "C" int PyRun_SimpleStringFlags(const char * /*command*/,
                                       PyCompilerFlags * /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_SimpleFile(FILE * /*fp*/, const char * /*filename*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_SimpleFileEx(FILE * /*fp*/, const char * /*filename*/,
                                  int /*closeit*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_SimpleFileExFlags(FILE * /*fp*/, const char * /*filename*/,
                                       int /*closeit*/,
                                       PyCompilerFlags * /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_InteractiveOne(FILE * /*fp*/, const char * /*filename*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_InteractiveOneFlags(FILE * /*fp*/,
                                         const char * /*filename*/,
                                         PyCompilerFlags * /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_InteractiveLoop(FILE * /*fp*/, const char * /*filename*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" int PyRun_InteractiveLoopFlags(FILE * /*fp*/,
                                          const char * /*filename*/,
                                          PyCompilerFlags * /*flags*/) {
  return 0;
}

// NOLINTNEXTLINE
extern "C" struct _node *PyParser_SimpleParseString(const char * /*str*/,
                                                    int /*start*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" struct _node *PyParser_SimpleParseStringFlags(const char * /*str*/,
                                                         int /*start*/,
                                                         int /*flags*/) {
  return nullptr;
}

extern "C" struct _node *
// NOLINTNEXTLINE
PyParser_SimpleParseStringFlagsFilename(const char * /*str*/,
                                        const char * /*filename*/,
                                        int /*start*/, int /*flags*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" struct _node *PyParser_SimpleParseFile(FILE * /*fp*/,
                                                  const char * /*filename*/,
                                                  int /*start*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" struct _node *
PyParser_SimpleParseFileFlags(FILE * /*fp*/, const char * /*filename*/,
                              int /*start*/, int /*flags*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyRun_String(const char * /*str*/, int /*start*/,
                                  PyObject * /*globals*/,
                                  PyObject * /*locals*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyRun_StringFlags(const char * /*str*/, int /*start*/,
                                       PyObject * /*globals*/,
                                       PyObject * /*locals*/,
                                       PyCompilerFlags * /*flags*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyRun_File(FILE * /*fp*/, const char * /*filename*/,
                                int /*start*/, PyObject * /*globals*/,
                                PyObject * /*locals*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyRun_FileEx(FILE * /*fp*/, const char * /*filename*/,
                                  int /*start*/, PyObject * /*globals*/,
                                  PyObject * /*locals*/, int /*closeit*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyRun_FileFlags(FILE * /*fp*/, const char * /*filename*/,
                                     int /*start*/, PyObject * /*globals*/,
                                     PyObject * /*locals*/,
                                     PyCompilerFlags * /*flags*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyRun_FileExFlags(FILE * /*fp*/, const char * /*filename*/,
                                       int /*start*/, PyObject * /*globals*/,
                                       PyObject * /*locals*/, int /*closeit*/,
                                       PyCompilerFlags * /*flags*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *Py_CompileString(const char * /*str*/,
                                      const char * /*filename*/,
                                      int /*start*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *Py_CompileStringFlags(const char * /*str*/,
                                           const char * /*filename*/,
                                           int /*start*/,
                                           PyCompilerFlags * /*flags*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *Py_CompileStringObject(const char * /*str*/,
                                            PyObject * /*filename*/,
                                            int /*start*/,
                                            PyCompilerFlags * /*flags*/,
                                            int /*optimize*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *Py_CompileStringExFlags(const char * /*str*/,
                                             const char * /*filename*/,
                                             int /*start*/,
                                             PyCompilerFlags * /*flags*/,
                                             int /*optimize*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyEval_EvalCode(PyObject * /*co*/, PyObject * /*globals*/,
                                     PyObject * /*locals*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *
PyEval_EvalCodeEx(PyObject * /*co*/, PyObject * /*globals*/,
                  PyObject * /*locals*/, PyObject ** /*args*/, int /*argcount*/,
                  PyObject ** /*kws*/, int /*kwcount*/, PyObject ** /*defs*/,
                  int /*defcount*/, PyObject * /*kwdefs*/,
                  PyObject * /*closure*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyEval_EvalFrame(PyFrameObject * /*f*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyEval_EvalFrameEx(PyFrameObject * /*f*/,
                                        int /*throwflag*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" int PyEval_MergeCompilerFlags(PyCompilerFlags * /*cf*/) { return 0; }

// NOLINTNEXTLINE
extern "C" PyObject *PyWeakref_NewRef(PyObject * /*ob*/,
                                      PyObject * /*callback*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyWeakref_NewProxy(PyObject * /*ob*/,
                                        PyObject * /*callback*/) {
  return nullptr;
}

// NOLINTNEXTLINE
extern "C" PyObject *PyWeakref_GetObject(PyObject * /*ref*/) { return nullptr; }

// NOLINTNEXTLINE
extern "C" PyObject *PyWeakref_GET_OBJECT(PyObject * /*ref*/) {
  return nullptr;
}
