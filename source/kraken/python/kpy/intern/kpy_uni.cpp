/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2021, Wabi.
 */

/**
 * @file
 * KRAKEN Python.
 * It Bites.
 */

#include <Python.h>

#include <float.h> /* FLT_MIN/MAX */
#include <stddef.h>

#include "KLI_utildefines.h"
#include "KLI_string_utils.h"

#include "KKE_context.h"
#include "KKE_main.h"
#include "KKE_robinhood.h"
#include "KKE_utils.h"

#include "UNI_access.h"
#include "UNI_object.h"
#include "UNI_wm_types.h"

#include "kpy_interface.h"
#include "kpy_intern_string.h"
#include "kpy_uni.h"

#define USE_PEDANTIC_WRITE
#define USE_MATHUTILS
#define USE_STRING_COERCE

#define USE_POSTPONED_ANNOTATIONS

WABI_NAMESPACE_BEGIN

KPy_ObjectUNI *kpy_context_module = nullptr; /* for fast access */

static PyObject *pyuni_register_class(PyObject *self, PyObject *py_class);
static PyObject *pyuni_unregister_class(PyObject *self, PyObject *py_class);

static bool uni_disallow_writes = false;

static bool rna_id_write_error(PointerUNI *ptr, PyObject *key)
{
  return false;
}

bool pyuni_write_check(void)
{
  return !uni_disallow_writes;
}

void pyuni_write_set(bool val)
{
  uni_disallow_writes = !val;
}

static int kpy_class_validate(ObjectUNI *dummyptr, void *py_data, int *have_function)
{
  // return kpy_class_validate_recursive(dummyptr, dummyptr->type, py_data, have_function);
  return 1;
}

static int kpy_class_call(kContext *C, ObjectUNI *ptr, void *func, UsdAttributeVector parms)
{
  return 1;
}

static void kpy_class_free(void *pyob_ptr)
{

}

struct KPy_TypesModule_State {
  /** `UNI_KrakenUNI`. */
  PointerUNI ptr;
  /** `UNI_KrakenUNI.objects`, exposed as `kpy.types` */
  PropertyUNI *prop;
};

static PyObject *kpy_types_module_getattro(PyObject *self, PyObject *pyname)
{
  Py_RETURN_NONE;
}

static PyObject *kpy_types_module_dir(PyObject *self)
{
  Py_RETURN_NONE;
}

static struct PyMethodDef kpy_types_module_methods[] = {
    {"__getattr__", (PyCFunction)kpy_types_module_getattro, METH_O, NULL},
    {"__dir__", (PyCFunction)kpy_types_module_dir, METH_NOARGS, NULL},
    {NULL, NULL, 0, NULL},
};

PyDoc_STRVAR(kpy_types_module_doc, "Access to internal Kraken types");
static struct PyModuleDef kpy_types_module_def = {
    PyModuleDef_HEAD_INIT,
    "kpy.types",                          /* m_name */
    kpy_types_module_doc,                 /* m_doc */
    sizeof(struct KPy_TypesModule_State), /* m_size */
    kpy_types_module_methods,             /* m_methods */
    NULL,                                 /* m_reload */
    NULL,                                 /* m_traverse */
    NULL,                                 /* m_clear */
    NULL,                                 /* m_free */
};

/**
 * Accessed from Python as 'kpy.types' */
PyObject *KPY_uni_types(void)
{
  PyObject *submodule = PyModule_Create(&kpy_types_module_def);
  KPy_TypesModule_State *state = (KPy_TypesModule_State *)PyModule_GetState(submodule);

  UNI_kraken_uni_pointer_create(&state->ptr);
  state->prop = UNI_object_find_property(&state->ptr, "objects");

  /* Internal base types we have no other accessors for. */
  {
    static PyTypeObject *pyuni_types[] = {
        &pyuni_object_meta_idprop_Type,
        &pyuni_object_Type,
        &pyuni_prop_Type,
        &pyuni_prop_array_Type,
        &pyuni_prop_collection_Type,
        &pyuni_func_Type,
    };

    PyObject *submodule_dict = PyModule_GetDict(submodule);
    for (int i = 0; i < ARRAY_SIZE(pyuni_types); i += 1) {
      PyDict_SetItemString(submodule_dict, pyuni_types[i]->tp_name, (PyObject *)pyuni_types[i]);
    }
  }

  return submodule;
}

ObjectUNI *pyuni_object_as_uni(PyObject *self, const bool parent, const char *error_prefix)
{
  KPy_ObjectUNI *py_uni = NULL;
  ObjectUNI *uni;

  /* Unfortunately PyObject_GetAttrString won't look up this types tp_dict first :/ */
  if (PyType_Check(self)) {
    py_uni = (KPy_ObjectUNI *)PyDict_GetItem(((PyTypeObject *)self)->tp_dict, kpy_intern_str_kr_uni);
    Py_XINCREF(py_uni);
  }

  if (parent) {
    /* be very careful with this since it will return a parent classes uni.
     * modifying this will do confusing stuff! */
    if (py_uni == NULL) {
      py_uni = (KPy_ObjectUNI *)PyObject_GetAttr(self, kpy_intern_str_kr_uni);
    }
  }

  if (py_uni == NULL) {
    PyErr_Format(PyExc_RuntimeError,
                 "%.200s, missing kr_uni attribute from '%.200s' instance (may not be registered)",
                 error_prefix,
                 Py_TYPE(self)->tp_name);
    return NULL;
  }

  if (!KPy_ObjectUNI_Check(py_uni)) {
    PyErr_Format(PyExc_TypeError,
                 "%.200s, kr_uni attribute wrong type '%.200s' on '%.200s'' instance",
                 error_prefix,
                 Py_TYPE(py_uni)->tp_name,
                 Py_TYPE(self)->tp_name);
    Py_DECREF(py_uni);
    return NULL;
  }

  if (py_uni->ptr.type != &UNI_Object) {
    PyErr_Format(PyExc_TypeError,
                 "%.200s, kr_uni attribute not a UNI_Object, on '%.200s'' instance",
                 error_prefix,
                 Py_TYPE(self)->tp_name);
    Py_DECREF(py_uni);
    return NULL;
  }

  uni = (ObjectUNI*)py_uni->ptr.data;
  Py_DECREF(py_uni);

  return uni;
}

PyDoc_STRVAR(pyuni_register_class_doc,
             ".. method:: register_class(cls)\n"
             "\n"
             "   Register a subclass of a Kraken type class.\n"
             "\n"
             "   :arg cls: Kraken type class in:\n"
             "      :class:`kpy.types.Panel`, :class:`kpy.types.UIList`,\n"
             "      :class:`kpy.types.Menu`, :class:`kpy.types.Header`,\n"
             "      :class:`kpy.types.Operator`, :class:`kpy.types.KeyingSetInfo`,\n"
             "      :class:`kpy.types.RenderEngine`\n"
             "   :type cls: class\n"
             "   :raises ValueError:\n"
             "      if the class is not a subclass of a registerable kraken class.\n"
             "\n"
             "   .. note::\n"
             "\n"
             "      If the class has a *register* class method it will be called\n"
             "      before registration.\n");
PyMethodDef meth_kpy_register_class = {
    "register_class", pyuni_register_class, METH_O, pyuni_register_class_doc};
static PyObject *pyuni_register_class(PyObject *UNUSED(self), PyObject *py_class)
{
  kContext *C = nullptr;
  ReportList reports;
  ObjectRegisterFunc reg;
  ObjectUNI *uni;
  ObjectUNI *uni_new;
  const char *identifier;
  PyObject *py_cls_meth;
  const char *error_prefix = "register_class(...):";

  if (!PyType_Check(py_class)) {
    PyErr_Format(PyExc_ValueError,
                 "register_class(...): "
                 "expected a class argument, not '%.200s'",
                 Py_TYPE(py_class)->tp_name);
    return NULL;
  }

  if (PyDict_GetItem(((PyTypeObject *)py_class)->tp_dict, kpy_intern_str_kr_uni)) {
    PyErr_Format(PyExc_ValueError,
                 "register_class(...): "
                 "already registered as a subclass '%.200s'",
                 ((PyTypeObject *)py_class)->tp_name);
    return NULL;
  }

  if (!pyuni_write_check()) {
    PyErr_Format(PyExc_RuntimeError,
                 "register_class(...): "
                 "can't run in readonly state '%.200s'",
                 ((PyTypeObject *)py_class)->tp_name);
    return NULL;
  }

  /* Warning: gets parent classes uni, only for the register function. */
  uni = pyuni_object_as_uni(py_class, true, "register_class(...):");
  if (uni == NULL) {
    return NULL;
  }

  /* Check that we have a register callback for this type. */
  reg = UNI_object_register(uni);

  if (!reg) {
    PyErr_Format(PyExc_ValueError,
                 "register_class(...): expected a subclass of a registerable "
                 "UNI type (%.200s does not support registration)",
                 UNI_object_identifier(uni));
    return NULL;
  }

  /* Get the context, so register callback can do necessary refreshes. */
  C = KPY_context_get();

  /* Call the register callback with reports & identifier. */
  // KKE_reports_init(&reports, RPT_STORE);

  identifier = ((PyTypeObject *)py_class)->tp_name;

  uni_new = reg(CTX_data_main(C),
                &reports,
                py_class,
                identifier,
                kpy_class_validate,
                kpy_class_call,
                kpy_class_free);

  if (uni_new == NULL) {
    return NULL;
  }

  /* Takes a reference to 'py_class'. */
  // pyuni_subtype_set_rna(py_class, uni_new);

  /* Old uni still references us, keep the check in case registering somehow can free it. */
  // if (UNI_struct_py_type_get(uni)) {
    // UNI_struct_py_type_set(uni, NULL);
  // }

  // if (pyuni_deferred_register_class(uni_new, (PyTypeObject *)py_class) != 0) {
    // return NULL;
  // }

  /**
   * Call classed register method.
   * Note that zero falls through, no attribute, no error. */
  switch (_PyObject_LookupAttr(py_class, kpy_intern_str_register, &py_cls_meth)) {
    case 1: {
      PyObject *ret = PyObject_CallObject(py_cls_meth, NULL);
      Py_DECREF(py_cls_meth);
      if (ret) {
        Py_DECREF(ret);
      }
      else {
        return NULL;
      }
      break;
    }
    case -1: {
      return NULL;
    }
  }

  Py_RETURN_NONE;
}

#ifdef USE_PYUNI_ITER

static void pyuni_prop_collection_iter_dealloc(KPy_CollectionPropertyUNI *self);
static PyObject *pyuni_prop_collection_iter_next(KPy_CollectionPropertyUNI *self);

static PyTypeObject pyuni_prop_collection_iter_Type = {
    PyVarObject_HEAD_INIT(NULL, 0) "kpy_prop_collection_iter", /* tp_name */
    sizeof(KPy_CollectionPropertyUNI),                     /* tp_basicsize */
    0,                                                         /* tp_itemsize */
    /* methods */
    (destructor)pyuni_prop_collection_iter_dealloc, /* tp_dealloc */
    0,                                              /* tp_vectorcall_offset */
    NULL,                                           /* getattrfunc tp_getattr; */
    NULL,                                           /* setattrfunc tp_setattr; */
    NULL,
    /* tp_compare */ /* DEPRECATED in Python 3.0! */
    NULL,
    /* subclassed */ /* tp_repr */

    /* Method suites for standard classes */

    NULL, /* PyNumberMethods *tp_as_number; */
    NULL, /* PySequenceMethods *tp_as_sequence; */
    NULL, /* PyMappingMethods *tp_as_mapping; */

    /* More standard operations (here for binary compatibility) */

    NULL, /* hashfunc tp_hash; */
    NULL, /* ternaryfunc tp_call; */
    NULL, /* reprfunc tp_str; */

    /* will only use these if this is a subtype of a py class */
    PyObject_GenericGetAttr, /* getattrofunc tp_getattro; */
    NULL,                    /* setattrofunc tp_setattro; */

    /* Functions to access object as input/output buffer */
    NULL, /* PyBufferProcs *tp_as_buffer; */

    /*** Flags to define presence of optional/expanded features ***/
    Py_TPFLAGS_DEFAULT, /* long tp_flags; */

    NULL, /*  char *tp_doc;  Documentation string */
    /*** Assigned meaning in release 2.0 ***/
    /* call function for all accessible objects */
    NULL, /* traverseproc tp_traverse; */

    /* delete references to contained objects */
    NULL, /* inquiry tp_clear; */

    /***  Assigned meaning in release 2.1 ***/
    /*** rich comparisons (subclassed) ***/
    NULL, /* richcmpfunc tp_richcompare; */

/***  weak reference enabler ***/
#  ifdef USE_WEAKREFS
    offsetof(KPy_CollectionPropertyUNI, in_weakreflist), /* long tp_weaklistoffset; */
#  else
    0,
#  endif
    /*** Added in release 2.2 ***/
    /*   Iterators */
    PyObject_SelfIter,                             /* getiterfunc tp_iter; */
    (iternextfunc)pyuni_prop_collection_iter_next, /* iternextfunc tp_iternext; */

    /*** Attribute descriptor and subclassing stuff ***/
    NULL, /* struct PyMethodDef *tp_methods; */
    NULL, /* struct PyMemberDef *tp_members; */
    NULL, /* struct PyGetSetDef *tp_getset; */
    NULL, /* struct _typeobject *tp_base; */
    NULL, /* PyObject *tp_dict; */
    NULL, /* descrgetfunc tp_descr_get; */
    NULL, /* descrsetfunc tp_descr_set; */
    0,    /* long tp_dictoffset; */
    NULL, /* initproc tp_init; */
    NULL, /* allocfunc tp_alloc; */
    NULL, /* newfunc tp_new; */
    /*  Low-level free-memory routine */
    NULL, /* freefunc tp_free; */
    /* For PyObject_IS_GC */
    NULL, /* inquiry tp_is_gc; */
    NULL, /* PyObject *tp_bases; */
    /* method resolution order */
    NULL, /* PyObject *tp_mro; */
    NULL, /* PyObject *tp_cache; */
    NULL, /* PyObject *tp_subclasses; */
    NULL, /* PyObject *tp_weaklist; */
    NULL,
};

static PyObject *pyuni_prop_collection_iter_CreatePyObject(PointerUNI *ptr, PropertyUNI *prop)
{
  KPy_CollectionPropertyUNI *self = PyObject_New(KPy_CollectionPropertyUNI,
                                                 &pyuni_prop_collection_iter_Type);

#  ifdef USE_WEAKREFS
  self->in_weakreflist = NULL;
#  endif

  UNI_property_collection_begin(ptr, prop, self->iter);

  return (PyObject *)self;
}

static PyObject *pyuni_prop_collection_iter(KPy_PropertyUNI *self)
{
  return pyuni_prop_collection_iter_CreatePyObject(&self->ptr, self->prop);
}

static PyObject *pyuni_prop_collection_iter_next(KPy_CollectionPropertyUNI *self)
{
  if (self->iter.empty()) {
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }

  KPy_ObjectUNI *pyuni = (KPy_ObjectUNI *)pyuni_object_CreatePyObject(new ObjectUNI());

#  ifdef USE_PYUNI_OBJECT_REFERENCE
  if (pyuni) { /* Unlikely, but may fail. */
    if ((PyObject *)pyuni != Py_None) {
      /* hold a reference to the iterator since it may have
       * allocated memory 'pyuni' needs. eg: introspecting dynamic enum's. */
      /* TODO: we could have an api call to know if this is
       * needed since most collections don't */
      pyuni_object_reference_set(pyuni, (PyObject *)self);
    }
  }
#  endif /* !USE_PYUNI_OBJECT_REFERENCE */

  self->iter.push_back(new PropertyUNI());

  return (PyObject *)pyuni;
}

static void pyuni_prop_collection_iter_dealloc(KPy_CollectionPropertyUNI *self)
{
#  ifdef USE_WEAKREFS
  if (self->in_weakreflist != NULL) {
    PyObject_ClearWeakRefs((PyObject *)self);
  }
#  endif

  self->iter.end();

  PyObject_DEL(self);
}

#endif /* USE_PYUNI_ITER */

void KPY_uni_init(void)
{
  /* For some reason MSVC complains of these. */
#if defined(_MSC_VER)
  pyuni_object_meta_idprop_Type.tp_base = &PyType_Type;
#endif

  /* metaclass */
  if (PyType_Ready(&pyuni_object_meta_idprop_Type) < 0) {
    return;
  }

  if (PyType_Ready(&pyuni_object_Type) < 0) {
    return;
  }

  if (PyType_Ready(&pyuni_prop_Type) < 0) {
    return;
  }

  if (PyType_Ready(&pyuni_prop_array_Type) < 0) {
    return;
  }

  if (PyType_Ready(&pyuni_prop_collection_Type) < 0) {
    return;
  }

  // if (PyType_Ready(&pyuni_prop_collection_idprop_Type) < 0) {
  //   return;
  // }

  if (PyType_Ready(&pyuni_func_Type) < 0) {
    return;
  }

#ifdef USE_PYUNI_ITER
  if (PyType_Ready(&pyuni_prop_collection_iter_Type) < 0) {
    return;
  }
#endif
}

/* 'kpy.data' from Python. */
static PointerUNI *uni_module_ptr = NULL;
PyObject *KPY_uni_module(void)
{
  KPy_ObjectUNI *pyuni;
  PointerUNI ptr;

  /* For now, return the base RNA type rather than a real module. */
  UNI_main_pointer_create(G.main, &ptr);
  pyuni = (KPy_ObjectUNI *)pyuni_object_CreatePyObject(&ptr);

  uni_module_ptr = &pyuni->ptr;
  return (PyObject *)pyuni;
}

void pyuni_alloc_types(void)
{
// #ifdef DEBUG
//   PyGILState_STATE gilstate;

//   PointerUNI ptr;
//   PropertyUNI *prop;

//   gilstate = PyGILState_Ensure();

//   /* Avoid doing this lookup for every getattr. */
//   UNI_kraken_uni_pointer_create(&ptr);
//   prop = UNI_object_find_property(&ptr, "objects");

//   UNI_PROP_BEGIN (&ptr, itemptr, prop) {
//     PyObject *item = pyuni_object_Subtype(&itemptr);
//     if (item == NULL) {
//       if (PyErr_Occurred()) {
//         PyErr_Print();
//         PyErr_Clear();
//       }
//     }
//     else {
//       Py_DECREF(item);
//     }
//   }
//   UNI_PROP_END;

//   PyGILState_Release(gilstate);
// #endif /* DEBUG */
}

/*-----------------------CreatePyObject---------------------------------*/
PyObject *pyuni_object_CreatePyObject(PointerUNI *ptr)
{
  KPy_ObjectUNI *pyuni = NULL;

  /* Note: don't rely on this to return None since NULL data with a valid type can often crash. */
  if (ptr->data == NULL && ptr->type == NULL) { /* Operator RNA has NULL data. */
    Py_RETURN_NONE;
  }

  void **instance = ptr->data ? UNI_object_instance(ptr) : NULL;
  if (instance && *instance) {
    pyuni = (KPy_ObjectUNI *)*instance;

    /* Refine may have changed types after the first instance was created. */
    if (ptr->type == pyuni->ptr.type) {
      Py_INCREF(pyuni);
      return (PyObject *)pyuni;
    }

    /* Existing users will need to use 'type_recast' method. */
    Py_DECREF(pyuni);
    *instance = NULL;
    /* Continue as if no instance was made. */
#if 0 /* No need to assign, will be written to next... */
      pyuni = NULL;
#endif
  }

  // {
    // PyTypeObject *tp = (PyTypeObject *)pyuni_object_Subtype(ptr);

    // if (tp) {
      // pyuni = (KPy_ObjectUNI *)tp->tp_alloc(tp, 0);
// #ifdef USE_PYUNI_OBJECT_REFERENCE
      /* #PyType_GenericAlloc will have set tracking.
       * We only want tracking when `StructRNA.reference` has been set. */
      // if (pyuni != NULL) {
        // PyObject_GC_UnTrack(pyuni);
      // }
// #endif
      // Py_DECREF(tp); /* srna owns, can't hold a reference. */
    // }
    // else {
      // TF_MSG("could not make type '%s'", UNI_object_identifier(ptr->type));

// #ifdef USE_PYUNI_OBJECT_REFERENCE
      // pyuni = (KPy_ObjectUNI *)PyObject_GC_New(KPy_ObjectUNI, &pyuni_object_Type);
// #else
      // pyuni = (KPy_ObjectUNI *)PyObject_New(KPy_ObjectUNI, &pyuni_object_Type);
// #endif

// #ifdef USE_WEAKREFS
      // if (pyuni != NULL) {
        // pyuni->in_weakreflist = NULL;
      // }
// #endif
    // }
  // }

  if (pyuni == NULL) {
    PyErr_SetString(PyExc_MemoryError, "couldn't create kpy_object");
    return NULL;
  }

  /* Blender's instance owns a reference (to avoid Python freeing it). */
  if (instance) {
    *instance = pyuni;
    Py_INCREF(pyuni);
  }

  pyuni->ptr = *ptr;
#ifdef PYUNI_FREE_SUPPORT
  pyuni->freeptr = false;
#endif

#ifdef USE_PYUNI_OBJECT_REFERENCE
  pyuni->reference = NULL;
#endif

  // PyC_ObSpit("NewStructRNA: ", (PyObject *)pyuni);

#ifdef USE_PYUNI_INVALIDATE_WEAKREF
  if (ptr->owner_id) {
    id_weakref_pool_add(ptr->owner_id, (KPy_DummyPointerRNA *)pyuni);
  }
#endif
  return (PyObject *)pyuni;
}

WABI_NAMESPACE_END