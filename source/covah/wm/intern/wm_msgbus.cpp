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
 * Window Manager.
 * Making GUI Fly.
 */

#include "WM_msgbus.h"
#include "WM_debug_codes.h"
#include "WM_operators.h"
#include "WM_window.h"

#include "UNI_path_defaults.h"
#include "UNI_screen.h"
#include "UNI_userpref.h"
#include "UNI_window.h"

#include "CKE_context.h"
#include "CKE_main.h"

#include <mutex>
#include <string>*
#include <vector>


/**
 *  -----  The Covah WindowManager. ----- */


WABI_NAMESPACE_BEGIN


/* ------ */


/**
 *  -----  The MsgBus Callback. ----- */


MsgBusCallback::MsgBusCallback(wmOperatorType *ot)
  : ref(1),
    notice(ot->notice),
    op({.type = ot})
{}


void MsgBusCallback::COMM(const TfNotice &notice,
                          MsgBus const &sender)
{
  TF_DEBUG(COVAH_DEBUG_MSGBUS).Msg("!! Hello from MsgBus.\n");
  TF_DEBUG(COVAH_DEBUG_MSGBUS).Msg("Operator: %s\n", op.type->idname);
  TF_DEBUG(COVAH_DEBUG_MSGBUS).Msg("RefCount: %s\n", CHARALL(ref));
  ++ref;
}


void WM_operatortype_append(const cContext &C, void (*opfunc)(wmOperatorType *))
{
  /* ------ */

  wmWindowManager wm = CTX_wm_manager(C);

  /** 
   * We keep all operators in
   * a RobinHood HashMap. Hashed
   * with a Unique TfToken, and
   * stored on a UsdPrim. As is
   * the same for the rest of
   * the Covah Stack. */

  RHash *rh = new RHash();

  wmOperatorType *ot = new wmOperatorType();
  opfunc(ot);

  /** Hashed. */
  rh->insert(typename RHash::value_type(
    std::make_pair(TfToken(ot->idname),
                   COVAH_PRIM_OPERATOR_CREATE(C, ot->idname).GetPath())));

  /** 
   * Have this Operator Shoot a message back up
   * to COMM, and any clients which have oppted
   * to SubcribeTo<SomeType>() will now be getting
   * notifications of this operator's state. */
  TfNotice notice = TfNotice();
  MsgBusCallback *cb = new MsgBusCallback(ot);
  MsgBus invoker(cb);
  TfNotice::Register(invoker, &MsgBusCallback::COMM, invoker);

  notice.Send(invoker); /* Operator says Hello. */
}

/**
 *  -----  MsgBus Initialization. ----- */


void WM_msgbus_register(const cContext &C)
{
  /* ------ */

  WM_operatortype_append((C), WM_OT_window_close);
  WM_operatortype_append((C), WM_OT_window_new);
  WM_operatortype_append((C), WM_OT_window_new_main);
  WM_operatortype_append((C), WM_OT_window_fullscreen_toggle);
  WM_operatortype_append((C), WM_OT_quit_covah);

  /* ------ */
}


WABI_NAMESPACE_END