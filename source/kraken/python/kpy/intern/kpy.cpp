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

#include "KLI_utildefines.h"
#include "KLI_string_utils.h"

#include "KKE_appdir.h"
#include "KKE_version.h"
#include "KKE_context.h"
#include "KKE_main.h"
#include "KKE_robinhood.h"
#include "KKE_utils.h"

#include "UNI_access.h"
#include "UNI_factory.h"
#include "UNI_types.h"
#include "UNI_wm_types.h"

#include "kpy.h"
#include "kpy_capi_utils.h"
#include "kpy_path.h"
#include "kpy_interface.h"
#include "kpy_intern_string.h"
#include "kpy_library.h"
#include "kpy_uni.h"

WABI_NAMESPACE_BEGIN

PyObject *kpy_package_py = NULL;

PyDoc_STRVAR(kpy_script_paths_doc,
             ".. function:: script_paths()\n"
             "\n"
             "   Return 2 paths to kraken scripts directories.\n"
             "\n"
             "   :return: (system, user) strings will be empty when not found.\n"
             "   :rtype: tuple of strings\n");
static PyObject *kpy_script_paths(PyObject *UNUSED(self))
{
  PyObject *ret = PyTuple_New(2);
  PyObject *item;
  const char *path;

  path = KKE_appdir_folder_id(KRAKEN_SYSTEM_SCRIPTS, NULL);
  item = PyC_UnicodeFromByte(path ? path : "");
  KLI_assert(item != NULL);
  PyTuple_SET_ITEM(ret, 0, item);
  path = KKE_appdir_folder_id(KRAKEN_USER_SCRIPTS, NULL);
  item = PyC_UnicodeFromByte(path ? path : "");
  KLI_assert(item != NULL);
  PyTuple_SET_ITEM(ret, 1, item);

  return ret;
}

static bool kpy_resolver_paths_visit_cb(void *userdata, char *UNUSED(path_dst), const char *path_src)
{
  // PyList_APPEND((PyObject *)userdata, PyC_UnicodeFromByte(path_src));
  return false; /* never edits the path */
}

PyDoc_STRVAR(kpy_resolver_paths_doc,
             ".. function:: resolver_paths(absolute=False, packed=False, local=False)\n"
             "\n"
             "   Returns a list of paths to external assets referenced by the loaded .usd file.\n"
             "\n"
             "   :arg absolute: When true the paths returned are made absolute.\n"
             "   :type absolute: boolean\n"
             "   :arg packed: When true skip file paths for packed data.\n"
             "   :type packed: boolean\n"
             "   :arg local: When true skip linked library paths.\n"
             "   :type local: boolean\n"
             "   :return: path list.\n"
             "   :rtype: list of strings\n");
static PyObject *kpy_resolver_paths(PyObject *UNUSED(self), PyObject *args, PyObject *kw)
{
  int flag = 0;
  PyObject *list;

  bool absolute = false;
  bool packed = false;
  bool local = false;

  static const char *_keywords[] = {"absolute", "packed", "local", NULL};
  static _PyArg_Parser _parser = {"|$O&O&O&:resolver_paths", _keywords, 0};
  if (!_PyArg_ParseTupleAndKeywordsFast(args,
                                        kw,
                                        &_parser,
                                        PyC_ParseBool,
                                        &absolute,
                                        PyC_ParseBool,
                                        &packed,
                                        PyC_ParseBool,
                                        &local)) {
    return NULL;
  }

  // if (absolute) {
  //   flag |= KKE_RESOLVER_TRAVERSE_ABS;
  // }
  // if (!packed) {
  //   flag |= KKE_RESOLVER_TRAVERSE_SKIP_PACKED;
  // }
  // if (local) {
  //   flag |= KKE_RESOLVER_TRAVERSE_SKIP_LIBRARY;
  // }

  list = PyList_New(0);

  // KKE_resolver_traverse_main(G.main, kpy_resolver_paths_visit_cb, flag, (void *)list);

  return list;
}

static PyMethodDef meth_kpy_script_paths = {
    "script_paths",
    (PyCFunction)kpy_script_paths,
    METH_NOARGS,
    kpy_script_paths_doc,
};
static PyMethodDef meth_kpy_resolver_paths = {
    "kraken_paths",
    (PyCFunction)kpy_resolver_paths,
    METH_VARARGS | METH_KEYWORDS,
    kpy_resolver_paths_doc,
};
// static PyMethodDef meth_kpy_user_resource = {
//     "user_resource",
//     (PyCFunction)kpy_user_resource,
//     METH_VARARGS | METH_KEYWORDS,
//     NULL,
// };
// static PyMethodDef meth_kpy_system_resource = {
//     "system_resource",
//     (PyCFunction)kpy_system_resource,
//     METH_VARARGS | METH_KEYWORDS,
//     kpy_system_resource_doc,
// };
// static PyMethodDef meth_kpy_resource_path = {
//     "resource_path",
//     (PyCFunction)kpy_resource_path,
//     METH_VARARGS | METH_KEYWORDS,
//     kpy_resource_path_doc,
// };
// static PyMethodDef meth_kpy_escape_identifier = {
//     "escape_identifier",
//     (PyCFunction)kpy_escape_identifier,
//     METH_O,
//     kpy_escape_identifier_doc,
// };
// static PyMethodDef meth_kpy_unescape_identifier = {
//     "unescape_identifier",
//     (PyCFunction)kpy_unescape_identifier,
//     METH_O,
//     kpy_unescape_identifier_doc,
// };

static PyObject *kpy_import_test(const char *modname)
{
  PyObject *mod = PyImport_ImportModuleLevel(modname, NULL, NULL, NULL, 0);

  // GPU_kvk_end();

  if (mod) {
    Py_DECREF(mod);
  }
  else {
    PyErr_Print();
    PyErr_Clear();
  }

  return mod;
}

/******************************************************************************
 * Description: Creates the kpy module and adds it to sys.modules for importing
 ******************************************************************************/
void KPy_init_modules(struct kContext *C)
{
  PointerUNI ctx_ptr;
  PyObject *mod;

  /* Needs to be first since this dir is needed for future modules */
  const char *const modpath = KKE_appdir_folder_id(KRAKEN_SYSTEM_SCRIPTS, "modules");
  if (modpath) {
    // printf("kpy: found module path '%s'.\n", modpath);
    PyObject *sys_path = PySys_GetObject("path"); /* borrow */
    PyObject *py_modpath = PyUnicode_FromString(modpath);
    PyList_Insert(sys_path, 0, py_modpath); /* add first */
    Py_DECREF(py_modpath);
  }
  else {
    printf("kpy: couldn't find 'scripts/modules', kraken probably won't start.\n");
  }
  /* stand alone utility modules not related to kraken directly */
  //IDProp_Init_Types(); /* not actually a submodule, just types */

  mod = PyModule_New("_kpy");

  /* add the module so we can import it */
  PyDict_SetItemString(PyImport_GetModuleDict(), "_kpy", mod);
  Py_DECREF(mod);

  /* run first, initializes rna types */
  KPY_uni_init();

  /* needs to be first so kpy_types can run */
  PyModule_AddObject(mod, "types", KPY_uni_types());

  /* needs to be first so kpy_types can run */
  KPY_library_load_type_ready();

  // KPY_uni_data_context_type_ready();

  // KPY_uni_gizmo_module(mod);

  kpy_import_test("kpy_types");
  PyModule_AddObject(mod, "data", KPY_uni_module()); /* imports kpy_types by running this */
  kpy_import_test("kpy_types");
  // PyModule_AddObject(mod, "props", KPY_uni_props());
  /* ops is now a python module that does the conversion from SOME_OT_foo -> some.foo */
  // PyModule_AddObject(mod, "ops", KPY_operator_module());
  // PyModule_AddObject(mod, "app", KPY_app_struct());
  // PyModule_AddObject(mod, "_utils_units", KPY_utils_units());
  // PyModule_AddObject(mod, "_utils_previews", KPY_utils_previews_module());
  // PyModule_AddObject(mod, "msgbus", KPY_msgbus_module());

  CreationFactory::PTR::New(SdfPath("/"), &UNI_Context, C, &ctx_ptr);
  kpy_context_module = (KPy_ObjectUNI *)pyuni_object_CreatePyObject(&ctx_ptr);
  /* odd that this is needed, 1 ref on creation and another for the module
   * but without we get a crash on exit */
  Py_INCREF(kpy_context_module);

  PyModule_AddObject(mod, "context", (PyObject *)kpy_context_module);

  /* Register methods and property get/set for RNA types. */
//   KPY_uni_types_extend_capi();

  /* utility func's that have nowhere else to go */
  PyModule_AddObject(mod,
                     meth_kpy_script_paths.ml_name,
                     (PyObject *)PyCFunction_New(&meth_kpy_script_paths, NULL));
  PyModule_AddObject(
      mod, meth_kpy_resolver_paths.ml_name, (PyObject *)PyCFunction_New(&meth_kpy_resolver_paths, NULL));
  // PyModule_AddObject(mod,
  //                    meth_kpy_user_resource.ml_name,
  //                    (PyObject *)PyCFunction_New(&meth_kpy_user_resource, NULL));
  // PyModule_AddObject(mod,
  //                    meth_kpy_system_resource.ml_name,
  //                    (PyObject *)PyCFunction_New(&meth_kpy_system_resource, NULL));
  // PyModule_AddObject(mod,
  //                    meth_kpy_resource_path.ml_name,
  //                    (PyObject *)PyCFunction_New(&meth_kpy_resource_path, NULL));
  // PyModule_AddObject(mod,
  //                    meth_kpy_escape_identifier.ml_name,
  //                    (PyObject *)PyCFunction_New(&meth_kpy_escape_identifier, NULL));
  // PyModule_AddObject(mod,
  //                    meth_kpy_unescape_identifier.ml_name,
  //                    (PyObject *)PyCFunction_New(&meth_kpy_unescape_identifier, NULL));

  /* register funcs (kpy_uni.c) */
  PyModule_AddObject(mod,
                     meth_kpy_register_class.ml_name,
                     (PyObject *)PyCFunction_New(&meth_kpy_register_class, NULL));
  PyModule_AddObject(mod,
                     meth_kpy_unregister_class.ml_name,
                     (PyObject *)PyCFunction_New(&meth_kpy_unregister_class, NULL));

  PyModule_AddObject(mod,
                     meth_kpy_owner_id_get.ml_name,
                     (PyObject *)PyCFunction_New(&meth_kpy_owner_id_get, NULL));
  PyModule_AddObject(mod,
                     meth_kpy_owner_id_set.ml_name,
                     (PyObject *)PyCFunction_New(&meth_kpy_owner_id_set, NULL));

  /* add our own modules dir, this is a python package */
  kpy_package_py = kpy_import_test("kpy");
}

WABI_NAMESPACE_END