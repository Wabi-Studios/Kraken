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
 * COVAH Python.
 * It Bites.
 */

#include "WPY_init_exit.h" /* Own Include. */

#include "CKE_context.h"

#include <wabi/base/arch/systemInfo.h>
#include <wabi/base/arch/vsnprintf.h>

#include <wabi/base/tf/error.h>
#include <wabi/base/tf/pathUtils.h>
#include <wabi/base/tf/pyInterpreter.h>
#include <wabi/base/tf/stringUtils.h>


/**
 *  -----  The Covah Python Module. ----- */


WABI_NAMESPACE_BEGIN


/* ------ */


/**
 *  -----  Python Init & Exit. ----- */


void WPY_python_init(cContext *C)
{
  Main *cmain = CTX_data_main(C);

  setenv("PYTHONPATH", CHARSTR(cmain->python_path), true);

  TfPyInitialize();
}


void WPY_python_exit()
{
  /**
   * TODO: STUBBED */
}


/* ------ */


WABI_NAMESPACE_END