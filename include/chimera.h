//! generated file see tools/extract_doc.py

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#ifndef _PyObject_EXTRA_INIT
#define _PyObject_EXTRA_INIT
#endif
#pragma clang diagnostic pop

typedef int64_t PY_INT64_T;
typedef int64_t Py_ssize_t;
typedef int64_t Py_hash_t;
typedef wchar_t Py_UNICODE;

typedef void *PyGILState_STATE;

typedef void (*PyOS_sighandler_t)(int);

/* start structures.rst */
#define PyObject_HEAD PyObject ob_base;
#define PyObject_VAR_HEAD PyVarObject ob_base;
typedef struct PyObject PyObject;
struct PyObject {
#ifdef Py_TRACE_REFS
  PyObject *_ob_next;
  PyObject *_ob_prev;
#endif
  size_t ob_refcnt;
  PyObject *ob_type;
};
typedef struct PyVarObject PyVarObject;
struct PyVarObject {
  PyObject ob_base;
  size_t ob_size;
};
#define Py_TYPE(o) (((PyObject *)(o))->ob_type)
#define Py_REFCNT(o) (((PyObject *)(o))->ob_refcnt)
#define Py_SIZE(o) (((PyVarObject *)(o))->ob_size)
#ifdef Py_TRACE_REFS
#define PyObject_HEAD_INIT(type) _PyObject_EXTRA_INIT NULL, NULL, 1U, (type),
#else
#define PyObject_HEAD_INIT(type) _PyObject_EXTRA_INIT 1U, (type),
#endif
#define PyVarObject_HEAD_INIT(type, size) {PyObject_HEAD_INIT(type)}, (size),
typedef PyObject *(PyCFunction)(PyObject *, PyObject *);
typedef PyObject *(PyCFunctionWithKeywords)(PyObject *, PyObject *, PyObject *);
typedef struct PyMethodDef PyMethodDef;
struct PyMethodDef {
  const char *ml_name;
  PyCFunction ml_meth;
  int ml_flags;
  const char *ml_doc;
};
typedef struct PyMemberDef PyMemberDef;
struct PyMemberDef {
  const char *name;
  int type;
  Py_ssize_t offset;
  int flags;
  const char *doc;
};
/* end structures.rst */
/* start typeobj.rst */
typedef struct PyTypeObject PyTypeObject;
struct PyTypeObject {
  PyObject_VAR_HEAD const char *tp_name;
  Py_ssize_t tp_basicsize;
  Py_ssize_t tp_itemsize;
  void (*tp_dealloc)(PyObject *);
  void (*tp_print)();
  PyObject *(*tp_getattr)(PyObject *, char *);
  PyObject *(*tp_setattr)(PyObject *, char *, PyObject *);
  struct PyAsyncMethods *tp_as_async;
  PyObject *(*tp_repr)(PyObject *);
  struct PyNumberMethods *tp_as_number;
  struct PySequenceMethods *tp_as_sequence;
  struct PyMappingMethods *tp_as_mapping;
  Py_hash_t (*tp_hash)(PyObject *);
  PyObject *(*tp_call)(PyObject *, PyObject *, PyObject *);
  PyObject *(*tp_str)(PyObject *);
  PyObject *(*tp_getattro)(PyObject *, PyObject *);
  int (*tp_setattro)(PyObject *, PyObject *, PyObject *);
  struct PyBufferProcs *tp_as_buffer;
  unsigned long tp_flags;
  const char *tp_doc;
  int (*tp_traverse)(PyObject *, int (*visitproc)(PyObject *, void *), void *);
  int (*tp_clear)(PyObject *);
  PyObject *(*tp_richcompare)(PyObject *, PyObject *, int);
  Py_ssize_t tp_weaklistoffset;
  PyObject *(*tp_iter)(PyObject *);
  PyObject *(*tp_iternext)(PyObject *);
  struct PyMethodDef *tp_methods;
  struct PyMemberDef *tp_members;
  struct PyGetSetDef *tp_getset;
  PyTypeObject *tp_base;
  PyObject *tp_dict;
  PyObject *(*tp_descr_get)(PyObject *, PyObject *, PyObject *);
  int (*tp_descr_set)(PyObject *, PyObject *, PyObject *);
  Py_ssize_t tp_dictoffset;
  int (*tp_init)(PyObject *, PyObject *, PyObject *);
  PyObject *(*tp_alloc)(PyTypeObject *, Py_ssize_t);
  PyObject *(*tp_new)(PyTypeObject *, PyObject *, PyObject *);
  void (*tp_free)(PyObject *);
  int (*tp_is_gc)(PyObject *);
  PyObject *tp_bases;
  PyObject *tp_mro;
  void (*tp_finalize)(PyObject *);
  PyObject *tp_cache;
  PyObject *tp_subclasses;
  PyObject *tp_weaklist;
  Py_ssize_t tp_allocs;
  Py_ssize_t tp_frees;
  Py_ssize_t tp_maxalloc;
  PyTypeObject *tp_next;
};
enum {
  Py_TPFLAGS_HEAPTYPE = 1U << 0U,
  Py_TPFLAGS_BASETYPE = 1U << 1U,
  Py_TPFLAGS_READY = 1U << 2U,
  Py_TPFLAGS_READYING = 1U << 3U,
  Py_TPFLAGS_HAVE_GC = 1U << 4U,
  Py_TPFLAGS_DEFAULT = 1U << 5U,
  Py_TPFLAGS_LONG_SUBCLASS = 1U << 6U,
  Py_TPFLAGS_LIST_SUBCLASS = 1U << 7U,
  Py_TPFLAGS_TUPLE_SUBCLASS = 1U << 8U,
  Py_TPFLAGS_BYTES_SUBCLASS = 1U << 9U,
  Py_TPFLAGS_UNICODE_SUBCLASS = 1U << 10U,
  Py_TPFLAGS_DICT_SUBCLASS = 1U << 11U,
  Py_TPFLAGS_BASE_EXC_SUBCLASS = 1U << 12U,
  Py_TPFLAGS_TYPE_SUBCLASS = 1U << 13U,
  Py_TPFLAGS_HAVE_FINALIZE = 1U << 14U
};
typedef struct PyNumberMethods PyNumberMethods;
struct PyNumberMethods {
  PyObject *(*nb_add)(PyObject *, PyObject *);
  PyObject *(*nb_subtract)(PyObject *, PyObject *);
  PyObject *(*nb_multiply)(PyObject *, PyObject *);
  PyObject *(*nb_remainder)(PyObject *, PyObject *);
  PyObject *(*nb_divmod)(PyObject *, PyObject *);
  PyObject *(*nb_power)(PyObject *, PyObject *, PyObject *);
  PyObject *(*nb_negative)(PyObject *);
  PyObject *(*nb_positive)(PyObject *);
  PyObject *(*nb_absolute)(PyObject *);
  int (*nb_bool)(PyObject *);
  PyObject *(*nb_invert)(PyObject *);
  PyObject *(*nb_lshift)(PyObject *, PyObject *);
  PyObject *(*nb_rshift)(PyObject *, PyObject *);
  PyObject *(*nb_and)(PyObject *, PyObject *);
  PyObject *(*nb_xor)(PyObject *, PyObject *);
  PyObject *(*nb_or)(PyObject *, PyObject *);
  PyObject *(*nb_int)(PyObject *);
  void *nb_reserved;
  PyObject *(*nb_float)(PyObject *);
  PyObject *(*nb_inplace_add)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_subtract)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_multiply)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_remainder)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_power)(PyObject *, PyObject *, PyObject *);
  PyObject *(*nb_inplace_lshift)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_rshift)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_and)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_xor)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_or)(PyObject *, PyObject *);
  PyObject *(*nb_floor_divide)(PyObject *, PyObject *);
  PyObject *(*nb_true_divide)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_floor_divide)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_true_divide)(PyObject *, PyObject *);
  PyObject *(*nb_index)(PyObject *);
  PyObject *(*nb_matrix_multiply)(PyObject *, PyObject *);
  PyObject *(*nb_inplace_matrix_multiply)(PyObject *, PyObject *);
};
typedef struct PyMappingMethods PyMappingMethods;
struct PyMappingMethods {
  Py_ssize_t (*mp_length)(PyObject *);
  PyObject *(*mp_subscript)(PyObject *, PyObject *);
  int (*mp_ass_subscript)(PyObject *, PyObject *, PyObject *);
};
typedef struct PySequenceMethods PySequenceMethods;
struct PySequenceMethods {
  Py_ssize_t (*sq_length)(PyObject *);
  PyObject *(*sq_concat)(PyObject *, PyObject *);
  PyObject *(*sq_repeat)(PyObject *, Py_ssize_t);
  PyObject *(*sq_item)(PyObject *, Py_ssize_t);
  int (*sq_ass_item)(PyObject *, Py_ssize_t, PyObject *);
  int (*sq_contains)(PyObject *, PyObject *);
  PyObject *(*sq_inplace_concat)(PyObject *, PyObject *);
  PyObject *(*sq_inplace_repeat)(PyObject *, Py_ssize_t);
};
typedef struct PyBufferProcs PyBufferProcs;
struct PyBufferProcs {
  int (*bf_getbuffer)(PyObject *, struct Py_buffer *, int);
  void (*bf_releasebuffer)(PyObject *, Py_buffer *);
};
typedef struct PyAsyncMethods PyAsyncMethods;
struct PyAsyncMethods {
  PyObject *(*am_await)(PyObject *);
  PyObject *(*am_aiter)(PyObject *);
  PyObject *(*am_anext)(PyObject *);
};
/* end typeobj.rst */
/* start type.rst */
typedef struct PyTypeObject PyTypeObject;
extern PyObject PyType_Type;
extern int PyType_Check(PyObject *o);
extern int PyType_CheckExact(PyObject *o);
extern unsigned int PyType_ClearCache();
extern long PyType_GetFlags(PyTypeObject *type);
extern void PyType_Modified(PyTypeObject *type);
extern int PyType_HasFeature(PyTypeObject *o, int feature);
extern int PyType_IS_GC(PyTypeObject *o);
extern int PyType_IsSubtype(PyTypeObject *a, PyTypeObject *b);
extern PyObject *PyType_GenericAlloc(PyTypeObject *type, Py_ssize_t nitems);
extern PyObject *PyType_GenericNew(PyTypeObject *type, PyObject *args,
                                   PyObject *kwds);
extern int PyType_Ready(PyTypeObject *type);
extern PyObject *PyType_FromSpec(struct PyType_Spec *spec);
extern PyObject *PyType_FromSpecWithBases(struct PyType_Spec *spec,
                                          PyObject *bases);
extern void *PyType_GetSlot(PyTypeObject *type, int slot);
/* end type.rst */
/* start abstract.rst */
/* end abstract.rst */
extern PyTypeObject PyBaseObject_Type;
/* start allocation.rst */
extern PyObject *_PyObject_New(PyTypeObject *type);
extern PyVarObject *_PyObject_NewVar(PyTypeObject *type, Py_ssize_t size);
extern PyObject *PyObject_Init(PyObject *op, PyTypeObject *type);
extern PyVarObject *PyObject_InitVar(PyVarObject *op, PyTypeObject *type,
                                     Py_ssize_t size);
#define PyObject_New(TYPE, type) (TYPE *)_PyObject_New((PyTypeObject *)(type));
#define PyObject_NewVar(TYPE, type, size)                                      \
  (TYPE *)_PyObject_NewVar((PyTypeObject *)(type), (Py_ssize_t)(size));
extern void PyObject_Del(PyObject *op);
extern const PyObject _Py_NoneStruct;
/* end allocation.rst */
/* start apiabiversion.rst */
/* end apiabiversion.rst */
/* start arg.rst */
extern int PyArg_ParseTuple(PyObject *args, const char *format, ...);
extern int PyArg_VaParse(PyObject *args, const char *format, va_list vargs);
extern int PyArg_ParseTupleAndKeywords(PyObject *args, PyObject *kw,
                                       const char *format, char *keywords[],
                                       ...);
extern int PyArg_VaParseTupleAndKeywords(PyObject *args, PyObject *kw,
                                         const char *format, char *keywords[],
                                         va_list vargs);
extern int PyArg_ValidateKeywordArguments(PyObject *kw);
extern int PyArg_Parse(PyObject *args, const char *format, ...);
extern int PyArg_UnpackTuple(PyObject *args, const char *name, Py_ssize_t min,
                             Py_ssize_t max, ...);
extern PyObject *Py_BuildValue(const char *format, ...);
extern PyObject *Py_VaBuildValue(const char *format, va_list vargs);
/* end arg.rst */
/* start bool.rst */
extern int PyBool_Check(PyObject *o);
extern PyObject *Py_False;
extern PyObject *Py_True;
#define Py_RETURN_FALSE                                                        \
  Py_INCREF(Py_False);                                                         \
  return Py_False
#define Py_RETURN_TRUE                                                         \
  Py_INCREF(Py_True);                                                          \
  return Py_True
extern PyObject *PyBool_FromLong(long v);
/* end bool.rst */
/* start buffer.rst */
typedef struct Py_buffer Py_buffer;
struct Py_buffer {
  void *buf;
  void *obj;
  Py_ssize_t len;
  int readonly;
  Py_ssize_t itemsize;
  const char *format;
  int ndim;
  Py_ssize_t *shape;
  Py_ssize_t *strides;
  Py_ssize_t *suboffsets;
  void *internal;
};
#define PyBUF_WRITABLE
#define PyBUF_FORMAT
#define PyBUF_INDIRECT | yes | yes | if needed |
#define PyBUF_STRIDES | yes | yes | NULL |
#define PyBUF_ND | yes | NULL | NULL |
#define PyBUF_SIMPLE | NULL | NULL | NULL |
#define PyBUF_C_CONTIGUOUS | yes | yes | NULL | C |
#define PyBUF_F_CONTIGUOUS | yes | yes | NULL | F |
#define PyBUF_ANY_CONTIGUOUS | yes | yes | NULL | C or F |
#define PyBUF_FULL | yes | yes | if needed | U | 0 | yes |
#define PyBUF_FULL_RO | yes | yes | if needed | U | 1 or 0 | yes |
#define PyBUF_RECORDS | yes | yes | NULL | U | 0 | yes |
#define PyBUF_RECORDS_RO | yes | yes | NULL | U | 1 or 0 | yes |
#define PyBUF_STRIDED | yes | yes | NULL | U | 0 | NULL |
#define PyBUF_STRIDED_RO | yes | yes | NULL | U | 1 or 0 | NULL |
#define PyBUF_CONTIG | yes | NULL | NULL | C | 0 | NULL |
#define PyBUF_CONTIG_RO | yes | NULL | NULL | C | 1 or 0 | NULL |
extern int PyObject_CheckBuffer(PyObject *obj);
extern int PyObject_GetBuffer(PyObject *exporter, Py_buffer *view, int flags);
extern void PyBuffer_Release(Py_buffer *view);
extern Py_ssize_t PyBuffer_SizeFromFormat(const char *format);
extern int PyBuffer_IsContiguous(Py_buffer *view, char order);
extern void PyBuffer_FillContiguousStrides(int ndim, Py_ssize_t *shape,
                                           Py_ssize_t *strides,
                                           Py_ssize_t itemsize, char order);
extern int PyBuffer_FillInfo(Py_buffer *view, PyObject *exporter, void *buf,
                             Py_ssize_t len, int readonly, int flags);
/* end buffer.rst */
/* start bytearray.rst */
typedef struct PyByteArrayObject PyByteArrayObject;
struct PyByteArrayObject {
  PyObject_HEAD
};
extern PyTypeObject PyByteArray_Type;
extern int PyByteArray_Check(PyObject *o);
extern int PyByteArray_CheckExact(PyObject *o);
extern PyObject *PyByteArray_FromObject(PyObject *o);
extern PyObject *PyByteArray_FromStringAndSize(const char *string,
                                               Py_ssize_t len);
extern PyObject *PyByteArray_Concat(PyObject *a, PyObject *b);
extern Py_ssize_t PyByteArray_Size(PyObject *bytearray);
extern char *PyByteArray_AsString(PyObject *bytearray);
extern int PyByteArray_Resize(PyObject *bytearray, Py_ssize_t len);
extern char *PyByteArray_AS_STRING(PyObject *bytearray);
extern Py_ssize_t PyByteArray_GET_SIZE(PyObject *bytearray);
/* end bytearray.rst */
/* start bytes.rst */
typedef struct PyBytesObject PyBytesObject;
struct PyBytesObject {
  PyObject_HEAD
};
extern PyTypeObject PyBytes_Type;
extern int PyBytes_Check(PyObject *o);
extern int PyBytes_CheckExact(PyObject *o);
extern PyObject *PyBytes_FromString(const char *v);
extern PyObject *PyBytes_FromStringAndSize(const char *v, Py_ssize_t len);
extern PyObject *PyBytes_FromFormat(const char *format, ...);
extern PyObject *PyBytes_FromFormatV(const char *format, va_list vargs);
extern PyObject *PyBytes_FromObject(PyObject *o);
extern Py_ssize_t PyBytes_Size(PyObject *o);
extern Py_ssize_t PyBytes_GET_SIZE(PyObject *o);
extern char *PyBytes_AsString(PyObject *o);
extern char *PyBytes_AS_STRING(PyObject *string);
extern int PyBytes_AsStringAndSize(PyObject *obj, char **buffer,
                                   Py_ssize_t *length);
extern void PyBytes_Concat(PyObject **bytes, PyObject *newpart);
extern void PyBytes_ConcatAndDel(PyObject **bytes, PyObject *newpart);
extern int _PyBytes_Resize(PyObject **bytes, Py_ssize_t newsize);
/* end bytes.rst */
/* start capsule.rst */
typedef struct PyCapsule PyCapsule;
struct PyCapsule {
  PyObject_HEAD
};
typedef void (*PyCapsule_Destructor)(PyObject *);
extern int PyCapsule_CheckExact(PyObject *p);
extern PyObject *PyCapsule_New(void *pointer, const char *name,
                               PyCapsule_Destructor destructor);
extern void *PyCapsule_GetPointer(PyObject *capsule, const char *name);
extern PyCapsule_Destructor PyCapsule_GetDestructor(PyObject *capsule);
extern void *PyCapsule_GetContext(PyObject *capsule);
extern const char *PyCapsule_GetName(PyObject *capsule);
extern void *PyCapsule_Import(const char *name, int no_block);
extern int PyCapsule_IsValid(PyObject *capsule, const char *name);
extern int PyCapsule_SetContext(PyObject *capsule, void *context);
extern int PyCapsule_SetDestructor(PyObject *capsule,
                                   PyCapsule_Destructor destructor);
extern int PyCapsule_SetName(PyObject *capsule, const char *name);
extern int PyCapsule_SetPointer(PyObject *capsule, void *pointer);
/* end capsule.rst */
/* start cell.rst */
typedef struct PyCellObject PyCellObject;
struct PyCellObject {
  PyObject_HEAD
};
extern PyTypeObject PyCell_Type;
extern int PyCell_Check(PyObject *ob);
extern PyObject *PyCell_New(PyObject *ob);
extern PyObject *PyCell_Get(PyObject *cell);
extern PyObject *PyCell_GET(PyObject *cell);
extern int PyCell_Set(PyObject *cell, PyObject *value);
extern void PyCell_SET(PyObject *cell, PyObject *value);
/* end cell.rst */
/* start code.rst */
typedef struct PyCodeObject PyCodeObject;
struct PyCodeObject {
  PyObject_HEAD
};
extern PyTypeObject PyCode_Type;
extern int PyCode_Check(PyObject *co);
extern int PyCode_GetNumFree(PyCodeObject *co);
extern PyCodeObject *PyCode_New(int argcount, int kwonlyargcount, int nlocals,
                                int stacksize, int flags, PyObject *code,
                                PyObject *consts, PyObject *names,
                                PyObject *varnames, PyObject *freevars,
                                PyObject *cellvars, PyObject *filename,
                                PyObject *name, int firstlineno,
                                PyObject *lnotab);
extern PyCodeObject *PyCode_NewEmpty(const char *filename, const char *funcname,
                                     int firstlineno);
/* end code.rst */
/* start codec.rst */
extern int PyCodec_Register(PyObject *search_function);
extern int PyCodec_KnownEncoding(const char *encoding);
extern PyObject *PyCodec_Encode(PyObject *object, const char *encoding,
                                const char *errors);
extern PyObject *PyCodec_Decode(PyObject *object, const char *encoding,
                                const char *errors);
extern PyObject *PyCodec_Encoder(const char *encoding);
extern PyObject *PyCodec_Decoder(const char *encoding);
extern PyObject *PyCodec_IncrementalEncoder(const char *encoding,
                                            const char *errors);
extern PyObject *PyCodec_IncrementalDecoder(const char *encoding,
                                            const char *errors);
extern PyObject *PyCodec_StreamReader(const char *encoding, PyObject *stream,
                                      const char *errors);
extern PyObject *PyCodec_StreamWriter(const char *encoding, PyObject *stream,
                                      const char *errors);
extern int PyCodec_RegisterError(const char *name, PyObject *error);
extern PyObject *PyCodec_LookupError(const char *name);
extern PyObject *PyCodec_StrictErrors(PyObject *exc);
extern PyObject *PyCodec_IgnoreErrors(PyObject *exc);
extern PyObject *PyCodec_ReplaceErrors(PyObject *exc);
extern PyObject *PyCodec_XMLCharRefReplaceErrors(PyObject *exc);
extern PyObject *PyCodec_BackslashReplaceErrors(PyObject *exc);
extern PyObject *PyCodec_NameReplaceErrors(PyObject *exc);
/* end codec.rst */
/* start complex.rst */
typedef struct Py_complex Py_complex;
struct Py_complex {
  double real;
  double imag;
};
extern Py_complex _Py_c_sum(Py_complex left, Py_complex right);
extern Py_complex _Py_c_diff(Py_complex left, Py_complex right);
extern Py_complex _Py_c_neg(Py_complex complex);
extern Py_complex _Py_c_prod(Py_complex left, Py_complex right);
extern Py_complex _Py_c_quot(Py_complex dividend, Py_complex divisor);
extern Py_complex _Py_c_pow(Py_complex num, Py_complex exp);
typedef struct PyComplexObject PyComplexObject;
struct PyComplexObject {
  PyObject_HEAD
};
extern PyTypeObject PyComplex_Type;
extern int PyComplex_Check(PyObject *p);
extern int PyComplex_CheckExact(PyObject *p);
extern PyObject *PyComplex_FromCComplex(Py_complex v);
extern PyObject *PyComplex_FromDoubles(double real, double imag);
extern double PyComplex_RealAsDouble(PyObject *op);
extern double PyComplex_ImagAsDouble(PyObject *op);
extern Py_complex PyComplex_AsCComplex(PyObject *op);
/* end complex.rst */
/* start concrete.rst */
/* end concrete.rst */
/* start conversion.rst */
extern int PyOS_snprintf(char *str, size_t size, const char *format, ...);
extern int PyOS_vsnprintf(char *str, size_t size, const char *format,
                          va_list va);
extern double PyOS_string_to_double(const char *s, char **endptr,
                                    PyObject *overflow_exception);
extern char *PyOS_double_to_string(double val, char format_code, int precision,
                                   int flags, int *ptype);
extern int PyOS_stricmp(const char *s1, const char *s2);
extern int PyOS_strnicmp(const char *s1, const char *s2, Py_ssize_t size);
/* end conversion.rst */
/* start coro.rst */
typedef struct PyCoroObject PyCoroObject;
struct PyCoroObject {
  PyObject_HEAD
};
extern PyTypeObject PyCoro_Type;
extern int PyCoro_CheckExact(PyObject *ob);
extern PyObject *PyCoro_New(struct PyFrameObject *frame, PyObject *name,
                            PyObject *qualname);
/* end coro.rst */
/* start datetime.rst */
extern int PyDate_Check(PyObject *ob);
extern int PyDate_CheckExact(PyObject *ob);
extern int PyDateTime_Check(PyObject *ob);
extern int PyDateTime_CheckExact(PyObject *ob);
extern int PyTime_Check(PyObject *ob);
extern int PyTime_CheckExact(PyObject *ob);
extern int PyDelta_Check(PyObject *ob);
extern int PyDelta_CheckExact(PyObject *ob);
extern int PyTZInfo_Check(PyObject *ob);
extern int PyTZInfo_CheckExact(PyObject *ob);
extern PyObject *PyDate_FromDate(int year, int month, int day);
extern PyObject *PyDateTime_FromDateAndTime(int year, int month, int day,
                                            int hour, int minute, int second,
                                            int usecond);
extern PyObject *PyTime_FromTime(int hour, int minute, int second, int usecond);
extern PyObject *PyDelta_FromDSU(int days, int seconds, int useconds);
extern int PyDateTime_GET_YEAR(struct PyDateTime_Date *o);
extern int PyDateTime_GET_MONTH(PyDateTime_Date *o);
extern int PyDateTime_GET_DAY(PyDateTime_Date *o);
extern int PyDateTime_DATE_GET_HOUR(struct PyDateTime_DateTime *o);
extern int PyDateTime_DATE_GET_MINUTE(PyDateTime_DateTime *o);
extern int PyDateTime_DATE_GET_SECOND(PyDateTime_DateTime *o);
extern int PyDateTime_DATE_GET_MICROSECOND(PyDateTime_DateTime *o);
extern int PyDateTime_TIME_GET_HOUR(struct PyDateTime_Time *o);
extern int PyDateTime_TIME_GET_MINUTE(PyDateTime_Time *o);
extern int PyDateTime_TIME_GET_SECOND(PyDateTime_Time *o);
extern int PyDateTime_TIME_GET_MICROSECOND(PyDateTime_Time *o);
extern int PyDateTime_DELTA_GET_DAYS(struct PyDateTime_Delta *o);
extern int PyDateTime_DELTA_GET_SECONDS(PyDateTime_Delta *o);
extern int PyDateTime_DELTA_GET_MICROSECOND(PyDateTime_Delta *o);
extern PyObject *PyDateTime_FromTimestamp(PyObject *args);
extern PyObject *PyDate_FromTimestamp(PyObject *args);
/* end datetime.rst */
/* start descriptor.rst */
extern PyTypeObject PyProperty_Type;
extern PyObject *PyDescr_NewGetSet(PyTypeObject *type,
                                   struct PyGetSetDef *getset);
extern PyObject *PyDescr_NewMember(PyTypeObject *type,
                                   struct PyMemberDef *meth);
extern PyObject *PyDescr_NewMethod(PyTypeObject *type,
                                   struct PyMethodDef *meth);
extern PyObject *PyDescr_NewWrapper(PyTypeObject *type,
                                    struct wrapperbase *wrapper, void *wrapped);
extern PyObject *PyDescr_NewClassMethod(PyTypeObject *type,
                                        PyMethodDef *method);
extern int PyDescr_IsData(PyObject *descr);
extern PyObject *PyWrapper_New(PyObject *a, PyObject *b);
/* end descriptor.rst */
/* start dict.rst */
typedef struct PyDictObject PyDictObject;
struct PyDictObject {
  PyObject_HEAD
};
extern PyTypeObject PyDict_Type;
extern int PyDict_Check(PyObject *p);
extern int PyDict_CheckExact(PyObject *p);
extern PyObject *PyDict_New();
extern PyObject *PyDictProxy_New(PyObject *mapping);
extern void PyDict_Clear(PyObject *p);
extern int PyDict_Contains(PyObject *p, PyObject *key);
extern PyObject *PyDict_Copy(PyObject *p);
extern int PyDict_SetItem(PyObject *p, PyObject *key, PyObject *val);
extern int PyDict_SetItemString(PyObject *p, const char *key, PyObject *val);
extern int PyDict_DelItem(PyObject *p, PyObject *key);
extern int PyDict_DelItemString(PyObject *p, const char *key);
extern PyObject *PyDict_GetItem(PyObject *p, PyObject *key);
extern PyObject *PyDict_GetItemWithError(PyObject *p, PyObject *key);
extern PyObject *PyDict_GetItemString(PyObject *p, const char *key);
extern PyObject *PyDict_SetDefault(PyObject *p, PyObject *key, PyObject *d);
extern PyObject *PyDict_Items(PyObject *p);
extern PyObject *PyDict_Keys(PyObject *p);
extern PyObject *PyDict_Values(PyObject *p);
extern Py_ssize_t PyDict_Size(PyObject *p);
extern int PyDict_Next(PyObject *p, Py_ssize_t *ppos, PyObject **pkey,
                       PyObject **pvalue);
extern int PyDict_Merge(PyObject *a, PyObject *b, int override);
extern int PyDict_Update(PyObject *a, PyObject *b);
extern int PyDict_MergeFromSeq2(PyObject *a, PyObject *seq2, int override);
extern int PyDict_ClearFreeList();
/* end dict.rst */
/* start exceptions.rst */
extern void PyErr_Clear();
extern void PyErr_PrintEx(int set_sys_last_vars);
extern void PyErr_Print();
extern void PyErr_WriteUnraisable(PyObject *obj);
extern void PyErr_SetString(PyObject *type, const char *message);
extern void PyErr_SetObject(PyObject *type, PyObject *value);
extern PyObject *PyErr_Format(PyObject *exception, const char *format, ...);
extern PyObject *PyErr_FormatV(PyObject *exception, const char *format,
                               va_list vargs);
extern void PyErr_SetNone(PyObject *type);
extern int PyErr_BadArgument();
extern PyObject *PyErr_NoMemory();
extern PyObject *PyErr_SetFromErrno(PyObject *type);
extern PyObject *PyErr_SetFromErrnoWithFilenameObject(PyObject *type,
                                                      PyObject *filenameObject);
extern PyObject *
PyErr_SetFromErrnoWithFilenameObjects(PyObject *type, PyObject *filenameObject,
                                      PyObject *filenameObject2);
extern PyObject *PyErr_SetFromErrnoWithFilename(PyObject *type,
                                                const char *filename);
extern PyObject *PyErr_SetFromWindowsErr(int ierr);
extern PyObject *PyErr_SetExcFromWindowsErr(PyObject *type, int ierr);
extern PyObject *PyErr_SetFromWindowsErrWithFilename(int ierr,
                                                     const char *filename);
extern PyObject *
PyErr_SetExcFromWindowsErrWithFilenameObject(PyObject *type, int ierr,
                                             PyObject *filename);
extern PyObject *PyErr_SetExcFromWindowsErrWithFilenameObjects(
    PyObject *type, int ierr, PyObject *filename, PyObject *filename2);
extern PyObject *PyErr_SetExcFromWindowsErrWithFilename(PyObject *type,
                                                        int ierr,
                                                        const char *filename);
extern PyObject *PyErr_SetImportError(PyObject *msg, PyObject *name,
                                      PyObject *path);
extern void PyErr_SyntaxLocationObject(PyObject *filename, int lineno,
                                       int col_offset);
extern void PyErr_SyntaxLocationEx(const char *filename, int lineno,
                                   int col_offset);
extern void PyErr_SyntaxLocation(const char *filename, int lineno);
extern void PyErr_BadInternalCall();
extern int PyErr_WarnEx(PyObject *category, const char *message,
                        Py_ssize_t stack_level);
extern PyObject *PyErr_SetImportErrorSubclass(PyObject *msg, PyObject *name,
                                              PyObject *path);
extern int PyErr_WarnExplicitObject(PyObject *category, PyObject *message,
                                    PyObject *filename, int lineno,
                                    PyObject *module, PyObject *registry);
extern int PyErr_WarnExplicit(PyObject *category, const char *message,
                              const char *filename, int lineno,
                              const char *module, PyObject *registry);
extern int PyErr_WarnFormat(PyObject *category, Py_ssize_t stack_level,
                            const char *format, ...);
extern int PyErr_ResourceWarning(PyObject *source, Py_ssize_t stack_level,
                                 const char *format, ...);
extern PyObject *PyErr_Occurred();
extern int PyErr_ExceptionMatches(PyObject *exc);
extern int PyErr_GivenExceptionMatches(PyObject *given, PyObject *exc);
extern void PyErr_Fetch(PyObject **ptype, PyObject **pvalue,
                        PyObject **ptraceback);
extern void PyErr_Restore(PyObject *type, PyObject *value, PyObject *traceback);
extern void PyErr_NormalizeException(PyObject **exc, PyObject **val,
                                     PyObject **tb);
extern void PyErr_GetExcInfo(PyObject **ptype, PyObject **pvalue,
                             PyObject **ptraceback);
extern void PyErr_SetExcInfo(PyObject *type, PyObject *value,
                             PyObject *traceback);
extern int PyErr_CheckSignals();
extern void PyErr_SetInterrupt();
extern int PySignal_SetWakeupFd(int fd);
extern PyObject *PyErr_NewException(const char *name, PyObject *base,
                                    PyObject *dict);
extern PyObject *PyErr_NewExceptionWithDoc(const char *name, const char *doc,
                                           PyObject *base, PyObject *dict);
extern PyObject *PyException_GetTraceback(PyObject *ex);
extern int PyException_SetTraceback(PyObject *ex, PyObject *tb);
extern PyObject *PyException_GetContext(PyObject *ex);
extern void PyException_SetContext(PyObject *ex, PyObject *ctx);
extern PyObject *PyException_GetCause(PyObject *ex);
extern void PyException_SetCause(PyObject *ex, PyObject *cause);
extern PyObject *PyUnicodeDecodeError_Create(const char *encoding,
                                             const char *object,
                                             Py_ssize_t length,
                                             Py_ssize_t start, Py_ssize_t end,
                                             const char *reason);
extern PyObject *PyUnicodeEncodeError_Create(const char *encoding,
                                             const Py_UNICODE *object,
                                             Py_ssize_t length,
                                             Py_ssize_t start, Py_ssize_t end,
                                             const char *reason);
extern PyObject *PyUnicodeTranslateError_Create(const Py_UNICODE *object,
                                                Py_ssize_t length,
                                                Py_ssize_t start,
                                                Py_ssize_t end,
                                                const char *reason);
extern PyObject *PyUnicodeDecodeError_GetEncoding(PyObject *exc);
extern PyObject *PyUnicodeDecodeError_GetObject(PyObject *exc);
extern int PyUnicodeDecodeError_GetStart(PyObject *exc, Py_ssize_t *start);
extern int PyUnicodeDecodeError_SetStart(PyObject *exc, Py_ssize_t start);
extern int PyUnicodeDecodeError_GetEnd(PyObject *exc, Py_ssize_t *end);
extern int PyUnicodeDecodeError_SetEnd(PyObject *exc, Py_ssize_t end);
extern PyObject *PyUnicodeDecodeError_GetReason(PyObject *exc);
extern int PyUnicodeDecodeError_SetReason(PyObject *exc, const char *reason);
extern int Py_EnterRecursiveCall(const char *where);
extern void Py_LeaveRecursiveCall();
extern int Py_ReprEnter(PyObject *object);
extern void Py_ReprLeave(PyObject *object);
/* end exceptions.rst */
/* start file.rst */
extern PyObject *PyFile_FromFd(int fd, const char *name, const char *mode,
                               int buffering, const char *encoding,
                               const char *errors, const char *newline,
                               int closefd);
extern int PyObject_AsFileDescriptor(PyObject *p);
extern PyObject *PyFile_GetLine(PyObject *p, int n);
extern int PyFile_WriteObject(PyObject *obj, PyObject *p, int flags);
extern int PyFile_WriteString(const char *s, PyObject *p);
/* end file.rst */
/* start float.rst */
typedef struct PyFloatObject PyFloatObject;
struct PyFloatObject {
  PyObject_HEAD
};
extern PyTypeObject PyFloat_Type;
extern int PyFloat_Check(PyObject *p);
extern int PyFloat_CheckExact(PyObject *p);
extern PyObject *PyFloat_FromString(PyObject *str);
extern PyObject *PyFloat_FromDouble(double v);
extern double PyFloat_AsDouble(PyObject *pyfloat);
extern double PyFloat_AS_DOUBLE(PyObject *pyfloat);
extern PyObject *PyFloat_GetInfo();
extern double PyFloat_GetMax();
extern double PyFloat_GetMin();
extern int PyFloat_ClearFreeList();
/* end float.rst */
/* start function.rst */
typedef struct PyFunctionObject PyFunctionObject;
struct PyFunctionObject {
  PyObject_HEAD
};
extern PyTypeObject PyFunction_Type;
extern int PyFunction_Check(PyObject *o);
extern PyObject *PyFunction_New(PyObject *code, PyObject *globals);
extern PyObject *PyFunction_NewWithQualName(PyObject *code, PyObject *globals,
                                            PyObject *qualname);
extern PyObject *PyFunction_GetCode(PyObject *op);
extern PyObject *PyFunction_GetGlobals(PyObject *op);
extern PyObject *PyFunction_GetModule(PyObject *op);
extern PyObject *PyFunction_GetDefaults(PyObject *op);
extern int PyFunction_SetDefaults(PyObject *op, PyObject *defaults);
extern PyObject *PyFunction_GetClosure(PyObject *op);
extern int PyFunction_SetClosure(PyObject *op, PyObject *closure);
extern PyObject *PyFunction_GetAnnotations(PyObject *op);
extern int PyFunction_SetAnnotations(PyObject *op, PyObject *annotations);
/* end function.rst */
/* start gcsupport.rst */
#define PyObject_GC_New(TYPE, type)
#define PyObject_GC_NewVar(TYPE, type, size)
#define PyObject_GC_Resize(TYPE, op, newsize)
extern void PyObject_GC_Track(PyObject *op);
extern void _PyObject_GC_TRACK(PyObject *op);
extern void PyObject_GC_Del(void *op);
extern void PyObject_GC_UnTrack(void *op);
extern void _PyObject_GC_UNTRACK(PyObject *op);
extern void Py_VISIT(PyObject *o);
/* end gcsupport.rst */
/* start gen.rst */
typedef struct PyGenObject PyGenObject;
struct PyGenObject {
  PyObject_HEAD
};
extern PyTypeObject PyGen_Type;
extern int PyGen_Check(PyObject *ob);
extern int PyGen_CheckExact(PyObject *ob);
extern PyObject *PyGen_New(PyFrameObject *frame);
extern PyObject *PyGen_NewWithQualName(PyFrameObject *frame, PyObject *name,
                                       PyObject *qualname);
/* end gen.rst */
/* start import.rst */
extern PyObject *PyImport_ImportModule(const char *name);
extern PyObject *PyImport_ImportModuleNoBlock(const char *name);
extern PyObject *PyImport_ImportModuleEx(const char *name, PyObject *globals,
                                         PyObject *locals, PyObject *fromlist);
extern PyObject *PyImport_ImportModuleLevelObject(PyObject *name,
                                                  PyObject *globals,
                                                  PyObject *locals,
                                                  PyObject *fromlist,
                                                  int level);
extern PyObject *PyImport_ImportModuleLevel(const char *name, PyObject *globals,
                                            PyObject *locals,
                                            PyObject *fromlist, int level);
extern PyObject *PyImport_Import(PyObject *name);
extern PyObject *PyImport_ReloadModule(PyObject *m);
extern PyObject *PyImport_AddModuleObject(PyObject *name);
extern PyObject *PyImport_AddModule(const char *name);
extern PyObject *PyImport_ExecCodeModule(const char *name, PyObject *co);
extern PyObject *PyImport_ExecCodeModuleEx(const char *name, PyObject *co,
                                           const char *pathname);
extern PyObject *PyImport_ExecCodeModuleObject(PyObject *name, PyObject *co,
                                               PyObject *pathname,
                                               PyObject *cpathname);
extern PyObject *PyImport_ExecCodeModuleWithPathnames(const char *name,
                                                      PyObject *co,
                                                      const char *pathname,
                                                      const char *cpathname);
extern long PyImport_GetMagicNumber();
extern const char *PyImport_GetMagicTag();
extern PyObject *PyImport_GetModuleDict();
extern PyObject *PyImport_GetImporter(PyObject *path);
extern void _PyImport_Init();
extern void PyImport_Cleanup();
extern void _PyImport_Fini();
extern int PyImport_ImportFrozenModuleObject(PyObject *name);
extern int PyImport_ImportFrozenModule(const char *name);
struct _frozen {
  const char *name;
  const unsigned char *code;
  int size;
};
extern const struct _frozen *PyImport_FrozenModules;
extern int PyImport_AppendInittab(const char *name, PyObject *(*initfunc)());
struct _inittab {
  const char *name;
  PyObject *(*initfunc)();
};
extern int PyImport_ExtendInittab(struct _inittab *newtab);
/* end import.rst */
/* start index.rst */
/* end index.rst */
/* start init.rst */
extern void Py_Initialize();
extern void Py_InitializeEx(int initsigs);
extern int Py_IsInitialized();
extern int Py_FinalizeEx();
extern void Py_Finalize();
extern int Py_SetStandardStreamEncoding(const char *encoding,
                                        const char *errors);
extern void Py_SetProgramName(wchar_t *name);
extern wchar_t *Py_GetProgramName();
extern wchar_t *Py_GetPrefix();
extern wchar_t *Py_GetExecPrefix();
extern wchar_t *Py_GetProgramFullPath();
extern wchar_t *Py_GetPath();
extern void Py_SetPath(const wchar_t *path);
extern const char *Py_GetVersion();
extern const char *Py_GetPlatform();
extern const char *Py_GetCopyright();
extern const char *Py_GetCompiler();
extern const char *Py_GetBuildInfo();
extern void PySys_SetArgvEx(int argc, wchar_t **argv, int updatepath);
extern void PySys_SetArgv(int argc, wchar_t **argv);
extern void Py_SetPythonHome(wchar_t *home);
extern wchar_t *Py_GetPythonHome();
typedef struct PyInterpreterState PyInterpreterState;
struct PyInterpreterState {
  PyObject_HEAD
};
typedef struct PyThreadState PyThreadState;
struct PyThreadState {
  PyInterpreterState *interp;
};
extern void PyEval_InitThreads();
extern int PyEval_ThreadsInitialized();
extern PyThreadState *PyEval_SaveThread();
extern void PyEval_RestoreThread(PyThreadState *tstate);
extern PyThreadState *PyThreadState_Get();
extern PyThreadState *PyThreadState_Swap(PyThreadState *tstate);
extern void PyEval_ReInitThreads();
extern PyGILState_STATE PyGILState_Ensure();
extern void PyGILState_Release(PyGILState_STATE state);
extern PyThreadState *PyGILState_GetThisThreadState();
extern int PyGILState_Check();
#define Py_BEGIN_ALLOW_THREADS                                                 \
  {                                                                            \
    PyThreadState *_save;                                                      \
    Py_UNBLOCK_THREADS
#define Py_END_ALLOW_THREADS                                                   \
  Py_BLOCK_THREADS                                                             \
  }
#define Py_BLOCK_THREADS PyEval_RestoreThread(_save);
#define Py_UNBLOCK_THREADS _save = PyEval_SaveThread();
extern PyInterpreterState *PyInterpreterState_New();
extern void PyInterpreterState_Clear(PyInterpreterState *interp);
extern void PyInterpreterState_Delete(PyInterpreterState *interp);
extern PyThreadState *PyThreadState_New(PyInterpreterState *interp);
extern void PyThreadState_Clear(PyThreadState *tstate);
extern void PyThreadState_Delete(PyThreadState *tstate);
extern PY_INT64_T PyInterpreterState_GetID(PyInterpreterState *interp);
extern PyObject *PyThreadState_GetDict();
extern int PyThreadState_SetAsyncExc(unsigned long id, PyObject *exc);
extern void PyEval_AcquireThread(PyThreadState *tstate);
extern void PyEval_ReleaseThread(PyThreadState *tstate);
extern void PyEval_AcquireLock();
extern void PyEval_ReleaseLock();
extern PyThreadState *Py_NewInterpreter();
extern void Py_EndInterpreter(PyThreadState *tstate);
extern int Py_AddPendingCall(int (*func)(void *), void *arg);
typedef int (*Py_tracefunc)(PyObject *obj, PyFrameObject *frame, int what,
                            PyObject *arg);
enum {
  PyTrace_CALL,
  PyTrace_EXCEPTION,
  PyTrace_LINE,
  PyTrace_RETURN,
  PyTrace_C_CALL,
  PyTrace_C_EXCEPTION,
  PyTrace_C_RETURN
};
extern void PyEval_SetProfile(Py_tracefunc func, PyObject *obj);
extern void PyEval_SetTrace(Py_tracefunc func, PyObject *obj);
extern PyInterpreterState *PyInterpreterState_Head();
extern PyInterpreterState *PyInterpreterState_Next(PyInterpreterState *interp);
extern PyThreadState *PyInterpreterState_ThreadHead(PyInterpreterState *interp);
extern PyThreadState *PyThreadState_Next(PyThreadState *tstate);
/* end init.rst */
/* start intro.rst */
/* end intro.rst */
/* start iter.rst */
extern int PyIter_Check(PyObject *o);
extern PyObject *PyIter_Next(PyObject *o);
/* end iter.rst */
/* start iterator.rst */
extern PyTypeObject PySeqIter_Type;
extern int PySeqIter_Check(PyObject *op);
extern PyObject *PySeqIter_New(PyObject *seq);
extern PyTypeObject PyCallIter_Type;
extern int PyCallIter_Check(PyObject *op);
extern PyObject *PyCallIter_New(PyObject *callable, PyObject *sentinel);
/* end iterator.rst */
/* start list.rst */
typedef struct PyListObject PyListObject;
struct PyListObject {
  PyObject_HEAD
};
extern PyTypeObject PyList_Type;
extern int PyList_Check(PyObject *p);
extern int PyList_CheckExact(PyObject *p);
extern PyObject *PyList_New(Py_ssize_t len);
extern Py_ssize_t PyList_Size(PyObject *list);
extern Py_ssize_t PyList_GET_SIZE(PyObject *list);
extern PyObject *PyList_GetItem(PyObject *list, Py_ssize_t index);
extern PyObject *PyList_GET_ITEM(PyObject *list, Py_ssize_t i);
extern int PyList_SetItem(PyObject *list, Py_ssize_t index, PyObject *item);
extern void PyList_SET_ITEM(PyObject *list, Py_ssize_t i, PyObject *o);
extern int PyList_Insert(PyObject *list, Py_ssize_t index, PyObject *item);
extern int PyList_Append(PyObject *list, PyObject *item);
extern PyObject *PyList_GetSlice(PyObject *list, Py_ssize_t low,
                                 Py_ssize_t high);
extern int PyList_SetSlice(PyObject *list, Py_ssize_t low, Py_ssize_t high,
                           PyObject *itemlist);
extern int PyList_Sort(PyObject *list);
extern int PyList_Reverse(PyObject *list);
extern PyObject *PyList_AsTuple(PyObject *list);
extern int PyList_ClearFreeList();
/* end list.rst */
/* start long.rst */
typedef struct PyLongObject PyLongObject;
struct PyLongObject {
  PyObject_HEAD
};
extern PyTypeObject PyLong_Type;
extern int PyLong_Check(PyObject *p);
extern int PyLong_CheckExact(PyObject *p);
extern PyObject *PyLong_FromLong(long v);
extern PyObject *PyLong_FromUnsignedLong(unsigned long v);
extern PyObject *PyLong_FromSsize_t(Py_ssize_t v);
extern PyObject *PyLong_FromSize_t(size_t v);
extern PyObject *PyLong_FromLongLong(long long v);
extern PyObject *PyLong_FromUnsignedLongLong(unsigned long long v);
extern PyObject *PyLong_FromDouble(double v);
extern PyObject *PyLong_FromString(const char *str, char **pend, int base);
extern PyObject *PyLong_FromUnicode(Py_UNICODE *u, Py_ssize_t length, int base);
extern PyObject *PyLong_FromUnicodeObject(PyObject *u, int base);
extern PyObject *PyLong_FromVoidPtr(void *p);
extern long PyLong_AsLong(PyObject *obj);
extern long PyLong_AsLongAndOverflow(PyObject *obj, int *overflow);
extern long long PyLong_AsLongLong(PyObject *obj);
extern long long PyLong_AsLongLongAndOverflow(PyObject *obj, int *overflow);
extern Py_ssize_t PyLong_AsSsize_t(PyObject *pylong);
extern unsigned long PyLong_AsUnsignedLong(PyObject *pylong);
extern size_t PyLong_AsSize_t(PyObject *pylong);
extern unsigned long long PyLong_AsUnsignedLongLong(PyObject *pylong);
extern unsigned long PyLong_AsUnsignedLongMask(PyObject *obj);
extern unsigned long long PyLong_AsUnsignedLongLongMask(PyObject *obj);
extern double PyLong_AsDouble(PyObject *pylong);
extern void *PyLong_AsVoidPtr(PyObject *pylong);
/* end long.rst */
/* start mapping.rst */
extern int PyMapping_Check(PyObject *o);
extern Py_ssize_t PyMapping_Size(PyObject *o);
extern int PyMapping_DelItemString(PyObject *o, const char *key);
extern int PyMapping_DelItem(PyObject *o, PyObject *key);
extern int PyMapping_HasKeyString(PyObject *o, const char *key);
extern int PyMapping_HasKey(PyObject *o, PyObject *key);
extern PyObject *PyMapping_Keys(PyObject *o);
extern PyObject *PyMapping_Values(PyObject *o);
extern PyObject *PyMapping_Items(PyObject *o);
extern PyObject *PyMapping_GetItemString(PyObject *o, const char *key);
extern int PyMapping_SetItemString(PyObject *o, const char *key, PyObject *v);
/* end mapping.rst */
/* start marshal.rst */
extern void PyMarshal_WriteLongToFile(long value, FILE *file, int version);
extern void PyMarshal_WriteObjectToFile(PyObject *value, FILE *file,
                                        int version);
extern PyObject *PyMarshal_WriteObjectToString(PyObject *value, int version);
extern long PyMarshal_ReadLongFromFile(FILE *file);
extern int PyMarshal_ReadShortFromFile(FILE *file);
extern PyObject *PyMarshal_ReadObjectFromFile(FILE *file);
extern PyObject *PyMarshal_ReadLastObjectFromFile(FILE *file);
extern PyObject *PyMarshal_ReadObjectFromString(const char *data,
                                                Py_ssize_t len);
/* end marshal.rst */
/* start memory.rst */
extern void *PyMem_RawMalloc(size_t n);
extern void *PyMem_RawCalloc(size_t nelem, size_t elsize);
extern void *PyMem_RawRealloc(void *p, size_t n);
extern void PyMem_RawFree(void *p);
extern void *PyMem_Malloc(size_t n);
extern void *PyMem_Calloc(size_t nelem, size_t elsize);
extern void *PyMem_Realloc(void *p, size_t n);
extern void PyMem_Free(void *p);
#define PyMem_New(TYPE, n) (TYPE *)PyMem_Malloc((n) * sizeof(TYPE))
#define PyMem_Resize(p, TYPE, n)                                               \
  { (p) = (TYPE *)PyMem_Realloc((p), (n) * sizeof(TYPE)); }
#define PyMem_Del PyMem_Free
typedef struct PyMemAllocatorEx PyMemAllocatorEx;
struct PyMemAllocatorEx {
  void *ctx;
  void *(*malloc)(void *ctx, size_t size);
  void *(*calloc)(void *ctx, size_t nelem, size_t elsize);
  void *(*realloc)(void *ctx, void *ptr, size_t new_size);
  void (*free)(void *ctx, void *ptr);
};
enum PyMemAllocatorDomain {
  PYMEM_DOMAIN_RAW,
  PYMEM_DOMAIN_MEM,
  PYMEM_DOMAIN_OBJ,
};
extern void PyMem_GetAllocator(PyMemAllocatorDomain domain,
                               PyMemAllocatorEx *allocator);
extern void PyMem_SetAllocator(PyMemAllocatorDomain domain,
                               PyMemAllocatorEx *allocator);
extern void PyMem_SetupDebugHooks();
typedef struct PyObjectArenaAllocator PyObjectArenaAllocator;
struct PyObjectArenaAllocator {
  void *ctx;
  void *(*alloc)(void *ctx, size_t size);
  void (*free)(void *ctx, size_t size, void *ptr);
};
extern PyObjectArenaAllocator *
PyObject_GetArenaAllocator(PyObjectArenaAllocator *allocator);
extern PyObjectArenaAllocator *
PyObject_SetArenaAllocator(PyObjectArenaAllocator *allocator);
/* end memory.rst */
/* start memoryview.rst */
extern PyObject *PyMemoryView_FromObject(PyObject *obj);
extern PyObject *PyMemoryView_FromMemory(char *mem, Py_ssize_t size, int flags);
extern PyObject *PyMemoryView_FromBuffer(Py_buffer *view);
extern PyObject *PyMemoryView_GetContiguous(PyObject *obj, int buffertype,
                                            char order);
extern int PyMemoryView_Check(PyObject *obj);
extern Py_buffer *PyMemoryView_GET_BUFFER(PyObject *mview);
extern Py_buffer *PyMemoryView_GET_BASE(PyObject *mview);
/* end memoryview.rst */
/* start method.rst */
extern PyTypeObject PyInstanceMethod_Type;
extern int PyInstanceMethod_Check(PyObject *o);
extern PyObject *PyInstanceMethod_New(PyObject *func);
extern PyObject *PyInstanceMethod_Function(PyObject *im);
extern PyObject *PyInstanceMethod_GET_FUNCTION(PyObject *im);
extern PyTypeObject PyMethod_Type;
extern int PyMethod_Check(PyObject *o);
extern PyObject *PyMethod_New(PyObject *func, PyObject *self);
extern PyObject *PyMethod_Function(PyObject *meth);
extern PyObject *PyMethod_GET_FUNCTION(PyObject *meth);
extern PyObject *PyMethod_Self(PyObject *meth);
extern PyObject *PyMethod_GET_SELF(PyObject *meth);
extern int PyMethod_ClearFreeList();
/* end method.rst */
/* start module.rst */
extern PyTypeObject PyModule_Type;
extern int PyModule_Check(PyObject *p);
extern int PyModule_CheckExact(PyObject *p);
extern PyObject *PyModule_NewObject(PyObject *name);
extern PyObject *PyModule_New(const char *name);
extern PyObject *PyModule_GetDict(PyObject *module);
extern PyObject *PyModule_GetNameObject(PyObject *module);
extern const char *PyModule_GetName(PyObject *module);
extern void *PyModule_GetState(PyObject *module);
extern struct PyModuleDef *PyModule_GetDef(PyObject *module);
extern PyObject *PyModule_GetFilenameObject(PyObject *module);
extern const char *PyModule_GetFilename(PyObject *module);
typedef struct PyModuleDef PyModuleDef;
struct PyModuleDef {
  const char *m_name;
  const char *m_doc;
  Py_ssize_t m_size;
  PyMethodDef *m_methods;
  struct PyModuleDef_Slot *m_slots;
  int (*m_reload)(PyObject *);
  int (*m_traverse)(PyObject *, int (*visit)(PyObject *, void *), void *);
  int (*m_clear)(PyObject *);
};
typedef void (*m_free)(void *);
extern PyObject *PyModule_Create(PyModuleDef *def);
extern PyObject *PyModule_Create2(PyModuleDef *def, int module_api_version);
extern PyObject *PyModuleDef_Init(PyModuleDef *def);
typedef struct PyModuleDef_Slot PyModuleDef_Slot;
struct PyModuleDef_Slot {
  int slot;
  void *value;
};
enum { Py_mod_create, Py_mod_exec };
typedef PyObject *(create_module)(PyObject *spec, PyModuleDef *def);
typedef int(exec_module)(PyObject *module);
extern PyObject *PyModule_FromDefAndSpec(PyModuleDef *def, PyObject *spec);
extern PyObject *PyModule_FromDefAndSpec2(PyModuleDef *def, PyObject *spec,
                                          int module_api_version);
extern int PyModule_ExecDef(PyObject *module, PyModuleDef *def);
extern int PyModule_SetDocString(PyObject *module, const char *docstring);
extern int PyModule_AddFunctions(PyObject *module, PyMethodDef *functions);
extern int PyModule_AddObject(PyObject *module, const char *name,
                              PyObject *value);
extern int PyModule_AddIntConstant(PyObject *module, const char *name,
                                   long value);
extern int PyModule_AddStringConstant(PyObject *module, const char *name,
                                      const char *value);
#define PyModule_AddIntMacro(module, macro)
#define PyModule_AddStringMacro(module, macro)
extern PyObject *PyState_FindModule(PyModuleDef *def);
extern int PyState_AddModule(PyObject *module, PyModuleDef *def);
extern int PyState_RemoveModule(PyModuleDef *def);
/* end module.rst */
/* start none.rst */
extern PyObject *Py_None;
#define Py_RETURN_NONE                                                         \
  Py_INCREF(Py_None);                                                          \
  return Py_None
/* end none.rst */
/* start number.rst */
extern int PyNumber_Check(PyObject *o);
extern PyObject *PyNumber_Add(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Subtract(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Multiply(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_MatrixMultiply(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_FloorDivide(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_TrueDivide(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Remainder(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Divmod(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Power(PyObject *o1, PyObject *o2, PyObject *o3);
extern PyObject *PyNumber_Negative(PyObject *o);
extern PyObject *PyNumber_Positive(PyObject *o);
extern PyObject *PyNumber_Absolute(PyObject *o);
extern PyObject *PyNumber_Invert(PyObject *o);
extern PyObject *PyNumber_Lshift(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Rshift(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_And(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Xor(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Or(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceAdd(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceSubtract(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceMultiply(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceMatrixMultiply(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceFloorDivide(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceTrueDivide(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceRemainder(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlacePower(PyObject *o1, PyObject *o2,
                                       PyObject *o3);
extern PyObject *PyNumber_InPlaceLshift(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceRshift(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceAnd(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceXor(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_InPlaceOr(PyObject *o1, PyObject *o2);
extern PyObject *PyNumber_Long(PyObject *o);
extern PyObject *PyNumber_Float(PyObject *o);
extern PyObject *PyNumber_Index(PyObject *o);
extern PyObject *PyNumber_ToBase(PyObject *n, int base);
extern Py_ssize_t PyNumber_AsSsize_t(PyObject *o, PyObject *exc);
extern int PyIndex_Check(PyObject *o);
/* end number.rst */
/* start objbuffer.rst */
extern int PyObject_AsCharBuffer(PyObject *obj, const char **buffer,
                                 Py_ssize_t *buffer_len);
extern int PyObject_AsReadBuffer(PyObject *obj, const void **buffer,
                                 Py_ssize_t *buffer_len);
extern int PyObject_CheckReadBuffer(PyObject *o);
extern int PyObject_AsWriteBuffer(PyObject *obj, void **buffer,
                                  Py_ssize_t *buffer_len);
/* end objbuffer.rst */
/* start object.rst */
extern PyObject *Py_NotImplemented;
#define Py_RETURN_NOTIMPLEMENTED                                               \
  Py_INCREF(Py_NotImplemented);                                                \
  return Py_NotImplemented
extern int PyObject_Print(PyObject *o, FILE *fp, int flags);
extern int PyObject_HasAttr(PyObject *o, PyObject *attr_name);
extern int PyObject_HasAttrString(PyObject *o, const char *attr_name);
extern PyObject *PyObject_GetAttr(PyObject *o, PyObject *attr_name);
extern PyObject *PyObject_GetAttrString(PyObject *o, const char *attr_name);
extern PyObject *PyObject_GenericGetAttr(PyObject *o, PyObject *name);
extern int PyObject_SetAttr(PyObject *o, PyObject *attr_name, PyObject *v);
extern int PyObject_SetAttrString(PyObject *o, const char *attr_name,
                                  PyObject *v);
extern int PyObject_GenericSetAttr(PyObject *o, PyObject *name,
                                   PyObject *value);
extern int PyObject_DelAttr(PyObject *o, PyObject *attr_name);
extern int PyObject_DelAttrString(PyObject *o, const char *attr_name);
extern PyObject *PyObject_GenericGetDict(PyObject *o, void *context);
extern int PyObject_GenericSetDict(PyObject *o, void *context);
extern PyObject *PyObject_RichCompare(PyObject *o1, PyObject *o2, int opid);
extern int PyObject_RichCompareBool(PyObject *o1, PyObject *o2, int opid);
extern PyObject *PyObject_Repr(PyObject *o);
extern PyObject *PyObject_ASCII(PyObject *o);
extern PyObject *PyObject_Str(PyObject *o);
extern PyObject *PyObject_Bytes(PyObject *o);
extern int PyObject_IsSubclass(PyObject *derived, PyObject *cls);
extern int PyObject_IsInstance(PyObject *inst, PyObject *cls);
extern int PyCallable_Check(PyObject *o);
extern PyObject *PyObject_Call(PyObject *callable, PyObject *args,
                               PyObject *kwargs);
extern PyObject *PyObject_CallObject(PyObject *callable, PyObject *args);
extern PyObject *PyObject_CallFunction(PyObject *callable, const char *format,
                                       ...);
extern PyObject *PyObject_CallMethod(PyObject *obj, const char *name,
                                     const char *format, ...);
extern PyObject *PyObject_CallFunctionObjArgs(PyObject *callable, ...);
extern PyObject *PyObject_CallMethodObjArgs(PyObject *obj, PyObject *name, ...);
extern Py_hash_t PyObject_Hash(PyObject *o);
extern Py_hash_t PyObject_HashNotImplemented(PyObject *o);
extern int PyObject_IsTrue(PyObject *o);
extern int PyObject_Not(PyObject *o);
extern PyObject *PyObject_Type(PyObject *o);
extern int PyObject_TypeCheck(PyObject *o, PyTypeObject *type);
extern Py_ssize_t PyObject_Length(PyObject *o);
extern Py_ssize_t PyObject_LengthHint(PyObject *o, Py_ssize_t d);
extern PyObject *PyObject_GetItem(PyObject *o, PyObject *key);
extern int PyObject_SetItem(PyObject *o, PyObject *key, PyObject *v);
extern int PyObject_DelItem(PyObject *o, PyObject *key);
extern PyObject *PyObject_Dir(PyObject *o);
extern PyObject *PyObject_GetIter(PyObject *o);
/* end object.rst */
/* start objimpl.rst */
/* end objimpl.rst */
/* start refcounting.rst */
extern void Py_INCREF(PyObject *o);
extern void Py_XINCREF(PyObject *o);
extern void Py_DECREF(PyObject *o);
extern void Py_XDECREF(PyObject *o);
extern void Py_CLEAR(PyObject *o);
/* end refcounting.rst */
/* start reflection.rst */
extern PyObject *PyEval_GetBuiltins();
extern PyObject *PyEval_GetLocals();
extern PyObject *PyEval_GetGlobals();
extern PyFrameObject *PyEval_GetFrame();
extern int PyFrame_GetLineNumber(PyFrameObject *frame);
extern const char *PyEval_GetFuncName(PyObject *func);
extern const char *PyEval_GetFuncDesc(PyObject *func);
/* end reflection.rst */
/* start sequence.rst */
extern int PySequence_Check(PyObject *o);
extern Py_ssize_t PySequence_Size(PyObject *o);
extern PyObject *PySequence_Concat(PyObject *o1, PyObject *o2);
extern PyObject *PySequence_Repeat(PyObject *o, Py_ssize_t count);
extern PyObject *PySequence_InPlaceConcat(PyObject *o1, PyObject *o2);
extern PyObject *PySequence_InPlaceRepeat(PyObject *o, Py_ssize_t count);
extern PyObject *PySequence_GetItem(PyObject *o, Py_ssize_t i);
extern PyObject *PySequence_GetSlice(PyObject *o, Py_ssize_t i1, Py_ssize_t i2);
extern int PySequence_SetItem(PyObject *o, Py_ssize_t i, PyObject *v);
extern int PySequence_DelItem(PyObject *o, Py_ssize_t i);
extern int PySequence_SetSlice(PyObject *o, Py_ssize_t i1, Py_ssize_t i2,
                               PyObject *v);
extern int PySequence_DelSlice(PyObject *o, Py_ssize_t i1, Py_ssize_t i2);
extern Py_ssize_t PySequence_Count(PyObject *o, PyObject *value);
extern int PySequence_Contains(PyObject *o, PyObject *value);
extern Py_ssize_t PySequence_Index(PyObject *o, PyObject *value);
extern PyObject *PySequence_List(PyObject *o);
extern PyObject *PySequence_Tuple(PyObject *o);
extern PyObject *PySequence_Fast(PyObject *o, const char *m);
extern PyObject *PySequence_Fast_GET_ITEM(PyObject *o, Py_ssize_t i);
extern PyObject **PySequence_Fast_ITEMS(PyObject *o);
extern PyObject *PySequence_ITEM(PyObject *o, Py_ssize_t i);
extern Py_ssize_t PySequence_Fast_GET_SIZE(PyObject *o);
/* end sequence.rst */
/* start set.rst */
typedef struct PySetObject PySetObject;
struct PySetObject {
  PyObject_HEAD
};
extern PyTypeObject PySet_Type;
extern PyTypeObject PyFrozenSet_Type;
extern int PySet_Check(PyObject *p);
extern int PyFrozenSet_Check(PyObject *p);
extern int PyAnySet_Check(PyObject *p);
extern int PyAnySet_CheckExact(PyObject *p);
extern int PyFrozenSet_CheckExact(PyObject *p);
extern PyObject *PySet_New(PyObject *iterable);
extern PyObject *PyFrozenSet_New(PyObject *iterable);
extern Py_ssize_t PySet_Size(PyObject *anyset);
extern Py_ssize_t PySet_GET_SIZE(PyObject *anyset);
extern int PySet_Contains(PyObject *anyset, PyObject *key);
extern int PySet_Add(PyObject *set, PyObject *key);
extern int PySet_Discard(PyObject *set, PyObject *key);
extern PyObject *PySet_Pop(PyObject *set);
extern int PySet_Clear(PyObject *set);
extern int PySet_ClearFreeList();
/* end set.rst */
/* start slice.rst */
extern PyTypeObject PySlice_Type;
extern int PySlice_Check(PyObject *ob);
extern PyObject *PySlice_New(PyObject *start, PyObject *stop, PyObject *step);
extern int PySlice_GetIndices(PyObject *slice, Py_ssize_t length,
                              Py_ssize_t *start, Py_ssize_t *stop,
                              Py_ssize_t *step);
extern int PySlice_GetIndicesEx(PyObject *slice, Py_ssize_t length,
                                Py_ssize_t *start, Py_ssize_t *stop,
                                Py_ssize_t *step, Py_ssize_t *slicelength);
extern int PySlice_Unpack(PyObject *slice, Py_ssize_t *start, Py_ssize_t *stop,
                          Py_ssize_t *step);
extern Py_ssize_t PySlice_AdjustIndices(Py_ssize_t length, Py_ssize_t *start,
                                        Py_ssize_t *stop, Py_ssize_t step);
extern PyObject *Py_Ellipsis;
/* end slice.rst */
/* start stable.rst */
/* end stable.rst */
/* start sys.rst */
extern PyObject *PyOS_FSPath(PyObject *path);
extern int Py_FdIsInteractive(FILE *fp, const char *filename);
extern void PyOS_BeforeFork();
extern void PyOS_AfterFork_Parent();
extern void PyOS_AfterFork_Child();
extern void PyOS_AfterFork();
extern int PyOS_CheckStack();
extern PyOS_sighandler_t PyOS_getsig(int i);
extern PyOS_sighandler_t PyOS_setsig(int i, PyOS_sighandler_t h);
extern wchar_t *Py_DecodeLocale(const char *arg, size_t *size);
extern char *Py_EncodeLocale(const wchar_t *text, size_t *error_pos);
extern PyObject *PySys_GetObject(const char *name);
extern int PySys_SetObject(const char *name, PyObject *v);
extern void PySys_ResetWarnOptions();
extern void PySys_AddWarnOption(const wchar_t *s);
extern void PySys_AddWarnOptionUnicode(PyObject *unicode);
extern void PySys_SetPath(const wchar_t *path);
extern void PySys_WriteStdout(const char *format, ...);
extern void PySys_WriteStderr(const char *format, ...);
extern void PySys_FormatStdout(const char *format, ...);
extern void PySys_FormatStderr(const char *format, ...);
extern void PySys_AddXOption(const wchar_t *s);
extern PyObject *PySys_GetXOptions();
extern void Py_FatalError(const char *message);
extern void Py_Exit(int status);
extern int Py_AtExit(void (*func)());
/* end sys.rst */
/* start tuple.rst */
typedef struct PyTupleObject PyTupleObject;
struct PyTupleObject {
  PyObject_HEAD
};
extern PyTypeObject PyTuple_Type;
extern int PyTuple_Check(PyObject *p);
extern int PyTuple_CheckExact(PyObject *p);
extern PyObject *PyTuple_New(Py_ssize_t len);
extern PyObject *PyTuple_Pack(Py_ssize_t n, ...);
extern Py_ssize_t PyTuple_Size(PyObject *p);
extern Py_ssize_t PyTuple_GET_SIZE(PyObject *p);
extern PyObject *PyTuple_GetItem(PyObject *p, Py_ssize_t pos);
extern PyObject *PyTuple_GET_ITEM(PyObject *p, Py_ssize_t pos);
extern PyObject *PyTuple_GetSlice(PyObject *p, Py_ssize_t low, Py_ssize_t high);
extern int PyTuple_SetItem(PyObject *p, Py_ssize_t pos, PyObject *o);
extern void PyTuple_SET_ITEM(PyObject *p, Py_ssize_t pos, PyObject *o);
extern int _PyTuple_Resize(PyObject **p, Py_ssize_t newsize);
extern int PyTuple_ClearFreeList();
extern PyTypeObject *
PyStructSequence_NewType(struct PyStructSequence_Desc *desc);
extern void PyStructSequence_InitType(PyTypeObject *type,
                                      PyStructSequence_Desc *desc);
extern int PyStructSequence_InitType2(PyTypeObject *type,
                                      PyStructSequence_Desc *desc);
typedef struct PyStructSequence_Desc PyStructSequence_Desc;
struct PyStructSequence_Desc {
  const char *name;
  const char *doc;
  struct PyStructSequence_Field *fields;
  int n_in_sequence;
};
typedef struct PyStructSequence_Field PyStructSequence_Field;
struct PyStructSequence_Field {
  const char *name;
  const char *doc;
};
extern char *PyStructSequence_UnnamedField;
extern PyObject *PyStructSequence_New(PyTypeObject *type);
extern PyObject *PyStructSequence_GetItem(PyObject *p, Py_ssize_t pos);
extern PyObject *PyStructSequence_GET_ITEM(PyObject *p, Py_ssize_t pos);
extern void PyStructSequence_SetItem(PyObject *p, Py_ssize_t pos, PyObject *o);
extern PyObject *PyStructSequence_SET_ITEM(PyObject *p, Py_ssize_t *pos,
                                           PyObject *o);
/* end tuple.rst */
/* start unicode.rst */
typedef uint32_t Py_UCS4;
typedef uint16_t Py_UCS2;
typedef uint8_t Py_UCS1;
typedef wchar_t Py_UNICODE;
typedef struct PyASCIIObject PyASCIIObject;
struct PyASCIIObject {
  PyObject_HEAD
};
typedef struct PyCompactUnicodeObject PyCompactUnicodeObject;
struct PyCompactUnicodeObject {
  PyObject_HEAD
};
typedef struct PyUnicodeObject PyUnicodeObject;
struct PyUnicodeObject {
  PyObject_HEAD
};
extern PyTypeObject PyUnicode_Type;
extern int PyUnicode_Check(PyObject *o);
extern int PyUnicode_CheckExact(PyObject *o);
extern int PyUnicode_READY(PyObject *o);
extern Py_ssize_t PyUnicode_GET_LENGTH(PyObject *o);
extern Py_UCS1 *PyUnicode_1BYTE_DATA(PyObject *o);
#define PyUnicode_WCHAR_KIND
extern int PyUnicode_KIND(PyObject *o);
extern void *PyUnicode_DATA(PyObject *o);
extern void PyUnicode_WRITE(int kind, void *data, Py_ssize_t index,
                            Py_UCS4 value);
extern Py_UCS4 PyUnicode_READ(int kind, void *data, Py_ssize_t index);
extern Py_UCS4 PyUnicode_READ_CHAR(PyObject *o, Py_ssize_t index);
#define PyUnicode_MAX_CHAR_VALUE(o) 0x10FFFF
extern int PyUnicode_ClearFreeList();
extern Py_ssize_t PyUnicode_GET_SIZE(PyObject *o);
extern Py_ssize_t PyUnicode_GET_DATA_SIZE(PyObject *o);
extern Py_UNICODE *PyUnicode_AS_UNICODE(PyObject *o);
extern int Py_UNICODE_ISSPACE(Py_UNICODE ch);
extern int Py_UNICODE_ISLOWER(Py_UNICODE ch);
extern int Py_UNICODE_ISUPPER(Py_UNICODE ch);
extern int Py_UNICODE_ISTITLE(Py_UNICODE ch);
extern int Py_UNICODE_ISLINEBREAK(Py_UNICODE ch);
extern int Py_UNICODE_ISDECIMAL(Py_UNICODE ch);
extern int Py_UNICODE_ISDIGIT(Py_UNICODE ch);
extern int Py_UNICODE_ISNUMERIC(Py_UNICODE ch);
extern int Py_UNICODE_ISALPHA(Py_UNICODE ch);
extern int Py_UNICODE_ISALNUM(Py_UNICODE ch);
extern int Py_UNICODE_ISPRINTABLE(Py_UNICODE ch);
extern Py_UNICODE Py_UNICODE_TOLOWER(Py_UNICODE ch);
extern Py_UNICODE Py_UNICODE_TOUPPER(Py_UNICODE ch);
extern Py_UNICODE Py_UNICODE_TOTITLE(Py_UNICODE ch);
extern int Py_UNICODE_TODECIMAL(Py_UNICODE ch);
extern int Py_UNICODE_TODIGIT(Py_UNICODE ch);
extern double Py_UNICODE_TONUMERIC(Py_UNICODE ch);
#define Py_UNICODE_IS_SURROGATE(ch) (0xD800 <= (ch) <= 0xDFFF)
#define Py_UNICODE_IS_HIGH_SURROGATE(ch) (0xD800 <= (ch) <= 0xDBFF)
#define Py_UNICODE_IS_LOW_SURROGATE(ch) (0xDC00 <= (ch) <= 0xDFFF)
#define Py_UNICODE_JOIN_SURROGATES(high, low)
extern PyObject *PyUnicode_New(Py_ssize_t size, Py_UCS4 maxchar);
extern PyObject *PyUnicode_FromKindAndData(int kind, const void *buffer,
                                           Py_ssize_t size);
extern PyObject *PyUnicode_FromStringAndSize(const char *u, Py_ssize_t size);
extern PyObject *PyUnicode_FromString(const char *u);
extern PyObject *PyUnicode_FromFormat(const char *format, ...);
extern PyObject *PyUnicode_FromFormatV(const char *format, va_list vargs);
extern PyObject *PyUnicode_FromEncodedObject(PyObject *obj,
                                             const char *encoding,
                                             const char *errors);
extern Py_ssize_t PyUnicode_GetLength(PyObject *unicode);
extern Py_ssize_t PyUnicode_CopyCharacters(PyObject *to, Py_ssize_t to_start,
                                           PyObject *from,
                                           Py_ssize_t from_start,
                                           Py_ssize_t how_many);
extern Py_ssize_t PyUnicode_Fill(PyObject *unicode, Py_ssize_t start,
                                 Py_ssize_t length, Py_UCS4 fill_char);
extern int PyUnicode_WriteChar(PyObject *unicode, Py_ssize_t index,
                               Py_UCS4 character);
extern Py_UCS4 PyUnicode_ReadChar(PyObject *unicode, Py_ssize_t index);
extern PyObject *PyUnicode_Substring(PyObject *str, Py_ssize_t start,
                                     Py_ssize_t end);
extern Py_UCS4 *PyUnicode_AsUCS4(PyObject *u, Py_UCS4 *buffer,
                                 Py_ssize_t buflen, int copy_null);
extern Py_UCS4 *PyUnicode_AsUCS4Copy(PyObject *u);
extern PyObject *PyUnicode_FromUnicode(const Py_UNICODE *u, Py_ssize_t size);
extern Py_UNICODE *PyUnicode_AsUnicode(PyObject *unicode);
extern PyObject *PyUnicode_TransformDecimalToASCII(Py_UNICODE *s,
                                                   Py_ssize_t size);
extern Py_UNICODE *PyUnicode_AsUnicodeAndSize(PyObject *unicode,
                                              Py_ssize_t *size);
extern Py_UNICODE *PyUnicode_AsUnicodeCopy(PyObject *unicode);
extern Py_ssize_t PyUnicode_GetSize(PyObject *unicode);
extern PyObject *PyUnicode_FromObject(PyObject *obj);
extern PyObject *PyUnicode_DecodeLocaleAndSize(const char *str, Py_ssize_t len,
                                               const char *errors);
extern PyObject *PyUnicode_DecodeLocale(const char *str, const char *errors);
extern PyObject *PyUnicode_EncodeLocale(PyObject *unicode, const char *errors);
extern int PyUnicode_FSConverter(PyObject *obj, void *result);
extern int PyUnicode_FSDecoder(PyObject *obj, void *result);
extern PyObject *PyUnicode_DecodeFSDefaultAndSize(const char *s,
                                                  Py_ssize_t size);
extern PyObject *PyUnicode_DecodeFSDefault(const char *s);
extern PyObject *PyUnicode_EncodeFSDefault(PyObject *unicode);
extern PyObject *PyUnicode_FromWideChar(const wchar_t *w, Py_ssize_t size);
extern Py_ssize_t PyUnicode_AsWideChar(PyUnicodeObject *unicode, wchar_t *w,
                                       Py_ssize_t size);
extern wchar_t *PyUnicode_AsWideCharString(PyObject *unicode, Py_ssize_t *size);
extern PyObject *PyUnicode_Decode(const char *s, Py_ssize_t size,
                                  const char *encoding, const char *errors);
extern PyObject *PyUnicode_AsEncodedString(PyObject *unicode,
                                           const char *encoding,
                                           const char *errors);
extern PyObject *PyUnicode_Encode(const Py_UNICODE *s, Py_ssize_t size,
                                  const char *encoding, const char *errors);
extern PyObject *PyUnicode_DecodeUTF8(const char *s, Py_ssize_t size,
                                      const char *errors);
extern PyObject *PyUnicode_DecodeUTF8Stateful(const char *s, Py_ssize_t size,
                                              const char *errors,
                                              Py_ssize_t *consumed);
extern PyObject *PyUnicode_AsUTF8String(PyObject *unicode);
extern const char *PyUnicode_AsUTF8AndSize(PyObject *unicode, Py_ssize_t *size);
extern const char *PyUnicode_AsUTF8(PyObject *unicode);
extern PyObject *PyUnicode_EncodeUTF8(const Py_UNICODE *s, Py_ssize_t size,
                                      const char *errors);
extern PyObject *PyUnicode_DecodeUTF32(const char *s, Py_ssize_t size,
                                       const char *errors, int *byteorder);
extern PyObject *PyUnicode_DecodeUTF32Stateful(const char *s, Py_ssize_t size,
                                               const char *errors,
                                               int *byteorder,
                                               Py_ssize_t *consumed);
extern PyObject *PyUnicode_AsUTF32String(PyObject *unicode);
extern PyObject *PyUnicode_EncodeUTF32(const Py_UNICODE *s, Py_ssize_t size,
                                       const char *errors, int byteorder);
extern PyObject *PyUnicode_DecodeUTF16(const char *s, Py_ssize_t size,
                                       const char *errors, int *byteorder);
extern PyObject *PyUnicode_DecodeUTF16Stateful(const char *s, Py_ssize_t size,
                                               const char *errors,
                                               int *byteorder,
                                               Py_ssize_t *consumed);
extern PyObject *PyUnicode_AsUTF16String(PyObject *unicode);
extern PyObject *PyUnicode_EncodeUTF16(const Py_UNICODE *s, Py_ssize_t size,
                                       const char *errors, int byteorder);
extern PyObject *PyUnicode_DecodeUTF7(const char *s, Py_ssize_t size,
                                      const char *errors);
extern PyObject *PyUnicode_DecodeUTF7Stateful(const char *s, Py_ssize_t size,
                                              const char *errors,
                                              Py_ssize_t *consumed);
extern PyObject *PyUnicode_EncodeUTF7(const Py_UNICODE *s, Py_ssize_t size,
                                      int base64SetO, int base64WhiteSpace,
                                      const char *errors);
extern PyObject *PyUnicode_DecodeUnicodeEscape(const char *s, Py_ssize_t size,
                                               const char *errors);
extern PyObject *PyUnicode_AsUnicodeEscapeString(PyObject *unicode);
extern PyObject *PyUnicode_EncodeUnicodeEscape(const Py_UNICODE *s,
                                               Py_ssize_t size);
extern PyObject *PyUnicode_DecodeRawUnicodeEscape(const char *s,
                                                  Py_ssize_t size,
                                                  const char *errors);
extern PyObject *PyUnicode_AsRawUnicodeEscapeString(PyObject *unicode);
extern PyObject *PyUnicode_EncodeRawUnicodeEscape(const Py_UNICODE *s,
                                                  Py_ssize_t size,
                                                  const char *errors);
extern PyObject *PyUnicode_DecodeLatin1(const char *s, Py_ssize_t size,
                                        const char *errors);
extern PyObject *PyUnicode_AsLatin1String(PyObject *unicode);
extern PyObject *PyUnicode_EncodeLatin1(const Py_UNICODE *s, Py_ssize_t size,
                                        const char *errors);
extern PyObject *PyUnicode_DecodeASCII(const char *s, Py_ssize_t size,
                                       const char *errors);
extern PyObject *PyUnicode_AsASCIIString(PyObject *unicode);
extern PyObject *PyUnicode_EncodeASCII(const Py_UNICODE *s, Py_ssize_t size,
                                       const char *errors);
extern PyObject *PyUnicode_DecodeCharmap(const char *data, Py_ssize_t size,
                                         PyObject *mapping, const char *errors);
extern PyObject *PyUnicode_AsCharmapString(PyObject *unicode,
                                           PyObject *mapping);
extern PyObject *PyUnicode_EncodeCharmap(const Py_UNICODE *s, Py_ssize_t size,
                                         PyObject *mapping, const char *errors);
extern PyObject *PyUnicode_Translate(PyObject *unicode, PyObject *mapping,
                                     const char *errors);
extern PyObject *PyUnicode_TranslateCharmap(const Py_UNICODE *s,
                                            Py_ssize_t size, PyObject *mapping,
                                            const char *errors);
extern PyObject *PyUnicode_DecodeMBCS(const char *s, Py_ssize_t size,
                                      const char *errors);
extern PyObject *PyUnicode_DecodeMBCSStateful(const char *s, int size,
                                              const char *errors,
                                              int *consumed);
extern PyObject *PyUnicode_AsMBCSString(PyObject *unicode);
extern PyObject *PyUnicode_EncodeCodePage(int code_page, PyObject *unicode,
                                          const char *errors);
extern PyObject *PyUnicode_EncodeMBCS(const Py_UNICODE *s, Py_ssize_t size,
                                      const char *errors);
extern PyObject *PyUnicode_Concat(PyObject *left, PyObject *right);
extern PyObject *PyUnicode_Split(PyObject *s, PyObject *sep,
                                 Py_ssize_t maxsplit);
extern PyObject *PyUnicode_Splitlines(PyObject *s, int keepend);
extern PyObject *PyUnicode_Join(PyObject *separator, PyObject *seq);
extern Py_ssize_t PyUnicode_Tailmatch(PyObject *str, PyObject *substr,
                                      Py_ssize_t start, Py_ssize_t end,
                                      int direction);
extern Py_ssize_t PyUnicode_Find(PyObject *str, PyObject *substr,
                                 Py_ssize_t start, Py_ssize_t end,
                                 int direction);
extern Py_ssize_t PyUnicode_FindChar(PyObject *str, Py_UCS4 ch,
                                     Py_ssize_t start, Py_ssize_t end,
                                     int direction);
extern Py_ssize_t PyUnicode_Count(PyObject *str, PyObject *substr,
                                  Py_ssize_t start, Py_ssize_t end);
extern PyObject *PyUnicode_Replace(PyObject *str, PyObject *substr,
                                   PyObject *replstr, Py_ssize_t maxcount);
extern int PyUnicode_Compare(PyObject *left, PyObject *right);
extern int PyUnicode_CompareWithASCIIString(PyObject *uni, const char *string);
extern PyObject *PyUnicode_RichCompare(PyObject *left, PyObject *right, int op);
extern PyObject *PyUnicode_Format(PyObject *format, PyObject *args);
extern int PyUnicode_Contains(PyObject *container, PyObject *element);
extern void PyUnicode_InternInPlace(PyObject **string);
extern PyObject *PyUnicode_InternFromString(const char *v);
/* end unicode.rst */
/* start utilities.rst */
/* end utilities.rst */
/* start veryhigh.rst */
extern int Py_Main(int argc, wchar_t **argv);
extern int PyRun_AnyFile(FILE *fp, const char *filename);
extern int PyRun_AnyFileFlags(FILE *fp, const char *filename,
                              struct PyCompilerFlags *flags);
extern int PyRun_AnyFileEx(FILE *fp, const char *filename, int closeit);
extern int PyRun_AnyFileExFlags(FILE *fp, const char *filename, int closeit,
                                PyCompilerFlags *flags);
extern int PyRun_SimpleString(const char *command);
extern int PyRun_SimpleStringFlags(const char *command, PyCompilerFlags *flags);
extern int PyRun_SimpleFile(FILE *fp, const char *filename);
extern int PyRun_SimpleFileEx(FILE *fp, const char *filename, int closeit);
extern int PyRun_SimpleFileExFlags(FILE *fp, const char *filename, int closeit,
                                   PyCompilerFlags *flags);
extern int PyRun_InteractiveOne(FILE *fp, const char *filename);
extern int PyRun_InteractiveOneFlags(FILE *fp, const char *filename,
                                     PyCompilerFlags *flags);
extern int PyRun_InteractiveLoop(FILE *fp, const char *filename);
extern int PyRun_InteractiveLoopFlags(FILE *fp, const char *filename,
                                      PyCompilerFlags *flags);
extern int (*PyOS_InputHook)();
extern char *(*PyOS_ReadlineFunctionPointer)(FILE *, FILE *, const char *);
extern struct _node *PyParser_SimpleParseString(const char *str, int start);
extern struct _node *PyParser_SimpleParseStringFlags(const char *str, int start,
                                                     int flags);
extern struct _node *
PyParser_SimpleParseStringFlagsFilename(const char *str, const char *filename,
                                        int start, int flags);
extern struct _node *PyParser_SimpleParseFile(FILE *fp, const char *filename,
                                              int start);
extern struct _node *PyParser_SimpleParseFileFlags(FILE *fp,
                                                   const char *filename,
                                                   int start, int flags);
extern PyObject *PyRun_String(const char *str, int start, PyObject *globals,
                              PyObject *locals);
extern PyObject *PyRun_StringFlags(const char *str, int start,
                                   PyObject *globals, PyObject *locals,
                                   PyCompilerFlags *flags);
extern PyObject *PyRun_File(FILE *fp, const char *filename, int start,
                            PyObject *globals, PyObject *locals);
extern PyObject *PyRun_FileEx(FILE *fp, const char *filename, int start,
                              PyObject *globals, PyObject *locals, int closeit);
extern PyObject *PyRun_FileFlags(FILE *fp, const char *filename, int start,
                                 PyObject *globals, PyObject *locals,
                                 PyCompilerFlags *flags);
extern PyObject *PyRun_FileExFlags(FILE *fp, const char *filename, int start,
                                   PyObject *globals, PyObject *locals,
                                   int closeit, PyCompilerFlags *flags);
extern PyObject *Py_CompileString(const char *str, const char *filename,
                                  int start);
extern PyObject *Py_CompileStringFlags(const char *str, const char *filename,
                                       int start, PyCompilerFlags *flags);
extern PyObject *Py_CompileStringObject(const char *str, PyObject *filename,
                                        int start, PyCompilerFlags *flags,
                                        int optimize);
extern PyObject *Py_CompileStringExFlags(const char *str, const char *filename,
                                         int start, PyCompilerFlags *flags,
                                         int optimize);
extern PyObject *PyEval_EvalCode(PyObject *co, PyObject *globals,
                                 PyObject *locals);
extern PyObject *PyEval_EvalCodeEx(PyObject *co, PyObject *globals,
                                   PyObject *locals, PyObject **args,
                                   int argcount, PyObject **kws, int kwcount,
                                   PyObject **defs, int defcount,
                                   PyObject *kwdefs, PyObject *closure);
typedef struct PyFrameObject PyFrameObject;
struct PyFrameObject {
  PyObject_HEAD
};
extern PyObject *PyEval_EvalFrame(PyFrameObject *f);
extern PyObject *PyEval_EvalFrameEx(PyFrameObject *f, int throwflag);
extern int PyEval_MergeCompilerFlags(PyCompilerFlags *cf);
enum {
  Py_eval_input,
  Py_file_input,
  Py_single_input,
};
typedef struct PyCompilerFlags PyCompilerFlags;
struct PyCompilerFlags {
  int cf_flags;
};
enum { CO_FUTURE_DIVISION };
/* end veryhigh.rst */
/* start weakref.rst */
#define PyWeakref_Check(ob) PyWeakref_CheckRef(ob) | PyWeakref_CheckProxy(ob)
#define PyWeakref_CheckRef(ob)
#define PyWeakref_CheckProxy(ob)
extern PyObject *PyWeakref_NewRef(PyObject *ob, PyObject *callback);
extern PyObject *PyWeakref_NewProxy(PyObject *ob, PyObject *callback);
extern PyObject *PyWeakref_GetObject(PyObject *ref);
extern PyObject *PyWeakref_GET_OBJECT(PyObject *ref);
/* end weakref.rst */

#ifdef __cplusplus
}
#endif
