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
 * Anchor.
 * Bare Metal.
 */

#include "ANCHOR_system.h"

#if defined(__linux__)
#  include "ANCHOR_BACKEND_sdl.h"
#elif defined(WIN32)
#  include "ANCHOR_BACKEND_win32.h"
#elif defined(__APPLE__)
#  include "ANCHOR_BACKEND_cocoa.h"
#endif

ANCHOR_ISystem *ANCHOR_ISystem::m_system = NULL;

eAnchorStatus ANCHOR_ISystem::createSystem()
{
  eAnchorStatus success;
  if (!m_system)
  {
#if defined(__linux__)
    m_system = new ANCHOR_SystemSDL();
#elif defined(WIN32)
    m_system = new ANCHOR_SystemWin32();
#elif defined(__APPLE__)
    m_system = new ANCHOR_SystemCocoa();
#endif
    success = m_system != NULL ? ANCHOR_SUCCESS : ANCHOR_ERROR;
  }
  else
  {
    success = ANCHOR_ERROR;
  }
  if (success)
  {
    success = m_system->init();
  }
  return success;
}

eAnchorStatus ANCHOR_ISystem::destroySystem()
{
  eAnchorStatus success = ANCHOR_SUCCESS;
  if (m_system)
  {
    delete m_system;
    m_system = NULL;
  }
  else
  {
    success = ANCHOR_ERROR;
  }
  return success;
}

ANCHOR_ISystem *ANCHOR_ISystem::getSystem()
{
  return m_system;
}