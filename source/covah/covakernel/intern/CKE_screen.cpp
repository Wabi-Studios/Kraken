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
 * COVAH Kernel.
 * Purple Underground.
 */

#include "UNI_area.h"
#include "UNI_context.h"
#include "UNI_factory.h"
#include "UNI_object.h"
#include "UNI_operator.h"
#include "UNI_pixar_utils.h"
#include "UNI_region.h"
#include "UNI_screen.h"
#include "UNI_space_types.h"
#include "UNI_userpref.h"
#include "UNI_window.h"
#include "UNI_wm_types.h"
#include "UNI_workspace.h"

#include "CKE_screen.h"

#include <wabi/usd/usd/tokens.h>

WABI_NAMESPACE_BEGIN


/* keep global; this has to be accessible outside of windowmanager */
static std::vector<SpaceType *> spacetypes;


SdfPath make_screenpath(int id, const char *name)
{
  SdfPath path(STRINGALL(COVAH_PATH_DEFAULTS::COVAH_WORKSPACES));
  return path.AppendPath(SdfPath(name + STRINGALL(id)));
}


int find_free_screenid(Main *cmain)
{
  int id = 1;

  UNIVERSE_FOR_ALL(screen, cmain->screens)
  {
    if (id <= screen->winid)
    {
      id = screen->winid + 1;
    }
  }
  return id;
}


bool CKE_screen_is_used(const cScreen *screen)
{
  return (screen->winid != 0);
}


SpaceType *CKE_spacetype_from_id(int spaceid)
{
  UNIVERSE_FOR_ALL(st, spacetypes)
  {
    if (st->spaceid == spaceid)
    {
      return st;
    }
  }
  return NULL;
}


ScrArea *CKE_screen_find_big_area(cScreen *screen, const int spacetype, const short min)
{
  ScrArea *big = nullptr;
  int maxsize = 0;

  UNIVERSE_FOR_ALL(area, screen->areas)
  {
    if (spacetype == SPACE_TYPE_ANY)
    {
      GfVec2f winsize = FormFactory(area->size);

      if (min <= GET_X(winsize) && min <= GET_Y(winsize))
      {
        int size = GET_X(winsize) * GET_Y(winsize);

        if (size > maxsize)
        {
          maxsize = size;
          big = area;
        }
      }
    }
  }

  return big;
}

void CKE_screen_sort_scrvert(ScrVert **v1, ScrVert **v2)
{
  if (*v1 > *v2)
  {
    ScrVert *tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
  }
}

WABI_NAMESPACE_END