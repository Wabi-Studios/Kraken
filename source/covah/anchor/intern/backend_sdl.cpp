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

// ANCHOR: Platform Backend for SDL2
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)
// (Info: SDL2 is a cross-platform general purpose library for handling windows, inputs, graphics
// context creation, etc.) (Requires: SDL 2.0. Prefer SDL 2.0.4+ for full feature support.)

// Implemented features:
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |=
//  ANCHORConfigFlags_NoMouseCursorChange'. [X] Platform: Clipboard support. [X] Platform: Keyboard
//  arrays indexed using SDL_SCANCODE_* codes, e.g. ANCHOR::IsKeyPressed(SDL_SCANCODE_SPACE). [X]
//  Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ANCHORConfigFlags_NavEnableGamepad'.
// Missing features:
//  [ ] Platform: SDL2 handling of IME under Windows appears to be broken and it explicitly disable
//  the regular Windows IME. You can restore Windows IME by compiling SDL with
//  SDL_DISABLE_WINDOWS_IME.

// You can use unmodified anchor_impl_* files in your project. See examples/ folder for examples of
// using this. Prefer including the entire anchor/ repository into your project (either as a copy
// or as a submodule), and only build the backends you need. If you are new to ANCHOR, read
// documentation from the docs/ folder + read the top of anchor.cpp. Read online:
// https://github.com/ocornut/anchor/tree/master/docs

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2021-03-22: Rework global mouse pos availability check listing supported platforms explicitly,
//  effectively fixing mouse access on Raspberry Pi. (#2837, #3950) 2020-05-25: Misc: Report a zero
//  display-size when window is minimized, to be consistent with other backends. 2020-02-20:
//  Inputs: Fixed mapping for ANCHOR_Key_KeyPadEnter (using SDL_SCANCODE_KP_ENTER instead of
//  SDL_SCANCODE_RETURN2). 2019-12-17: Inputs: On Wayland, use SDL_GetMouseState (because there is
//  no global mouse state). 2019-12-05: Inputs: Added support for ANCHOR_MouseCursor_NotAllowed
//  mouse cursor. 2019-07-21: Inputs: Added mapping for ANCHOR_Key_KeyPadEnter. 2019-04-23: Inputs:
//  Added support for SDL_GameController (if ANCHORConfigFlags_NavEnableGamepad is set by user
//  application). 2019-03-12: Misc: Preserve DisplayFramebufferScale when main window is minimized.
//  2018-12-21: Inputs: Workaround for Android/iOS which don't seem to handle focus related calls.
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-11-14: Changed the signature of ANCHOR_ImplSDL2_ProcessEvent() to take a 'const
//  SDL_Event*'. 2018-08-01: Inputs: Workaround for Emscripten which doesn't seem to handle focus
//  related calls. 2018-06-29: Inputs: Added support for the ANCHOR_MouseCursor_Hand cursor.
//  2018-06-08: Misc: Extracted anchor_impl_sdl.cpp/.h away from the old combined
//  SDL2+OpenGL/Vulkan examples. 2018-06-08: Misc: ANCHOR_ImplSDL2_InitForOpenGL() now takes a
//  SDL_GLContext parameter. 2018-05-09: Misc: Fixed clipboard paste memory leak (we didn't call
//  SDL_FreeMemory on the data returned by SDL_GetClipboardText). 2018-03-20: Misc: Setup
//  io.BackendFlags ANCHORBackendFlags_HasMouseCursors flag + honor
//  ANCHORConfigFlags_NoMouseCursorChange flag. 2018-02-16: Inputs: Added support for mouse
//  cursors, honoring ANCHOR::GetMouseCursor() value. 2018-02-06: Misc: Removed call to
//  ANCHOR::Shutdown() which is not available from 1.60 WIP, user needs to call
//  CreateContext/DestroyContext themselves. 2018-02-06: Inputs: Added mapping for
//  ANCHOR_Key_Space. 2018-02-05: Misc: Using SDL_GetPerformanceCounter() instead of SDL_GetTicks()
//  to be able to handle very high framerate (1000+ FPS). 2018-02-05: Inputs: Keyboard mapping is
//  using scancodes everywhere instead of a confusing mixture of keycodes and scancodes.
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-19: Inputs: When available (SDL 2.0.4+) using SDL_CaptureMouse() to retrieve
//  coordinates outside of client area when dragging. Otherwise (SDL 2.0.3 and before) testing for
//  SDL_WINDOW_INPUT_FOCUS instead of SDL_WINDOW_MOUSE_FOCUS. 2018-01-18: Inputs: Added mapping for
//  ANCHOR_Key_Insert. 2017-08-25: Inputs: MousePos set to -FLT_MAX,-FLT_MAX when mouse is
//  unavailable/missing (instead of -1,-1). 2016-10-15: Misc: Added a void* user_data parameter to
//  Clipboard function handlers.

#include "ANCHOR_BACKEND_sdl.h"
#include "ANCHOR_BACKEND_vulkan.h"

#include "ANCHOR_api.h"
#include "ANCHOR_debug_codes.h"
#include "ANCHOR_window_manager.h"

// SDL
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>
#if defined(__APPLE__)
#  include "TargetConditionals.h"
#endif

#include <wabi/base/arch/systemInfo.h>
#include <wabi/imaging/hgiVulkan/diagnostic.h>
#include <wabi/imaging/hgiVulkan/hgi.h>
#include <wabi/imaging/hgiVulkan/instance.h>

WABI_NAMESPACE_USING

#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE SDL_VERSION_ATLEAST(2, 0, 4)
#define SDL_HAS_VULKAN SDL_VERSION_ATLEAST(2, 0, 6)

/* clang-format off */

/**
 * PIXAR HYDRA GRAPHICS INTERFACE. */
static HgiVulkan         *g_PixarHydra      = nullptr;
static HgiVulkanInstance *g_PixarVkInstance = nullptr;

/**
 * HYDRA RENDERING PARAMS. */
static UsdApolloRenderParams g_HDPARAMS_Apollo;

/**
 * ANCHOR VULKAN GLOBALS. */
static VkAllocationCallbacks   *g_Allocator      = NULL;
static VkInstance               g_Instance       = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device         = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily    = (uint32_t)-1;
static VkQueue                  g_Queue          = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport    = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache  = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ANCHOR_VulkanGPU_Surface g_MainWindowData;
static uint32_t                  g_MinImageCount    = 2;
static bool                      g_SwapChainRebuild = false;

/**
 * SDL SYSTEM GLOBALS. */
static SDL_Window *g_Window                                 = NULL;
static Uint64 g_Time                                        = 0;
static bool g_MousePressed[3]                               = {false, false, false};
static SDL_Cursor *g_MouseCursors[ANCHOR_MouseCursor_COUNT] = {};
static char *g_ClipboardTextData                            = NULL;
static bool g_MouseCanUseGlobalState                        = true;

/* clang-format on */

static const char *ANCHOR_ImplSDL2_GetClipboardText(void *)
{
  if (g_ClipboardTextData)
    SDL_free(g_ClipboardTextData);
  g_ClipboardTextData = SDL_GetClipboardText();
  return g_ClipboardTextData;
}

static void ANCHOR_ImplSDL2_SetClipboardText(void *, const char *text)
{
  SDL_SetClipboardText(text);
}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if ANCHOR wants to
// use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main
// application. Generally you may always pass all inputs to ANCHOR, and hide them from your
// application based on those two flags. If you have multiple SDL events and some of them are not
// meant to be used by ANCHOR, you may need to filter events based on their windowID field.
bool ANCHOR_SystemSDL::ANCHOR_ImplSDL2_ProcessEvent(const SDL_Event *event)
{
  ANCHOR_IO &io = ANCHOR::GetIO();
  switch (event->type) {
    case SDL_MOUSEWHEEL: {
      if (event->wheel.x > 0)
        io.MouseWheelH += 1;
      if (event->wheel.x < 0)
        io.MouseWheelH -= 1;
      if (event->wheel.y > 0)
        io.MouseWheel += 1;
      if (event->wheel.y < 0)
        io.MouseWheel -= 1;
      return true;
    }
    case SDL_MOUSEBUTTONDOWN: {
      if (event->button.button == SDL_BUTTON_LEFT)
        g_MousePressed[0] = true;
      if (event->button.button == SDL_BUTTON_RIGHT)
        g_MousePressed[1] = true;
      if (event->button.button == SDL_BUTTON_MIDDLE)
        g_MousePressed[2] = true;
      return true;
    }
    case SDL_TEXTINPUT: {
      io.AddInputCharactersUTF8(event->text.text);
      return true;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      int key = event->key.keysym.scancode;
      ANCHOR_ASSERT(key >= 0 && key < ANCHOR_ARRAYSIZE(io.KeysDown));
      io.KeysDown[key] = (event->type == SDL_KEYDOWN);
      io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
      io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
      io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
      io.KeySuper = false;
#else
      io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
      return true;
    }
  }
  return false;
}

static bool ANCHOR_ImplSDL2_Init(SDL_Window *window)
{
  g_Window = window;

  // Setup backend capabilities flags
  ANCHOR_IO &io = ANCHOR::GetIO();
  io.BackendFlags |= ANCHORBackendFlags_HasMouseCursors;  // We can honor GetMouseCursor() values (optional)
  io.BackendFlags |= ANCHORBackendFlags_HasSetMousePos;   // We can honor io.WantSetMousePos
                                                          // requests (optional, rarely used)
  io.BackendPlatformName = "ANCHOR_BACKEND_sdl";

  // Keyboard mapping. ANCHOR will use those indices to peek into the io.KeysDown[] array.
  io.KeyMap[ANCHOR_Key_Tab] = SDL_SCANCODE_TAB;
  io.KeyMap[ANCHOR_Key_LeftArrow] = SDL_SCANCODE_LEFT;
  io.KeyMap[ANCHOR_Key_RightArrow] = SDL_SCANCODE_RIGHT;
  io.KeyMap[ANCHOR_Key_UpArrow] = SDL_SCANCODE_UP;
  io.KeyMap[ANCHOR_Key_DownArrow] = SDL_SCANCODE_DOWN;
  io.KeyMap[ANCHOR_Key_PageUp] = SDL_SCANCODE_PAGEUP;
  io.KeyMap[ANCHOR_Key_PageDown] = SDL_SCANCODE_PAGEDOWN;
  io.KeyMap[ANCHOR_Key_Home] = SDL_SCANCODE_HOME;
  io.KeyMap[ANCHOR_Key_End] = SDL_SCANCODE_END;
  io.KeyMap[ANCHOR_Key_Insert] = SDL_SCANCODE_INSERT;
  io.KeyMap[ANCHOR_Key_Delete] = SDL_SCANCODE_DELETE;
  io.KeyMap[ANCHOR_Key_Backspace] = SDL_SCANCODE_BACKSPACE;
  io.KeyMap[ANCHOR_Key_Space] = SDL_SCANCODE_SPACE;
  io.KeyMap[ANCHOR_Key_Enter] = SDL_SCANCODE_RETURN;
  io.KeyMap[ANCHOR_Key_Escape] = SDL_SCANCODE_ESCAPE;
  io.KeyMap[ANCHOR_Key_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
  io.KeyMap[ANCHOR_Key_A] = SDL_SCANCODE_A;
  io.KeyMap[ANCHOR_Key_C] = SDL_SCANCODE_C;
  io.KeyMap[ANCHOR_Key_V] = SDL_SCANCODE_V;
  io.KeyMap[ANCHOR_Key_X] = SDL_SCANCODE_X;
  io.KeyMap[ANCHOR_Key_Y] = SDL_SCANCODE_Y;
  io.KeyMap[ANCHOR_Key_Z] = SDL_SCANCODE_Z;

  io.SetClipboardTextFn = ANCHOR_ImplSDL2_SetClipboardText;
  io.GetClipboardTextFn = ANCHOR_ImplSDL2_GetClipboardText;
  io.ClipboardUserData = NULL;

  // Load mouse cursors
  g_MouseCursors[ANCHOR_MouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  g_MouseCursors[ANCHOR_MouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
  g_MouseCursors[ANCHOR_MouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
  g_MouseCursors[ANCHOR_MouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
  g_MouseCursors[ANCHOR_MouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
  g_MouseCursors[ANCHOR_MouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
  g_MouseCursors[ANCHOR_MouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
  g_MouseCursors[ANCHOR_MouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  g_MouseCursors[ANCHOR_MouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

  // Check and store if we are on a SDL backend that supports global mouse position
  // ("wayland" and "rpi" don't support it, but we chose to use a white-list instead of a
  // black-list)
  const char *sdl_backend = SDL_GetCurrentVideoDriver();
  const char *global_mouse_whitelist[] = {"windows", "cocoa", "x11", "DIVE", "VMAN"};
  g_MouseCanUseGlobalState = false;
  for (int n = 0; n < ANCHOR_ARRAYSIZE(global_mouse_whitelist); n++)
    if (strncmp(sdl_backend, global_mouse_whitelist[n], strlen(global_mouse_whitelist[n])) == 0)
      g_MouseCanUseGlobalState = true;

#ifdef _WIN32
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window, &wmInfo);
  io.ImeWindowHandle = wmInfo.info.win.window;
#else
  (void)window;
#endif

  return true;
}

bool ANCHOR_SystemSDL::ANCHOR_ImplSDL2_InitForOpenGL(SDL_Window *window, void *sdl_gl_context)
{
  (void)sdl_gl_context;  // Viewport branch will need this.
  return ANCHOR_ImplSDL2_Init(window);
}

bool ANCHOR_SystemSDL::ANCHOR_ImplSDL2_InitForVulkan(SDL_Window *window)
{
#if !SDL_HAS_VULKAN
  ANCHOR_ASSERT(0 && "Unsupported");
#endif
  return ANCHOR_ImplSDL2_Init(window);
}

bool ANCHOR_SystemSDL::ANCHOR_ImplSDL2_InitForD3D(SDL_Window *window)
{
#if !defined(_WIN32)
  ANCHOR_ASSERT(0 && "Unsupported");
#endif
  return ANCHOR_ImplSDL2_Init(window);
}

bool ANCHOR_SystemSDL::ANCHOR_ImplSDL2_InitForMetal(SDL_Window *window)
{
  return ANCHOR_ImplSDL2_Init(window);
}

void ANCHOR_SystemSDL::ANCHOR_ImplSDL2_Shutdown()
{
  g_Window = NULL;

  // Destroy last known clipboard data
  if (g_ClipboardTextData)
    SDL_free(g_ClipboardTextData);
  g_ClipboardTextData = NULL;

  // Destroy SDL mouse cursors
  for (ANCHOR_MouseCursor cursor_n = 0; cursor_n < ANCHOR_MouseCursor_COUNT; cursor_n++)
    SDL_FreeCursor(g_MouseCursors[cursor_n]);
  memset(g_MouseCursors, 0, sizeof(g_MouseCursors));
}

ANCHOR_DisplayManagerSDL::ANCHOR_DisplayManagerSDL(ANCHOR_SystemSDL *system)
  : ANCHOR_DisplayManager(),
    m_system(system)
{
  memset(&m_mode, 0, sizeof(m_mode));
}

eAnchorStatus ANCHOR_DisplayManagerSDL::getNumDisplays(AnchorU8 &numDisplays) const
{
  numDisplays = SDL_GetNumVideoDisplays();
  return ANCHOR_SUCCESS;
}

eAnchorStatus ANCHOR_DisplayManagerSDL::getNumDisplaySettings(AnchorU8 display, AnchorS32 &numSettings) const
{
  ANCHOR_ASSERT(display < 1);

  numSettings = SDL_GetNumDisplayModes(display - 1);

  return ANCHOR_SUCCESS;
}

static void anchor_mode_from_sdl(ANCHOR_DisplaySetting &setting, SDL_DisplayMode *mode)
{
  setting.xPixels = mode->w;
  setting.yPixels = mode->h;
  setting.bpp = SDL_BYTESPERPIXEL(mode->format) * 8;
  /* Just guess the frequency :( */
  setting.frequency = mode->refresh_rate ? mode->refresh_rate : 60;
}

static void anchor_mode_to_sdl(const ANCHOR_DisplaySetting &setting, SDL_DisplayMode *mode)
{
  mode->w = setting.xPixels;
  mode->h = setting.yPixels;
  // setting.bpp = SDL_BYTESPERPIXEL(mode->format) * 8; ???
  mode->refresh_rate = setting.frequency;
}

eAnchorStatus ANCHOR_DisplayManagerSDL::getDisplaySetting(AnchorU8 display,
                                                          AnchorS32 index,
                                                          ANCHOR_DisplaySetting &setting) const
{
  ANCHOR_ASSERT(display < 1);

  SDL_DisplayMode mode;
  SDL_GetDisplayMode(display, index, &mode);

  anchor_mode_from_sdl(setting, &mode);

  return ANCHOR_SUCCESS;
}

eAnchorStatus ANCHOR_DisplayManagerSDL::getCurrentDisplaySetting(AnchorU8 display,
                                                                 ANCHOR_DisplaySetting &setting) const
{
  SDL_DisplayMode mode;
  SDL_GetCurrentDisplayMode(display, &mode);

  anchor_mode_from_sdl(setting, &mode);

  return ANCHOR_SUCCESS;
}

eAnchorStatus ANCHOR_DisplayManagerSDL::getCurrentDisplayModeSDL(SDL_DisplayMode &mode) const
{
  mode = m_mode;
  return ANCHOR_SUCCESS;
}

eAnchorStatus ANCHOR_DisplayManagerSDL::setCurrentDisplaySetting(AnchorU8 display,
                                                                 const ANCHOR_DisplaySetting &setting)
{
  /*
   * Mode switching code ported from Quake 2 version 3.21 and bzflag version
   * 2.4.0:
   * ftp://ftp.idsoftware.com/idstuff/source/q2source-3.21.zip
   * See linux/gl_glx.c:GLimp_SetMode
   * http://wiki.bzflag.org/BZFlag_Source
   * See src/platform/SDLDisplay.cxx:SDLDisplay and createWindow */
  SDL_DisplayMode mode;
  const int num_modes = SDL_GetNumDisplayModes(display);
  int best_fit, best_dist, dist, x, y;

  best_dist = 9999999;
  best_fit = -1;

  if (num_modes == 0) {
    /* Any mode is OK. */
    anchor_mode_to_sdl(setting, &mode);
  }
  else {
    for (int i = 0; i < num_modes; i++) {

      SDL_GetDisplayMode(display, i, &mode);

      if (setting.xPixels > mode.w || setting.yPixels > mode.h) {
        continue;
      }

      x = setting.xPixels - mode.w;
      y = setting.yPixels - mode.h;
      dist = (x * x) + (y * y);
      if (dist < best_dist) {
        best_dist = dist;
        best_fit = i;
      }
    }

    if (best_fit == -1)
      return ANCHOR_ERROR;

    SDL_GetDisplayMode(display, best_fit, &mode);
  }

  m_mode = mode;

  /* evil, SDL2 needs a window to adjust display modes */
  ANCHOR_WindowSDL *win = (ANCHOR_WindowSDL *)m_system->getWindowManager()->getActiveWindow();

  if (win) {
    SDL_Window *sdl_win = win->getSDLWindow();

    SDL_SetWindowDisplayMode(sdl_win, &mode);
    SDL_ShowWindow(sdl_win);
    SDL_SetWindowFullscreen(sdl_win, SDL_TRUE);

    return ANCHOR_SUCCESS;
  }
  else {
    TF_CODING_ERROR("No windows available, can't fullscreen.\n");

    /* do not fail, we will try again later when the window is created - wander */
    return ANCHOR_SUCCESS;
  }
}

static void ANCHOR_ImplSDL2_UpdateMousePosAndButtons()
{
  ANCHOR_IO &io = ANCHOR::GetIO();

  // Set OS mouse position if requested (rarely used, only when
  // ANCHORConfigFlags_NavEnableSetMousePos is enabled by user)
  if (io.WantSetMousePos)
    SDL_WarpMouseInWindow(g_Window, (int)io.MousePos[0], (int)io.MousePos[1]);
  else
    io.MousePos = GfVec2f(-FLT_MAX, -FLT_MAX);

  int mx, my;
  Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
  io.MouseDown[0] = g_MousePressed[0] ||
                    (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) !=
                      0;  // If a mouse press event came, always pass it as "mouse held this frame", so we
                          // don't miss click-release events that are shorter than 1 frame.
  io.MouseDown[1] = g_MousePressed[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
  io.MouseDown[2] = g_MousePressed[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
  g_MousePressed[0] = g_MousePressed[1] = g_MousePressed[2] = false;

#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && \
  !(defined(__APPLE__) && TARGET_OS_IOS)
  SDL_Window *focused_window = SDL_GetKeyboardFocus();
  if (g_Window == focused_window) {
    if (g_MouseCanUseGlobalState) {
      // SDL_GetMouseState() gives mouse position seemingly based on the last window
      // entered/focused(?) The creation of a new windows at runtime and SDL_CaptureMouse both
      // seems to severely mess up with that, so we retrieve that position globally. Won't use this
      // workaround on SDL backends that have no global mouse position, like Wayland or RPI
      int wx, wy;
      SDL_GetWindowPosition(focused_window, &wx, &wy);
      SDL_GetGlobalMouseState(&mx, &my);
      mx -= wx;
      my -= wy;
    }
    io.MousePos = GfVec2f((float)mx, (float)my);
  }

  // SDL_CaptureMouse() let the OS know e.g. that our anchor drag outside the SDL window boundaries
  // shouldn't e.g. trigger the OS window resize cursor. The function is only supported from
  // SDL 2.0.4 (released Jan 2016)
  bool any_mouse_button_down = ANCHOR::IsAnyMouseDown();
  SDL_CaptureMouse(any_mouse_button_down ? SDL_TRUE : SDL_FALSE);
#else
  if (SDL_GetWindowFlags(g_Window) & SDL_WINDOW_INPUT_FOCUS)
    io.MousePos = GfVec2f((float)mx, (float)my);
#endif
}

static void ANCHOR_ImplSDL2_UpdateMouseCursor()
{
  ANCHOR_IO &io = ANCHOR::GetIO();
  if (io.ConfigFlags & ANCHORConfigFlags_NoMouseCursorChange)
    return;

  ANCHOR_MouseCursor anchor_cursor = ANCHOR::GetMouseCursor();
  if (io.MouseDrawCursor || anchor_cursor == ANCHOR_MouseCursor_None) {
    // Hide OS mouse cursor if anchor is drawing it or if it wants no cursor
    SDL_ShowCursor(SDL_FALSE);
  }
  else {
    // Show OS mouse cursor
    SDL_SetCursor(g_MouseCursors[anchor_cursor] ? g_MouseCursors[anchor_cursor] :
                                                  g_MouseCursors[ANCHOR_MouseCursor_Arrow]);
    SDL_ShowCursor(SDL_TRUE);
  }
}

static void ANCHOR_ImplSDL2_UpdateGamepads()
{
  ANCHOR_IO &io = ANCHOR::GetIO();
  memset(io.NavInputs, 0, sizeof(io.NavInputs));
  if ((io.ConfigFlags & ANCHORConfigFlags_NavEnableGamepad) == 0)
    return;

  // Get gamepad
  SDL_GameController *game_controller = SDL_GameControllerOpen(0);
  if (!game_controller) {
    io.BackendFlags &= ~ANCHORBackendFlags_HasGamepad;
    return;
  }

// Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO) \
  { \
    io.NavInputs[NAV_NO] = (SDL_GameControllerGetButton(game_controller, BUTTON_NO) != 0) ? 1.0f : 0.0f; \
  }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) \
  { \
    float vn = (float)(SDL_GameControllerGetAxis(game_controller, AXIS_NO) - V0) / (float)(V1 - V0); \
    if (vn > 1.0f) \
      vn = 1.0f; \
    if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) \
      io.NavInputs[NAV_NO] = vn; \
  }
  const int thumb_dead_zone = 8000;  // SDL_gamecontroller.h suggests using this value.
  MAP_BUTTON(ANCHOR_NavInput_Activate, SDL_CONTROLLER_BUTTON_A);               // Cross / A
  MAP_BUTTON(ANCHOR_NavInput_Cancel, SDL_CONTROLLER_BUTTON_B);                 // Circle / B
  MAP_BUTTON(ANCHOR_NavInput_Menu, SDL_CONTROLLER_BUTTON_X);                   // Square / X
  MAP_BUTTON(ANCHOR_NavInput_Input, SDL_CONTROLLER_BUTTON_Y);                  // Triangle / Y
  MAP_BUTTON(ANCHOR_NavInput_DpadLeft, SDL_CONTROLLER_BUTTON_DPAD_LEFT);       // D-Pad Left
  MAP_BUTTON(ANCHOR_NavInput_DpadRight, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);     // D-Pad Right
  MAP_BUTTON(ANCHOR_NavInput_DpadUp, SDL_CONTROLLER_BUTTON_DPAD_UP);           // D-Pad Up
  MAP_BUTTON(ANCHOR_NavInput_DpadDown, SDL_CONTROLLER_BUTTON_DPAD_DOWN);       // D-Pad Down
  MAP_BUTTON(ANCHOR_NavInput_FocusPrev, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);   // L1 / LB
  MAP_BUTTON(ANCHOR_NavInput_FocusNext, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);  // R1 / RB
  MAP_BUTTON(ANCHOR_NavInput_TweakSlow, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);   // L1 / LB
  MAP_BUTTON(ANCHOR_NavInput_TweakFast, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);  // R1 / RB
  MAP_ANALOG(ANCHOR_NavInput_LStickLeft, SDL_CONTROLLER_AXIS_LEFTX, -thumb_dead_zone, -32768);
  MAP_ANALOG(ANCHOR_NavInput_LStickRight, SDL_CONTROLLER_AXIS_LEFTX, +thumb_dead_zone, +32767);
  MAP_ANALOG(ANCHOR_NavInput_LStickUp, SDL_CONTROLLER_AXIS_LEFTY, -thumb_dead_zone, -32767);
  MAP_ANALOG(ANCHOR_NavInput_LStickDown, SDL_CONTROLLER_AXIS_LEFTY, +thumb_dead_zone, +32767);

  io.BackendFlags |= ANCHORBackendFlags_HasGamepad;
#undef MAP_BUTTON
#undef MAP_ANALOG
}

void ANCHOR_SystemSDL::ANCHOR_ImplSDL2_NewFrame(SDL_Window *window)
{
  ANCHOR_IO &io = ANCHOR::GetIO();
  ANCHOR_ASSERT(io.Fonts->IsBuilt() &&
                "Font atlas not built! It is generally built by the renderer backend. Missing "
                "call to renderer _NewFrame() function? e.g. ANCHOR_ImplOpenGL3_NewFrame().");

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  SDL_GetWindowSize(window, &w, &h);
  if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
    w = h = 0;
  SDL_GL_GetDrawableSize(window, &display_w, &display_h);
  io.DisplaySize = GfVec2f((float)w, (float)h);
  if (w > 0 && h > 0)
    io.DisplayFramebufferScale = GfVec2f((float)display_w / w, (float)display_h / h);

  // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
  static Uint64 frequency = SDL_GetPerformanceFrequency();
  Uint64 current_time = SDL_GetPerformanceCounter();
  io.DeltaTime = g_Time > 0 ? (float)((double)(current_time - g_Time) / frequency) : (float)(1.0f / 60.0f);
  g_Time = current_time;

  ANCHOR_ImplSDL2_UpdateMousePosAndButtons();
  ANCHOR_ImplSDL2_UpdateMouseCursor();

  // Update game controllers (if enabled and available)
  ANCHOR_ImplSDL2_UpdateGamepads();
}

ANCHOR_WindowSDL::ANCHOR_WindowSDL(ANCHOR_SystemSDL *system,
                                   const char *title,
                                   const char *icon,
                                   AnchorS32 left,
                                   AnchorS32 top,
                                   AnchorU32 width,
                                   AnchorU32 height,
                                   eAnchorWindowState state,
                                   eAnchorDrawingContextType type,
                                   const bool stereoVisual,
                                   const bool exclusive,
                                   const ANCHOR_ISystemWindow *parentWindow)
  : ANCHOR_SystemWindow(width, height, state, stereoVisual, exclusive),
    m_system(system),
    m_valid_setup(false),
    m_invalid_window(false),
    m_sdl_custom_cursor(NULL)
{
  /* creating the window _must_ come after setting attributes */
  m_sdl_win = SDL_CreateWindow(
    title, left, top, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);

  /* now set up the rendering context. */
  if (setDrawingContextType(type) == ANCHOR_SUCCESS) {
    m_valid_setup = true;
  }

  if (exclusive) {
    SDL_RaiseWindow(m_sdl_win);
  }

  setTitle(title);
  setIcon(icon);
}

static void check_vk_result(VkResult err)
{
  if (err == 0)
    return;
  TF_CODING_ERROR("[vulkan] Error: VkResult = %d\n", err);
  if (err < 0)
    abort();
}

/**
 * Main Vulkan Setup and Instantiation Routine. ------------------------------ */
static void SetupVulkan(const char **extensions, uint32_t extensions_count)
{
  VkResult err;

  /**
   * Create Vulkan Instance. */
  {
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    /**
     * OpenGL Interop :: Memory */
    const char **ext_gl_mem = (const char **)malloc(sizeof(const char *) * (extensions_count + 1));
    memcpy(ext_gl_mem, extensions, extensions_count * sizeof(const char *));
    ext_gl_mem[extensions_count] = VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME;
    extensions = ext_gl_mem;
    extensions_count += 1;

    /**
     * OpenGL Interop :: Semaphore */
    const char **ext_gl_sem = (const char **)malloc(sizeof(const char *) * (extensions_count + 1));
    memcpy(ext_gl_sem, extensions, extensions_count * sizeof(const char *));
    ext_gl_sem[extensions_count] = VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME;
    extensions = ext_gl_sem;
    extensions_count += 1;

    /**
     * Physical Device Properties 2 */
    const char **ext_dprops_2 = (const char **)malloc(sizeof(const char *) * (extensions_count + 1));
    memcpy(ext_dprops_2, extensions, extensions_count * sizeof(const char *));
    ext_dprops_2[extensions_count] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
    extensions = ext_dprops_2;
    extensions_count += 1;

    create_info.enabledExtensionCount = extensions_count;
    create_info.ppEnabledExtensionNames = extensions;

    if (HgiVulkanIsDebugEnabled()) {
      /**
       * Enable validation layers. */
      const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
      create_info.enabledLayerCount = 1;
      create_info.ppEnabledLayerNames = layers;

      /**
       * Enable debug report extension (we need additional storage,
       * so we duplicate the user array to add our new extension to it). */
      const char **extensions_ext = (const char **)malloc(sizeof(const char *) * (extensions_count + 1));
      memcpy(extensions_ext, extensions, extensions_count * sizeof(const char *));
      extensions_ext[extensions_count] = "VK_EXT_debug_utils";
      create_info.enabledExtensionCount = extensions_count + 1;
      create_info.ppEnabledExtensionNames = extensions_ext;

      /**
       * Create Vulkan Instance with debug features. */
      err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
      g_PixarHydra = new HgiVulkan(g_PixarVkInstance = new HgiVulkanInstance(g_Instance));
      check_vk_result(err);
      free(extensions_ext);
    }
    else {

      /**
       * Create Vulkan Instance without any debug features. */
      err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
      g_PixarHydra = new HgiVulkan(g_PixarVkInstance = new HgiVulkanInstance(g_Instance));
      check_vk_result(err);
    }

    TF_UNUSED(g_DebugReport);

    free(ext_gl_mem);
    free(ext_gl_sem);
    free(ext_dprops_2);
  }

  /**
   * Select GPU. */
  {
    uint32_t gpu_count;
    err = vkEnumeratePhysicalDevices(g_PixarVkInstance->GetVulkanInstance(), &gpu_count, NULL);
    check_vk_result(err);
    ANCHOR_ASSERT(gpu_count > 0);

    VkPhysicalDevice *gpus = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * gpu_count);
    err = vkEnumeratePhysicalDevices(g_PixarVkInstance->GetVulkanInstance(), &gpu_count, gpus);
    check_vk_result(err);

    /**
     * If a number >1 of GPUs got reported, find discrete GPU if present,
     * or use first one available. This covers most common cases (multi-gpu
     * & integrated+dedicated graphics).
     *
     * TODO: Handle more complicated setups (multiple dedicated GPUs). */
    int use_gpu = 0;
    for (int i = 0; i < (int)gpu_count; i++) {

      VkPhysicalDeviceProperties properties;
      vkGetPhysicalDeviceProperties(gpus[i], &properties);
      if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        use_gpu = i;
        break;
      }
    }

    g_PhysicalDevice = gpus[use_gpu];
    free(gpus);
  }

  /**
   * Select graphics queue family. */
  {
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
    VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) *
                                                                        count);
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
    for (uint32_t i = 0; i < count; i++)
      if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        g_QueueFamily = i;
        break;
      }
    free(queues);
    ANCHOR_ASSERT(g_QueueFamily != (uint32_t)-1);
  }

  /**
   * Create Logical Device (with 1 queue). */
  {
    int device_extension_count = 1;
    const char *device_extensions[] = {"VK_KHR_swapchain"};
    const float queue_priority[] = {1.0f};
    VkDeviceQueueCreateInfo queue_info[1] = {};
    queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[0].queueFamilyIndex = g_QueueFamily;
    queue_info[0].queueCount = 1;
    queue_info[0].pQueuePriorities = queue_priority;
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
    create_info.pQueueCreateInfos = queue_info;
    create_info.enabledExtensionCount = device_extension_count;
    create_info.ppEnabledExtensionNames = device_extensions;
    err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
    check_vk_result(err);
    vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
  }

  /**
   * Create Descriptor Pool. */
  {
    /* clang-format off */
    VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };
    /* clang-format on */
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * ANCHOR_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)ANCHOR_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
    check_vk_result(err);
  }
}

static void SetFont()
{
  ANCHOR_IO &io = ANCHOR::GetIO();
  io.Fonts->AddFontDefault();

  const static std::string exe_path = TfGetPathName(ArchGetExecutablePath());

  /* Gotham Font. */
  const static std::string gm_ttf("../datafiles/fonts/GothamPro.ttf");
  const static char *gm_path = TfStringCatPaths(exe_path, gm_ttf).c_str();
  io.Fonts->AddFontFromFileTTF(gm_path, 11.0f);

  /* Dankmono Font. */
  const static std::string dm_ttf("../datafiles/fonts/dankmono.ttf");
  const static char *dm_path = TfStringCatPaths(exe_path, dm_ttf).c_str();
  io.Fonts->AddFontFromFileTTF(dm_path, 12.0f);

  /* San Francisco Font (Default). */
  const static std::string sf_ttf("../datafiles/fonts/SFProText-Medium.ttf");
  const static char *sf_path = TfStringCatPaths(exe_path, sf_ttf).c_str();
  io.FontDefault = io.Fonts->AddFontFromFileTTF(sf_path, 14.0f);
}

/**
 * Display Vulkan Debug Report. */
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags,
                                                   VkDebugReportObjectTypeEXT objectType,
                                                   uint64_t object,
                                                   size_t location,
                                                   int32_t messageCode,
                                                   const char *pLayerPrefix,
                                                   const char *pMessage,
                                                   void *pUserData)
{
  const char *type = (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) ? "VULKAN_ERROR" : "VULKAN_MESSAGE";
  TF_UNUSED(object);
  TF_UNUSED(location);
  TF_UNUSED(messageCode);
  TF_UNUSED(pUserData);
  TF_UNUSED(pLayerPrefix);

  TF_WARN("[vulkan] Debug report from ObjectType: %s\nMessage: %s\n\n", type, pMessage);
  return VK_FALSE;
}

/**
 * Main Application Window. --------------------------------- */
static void SetupVulkanWindow(ANCHOR_VulkanGPU_Surface *wd, VkSurfaceKHR surface, int width, int height)
{
  wd->Surface = surface;

  /**
   * Check for WSI support. */
  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
  if (res != VK_TRUE) {
    fprintf(stderr, "Error no WSI support on physical device 0\n");
    exit(-1);
  }

  /**
   * Select Surface Format. */
  /* clang-format off */
  const VkFormat requestSurfaceImageFormat[] = {
    VK_FORMAT_B8G8R8A8_UNORM,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_B8G8R8_UNORM,
    VK_FORMAT_R8G8B8_UNORM
  };
  /* clang-format on */
  const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

  wd->SurfaceFormat = ANCHOR_ImplVulkanH_SelectSurfaceFormat(
    g_PhysicalDevice,
    wd->Surface,
    requestSurfaceImageFormat,
    (size_t)ANCHOR_ARRAYSIZE(requestSurfaceImageFormat),
    requestSurfaceColorSpace);

  /**
   * Render at maximum possible FPS. */
  if (HgiVulkanIsMaxFPSEnabled()) {

    TF_DEBUG(ANCHOR_SDL_VULKAN).Msg("[Anchor] Rendering at maximum possible frames per second.\n");

    /* clang-format off */
    VkPresentModeKHR present_modes[] = {
      /** Removes screen tearing. */
      VK_PRESENT_MODE_MAILBOX_KHR,
      /** Present frames immediately. */
      VK_PRESENT_MODE_IMMEDIATE_KHR,
      /** Required for presentation. */
      VK_PRESENT_MODE_FIFO_KHR
    };
    /* clang-format on */

    wd->PresentMode = ANCHOR_ImplVulkanH_SelectPresentMode(
      g_PhysicalDevice, wd->Surface, &present_modes[0], ANCHOR_ARRAYSIZE(present_modes));
  }
  else { /** Throttled FPS ~75FPS */

    TF_DEBUG(ANCHOR_SDL_VULKAN).Msg("[Anchor] Throttled maximum frames per second.\n");

    /* clang-format off */
    VkPresentModeKHR present_modes[] = {
      /** Required for presentation. */
      VK_PRESENT_MODE_FIFO_KHR
    };
    /* clang-format on */

    wd->PresentMode = ANCHOR_ImplVulkanH_SelectPresentMode(
      g_PhysicalDevice, wd->Surface, &present_modes[0], ANCHOR_ARRAYSIZE(present_modes));
  }

  TF_DEBUG(ANCHOR_SDL_VULKAN).Msg("[Anchor] Selected PresentMode = %d\n", wd->PresentMode);

  /**
   * Create SwapChain, RenderPass, Framebuffer, etc. */
  ANCHOR_ASSERT(g_MinImageCount >= 2);
  ANCHOR_ImplVulkanH_CreateOrResizeWindow(g_PixarVkInstance->GetVulkanInstance(),
                                          g_PhysicalDevice,
                                          g_Device,
                                          wd,
                                          g_QueueFamily,
                                          g_Allocator,
                                          width,
                                          height,
                                          g_MinImageCount);
}

static void CleanupVulkan()
{
  vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

  // if (HgiVulkanIsDebugEnabled()) {
  //   /**
  //    * Remove the debug report callback. */
  //   auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)
  //       vkGetInstanceProcAddr(g_PixarVkInstance->GetVulkanInstance(),
  //       "vkDestroyDebugReportCallbackEXT");
  //   vkDestroyDebugReportCallbackEXT(g_PixarVkInstance->GetVulkanInstance(), g_DebugReport,
  //   g_Allocator);
  // }

  vkDestroyDevice(g_Device, g_Allocator);
  vkDestroyInstance(g_PixarVkInstance->GetVulkanInstance(), g_Allocator);
}

static void CleanupVulkanWindow()
{
  ANCHOR_ImplVulkanH_DestroyWindow(
    g_PixarVkInstance->GetVulkanInstance(), g_Device, &g_MainWindowData, g_Allocator);
}

static void FrameRender(ANCHOR_VulkanGPU_Surface *wd, ImDrawData *draw_data)
{
  VkResult err;

  VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  err = vkAcquireNextImageKHR(
    g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    g_SwapChainRebuild = true;
    return;
  }
  check_vk_result(err);

  ANCHOR_VulkanGPU_Frame *fd = &wd->Frames[wd->FrameIndex];
  {
    err = vkWaitForFences(g_Device,
                          1,
                          &fd->Fence,
                          VK_TRUE,
                          /* wait indefinitely==**/ UINT64_MAX);
    check_vk_result(err);

    err = vkResetFences(g_Device, 1, &fd->Fence);
    check_vk_result(err);
  }
  {
    err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    check_vk_result(err);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = wd->RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = wd->Width;
    info.renderArea.extent.height = wd->Height;
    info.clearValueCount = 1;
    info.pClearValues = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  /**
   * Record ANCHOR primitives into command buffer. */
  ANCHOR_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

  /**
   * Submit command buffer. */
  vkCmdEndRenderPass(fd->CommandBuffer);
  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result(err);
    err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
    check_vk_result(err);
  }
}

static void FramePresent(ANCHOR_VulkanGPU_Surface *wd)
{
  if (g_SwapChainRebuild)
    return;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &wd->Swapchain;
  info.pImageIndices = &wd->FrameIndex;
  VkResult err = vkQueuePresentKHR(g_Queue, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
    g_SwapChainRebuild = true;
    return;
  }
  check_vk_result(err);
  /**
   * Now we can use the next set of semaphores. */
  wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount;
}

ANCHOR_Context *ANCHOR_WindowSDL::newDrawingContext(eAnchorDrawingContextType type)
{
  if (type == ANCHOR_DrawingContextTypeVulkan) {

    /**
     * Setup Vulkan. */
    uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(m_sdl_win, &extensions_count, NULL);
    const char **extensions = new const char *[extensions_count];
    SDL_Vulkan_GetInstanceExtensions(m_sdl_win, &extensions_count, extensions);
    SetupVulkan(extensions, extensions_count);
    delete[] extensions;

    /**
     * Create Window Surface. */
    VkSurfaceKHR surface;
    if (SDL_Vulkan_CreateSurface(m_sdl_win, g_PixarVkInstance->GetVulkanInstance(), &surface) == 0) {
      TF_CODING_ERROR("Failed to create Vulkan surface.\n");
    }

    /**
     * Create Framebuffers. */
    int w, h;
    SDL_GetWindowSize(m_sdl_win, &w, &h);
    m_vulkan_context = &g_MainWindowData;
    SetupVulkanWindow(m_vulkan_context, surface, w, h);

    /**
     * Setup ANCHOR context. */
    ANCHOR_CHECKVERSION();
    ANCHOR::CreateContext();

    /**
     * Setup Keyboard & Gamepad controls. */
    ANCHOR_IO &io = ANCHOR::GetIO();
    io.ConfigFlags |= ANCHORConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ANCHORConfigFlags_NavEnableGamepad;

    /**
     * Setup Default Covah theme.
     *   Themes::
     *     - ANCHOR::StyleColorsDefault()
     *     - ANCHOR::StyleColorsLight()
     *     - ANCHOR::StyleColorsDark() */
    ANCHOR::StyleColorsDefault();

    /**
     * Setup Platform/Renderer backends. */
    ANCHOR_SystemSDL::ANCHOR_ImplSDL2_InitForVulkan(m_sdl_win);
    ANCHOR_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = g_PixarVkInstance->GetVulkanInstance();
    init_info.PhysicalDevice = g_PhysicalDevice;
    init_info.Device = g_Device;
    init_info.QueueFamily = g_QueueFamily;
    init_info.Queue = g_Queue;
    init_info.PipelineCache = g_PipelineCache;
    init_info.DescriptorPool = g_DescriptorPool;
    init_info.Allocator = g_Allocator;
    init_info.MinImageCount = g_MinImageCount;
    init_info.ImageCount = m_vulkan_context->ImageCount;
    init_info.CheckVkResultFn = check_vk_result;
    ANCHOR_ImplVulkan_Init(&init_info, m_vulkan_context->RenderPass);

    /**
     * Create Pixar Hydra Graphics Interface. */
    HdDriver driver;
    HgiUniquePtr hgi = HgiUniquePtr(g_PixarHydra);
    driver.name = HgiTokens->renderDriver;
    driver.driver = VtValue(hgi.get());

    /**
     * Setup Pixar Driver & Engine. */
    ANCHOR::GetPixarDriver().name = driver.name;
    ANCHOR::GetPixarDriver().driver = driver.driver;

    SetFont();

    /**
     * Upload Fonts. */
    {
      /**
       * Use any command queue. */
      VkCommandPool command_pool = m_vulkan_context->Frames[m_vulkan_context->FrameIndex].CommandPool;
      VkCommandBuffer command_buffer = m_vulkan_context->Frames[m_vulkan_context->FrameIndex].CommandBuffer;

      VkResult err = vkResetCommandPool(g_Device, command_pool, 0);
      check_vk_result(err);
      VkCommandBufferBeginInfo begin_info = {};
      begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      err = vkBeginCommandBuffer(command_buffer, &begin_info);
      check_vk_result(err);

      ANCHOR_ImplVulkan_CreateFontsTexture(command_buffer);

      VkSubmitInfo end_info = {};
      end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      end_info.commandBufferCount = 1;
      end_info.pCommandBuffers = &command_buffer;
      err = vkEndCommandBuffer(command_buffer);
      check_vk_result(err);
      err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
      check_vk_result(err);

      err = vkDeviceWaitIdle(g_Device);
      check_vk_result(err);
      ANCHOR_ImplVulkan_DestroyFontUploadObjects();
    }

    return ANCHOR::GetCurrentContext();
  }

  return NULL;
}

void ANCHOR_WindowSDL::setTitle(const char *title)
{
  SDL_SetWindowTitle(m_sdl_win, title);
}

void ANCHOR_WindowSDL::setIcon(const char *icon)
{
  SDL_SetWindowIcon(m_sdl_win, IMG_Load(icon));
}

ANCHOR_SystemSDL::ANCHOR_SystemSDL() : ANCHOR_System()
{
  /**
   * Setup SDL. */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    TF_CODING_ERROR("Error: %s\n", SDL_GetError());
    exit(ANCHOR_ERROR);
  }
}

ANCHOR_ISystemWindow *ANCHOR_SystemSDL::createWindow(
  const char *title,
  const char *icon,
  AnchorS32 left,
  AnchorS32 top,
  AnchorU32 width,
  AnchorU32 height,
  eAnchorWindowState state,
  eAnchorDrawingContextType type = ANCHOR_DrawingContextTypeNone,
  int vkSettings = 0,
  const bool exclusive,
  const bool /* is_dialog */,
  const ANCHOR_ISystemWindow *parentWindow)
{
  ANCHOR_WindowSDL *window = NULL;

  window = new ANCHOR_WindowSDL(
    this, title, icon, left, top, width, height, state, type, vkSettings, exclusive, parentWindow);

  if (window) {
    if (ANCHOR_WindowStateFullScreen == state) {
      SDL_Window *sdl_win = window->getSDLWindow();

      SDL_DisplayMode mode;
      SDL_SetWindowDisplayMode(sdl_win, &mode);
      SDL_ShowWindow(sdl_win);
      SDL_SetWindowFullscreen(sdl_win, SDL_TRUE);
    }

    if (window->getValid()) {
      // m_windowManager->addWindow(window);
      // pushEvent(new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowSize, window));
    }
    else {
      delete window;
      window = NULL;
    }
  }
  return window;
}

eAnchorStatus ANCHOR_SystemSDL::init()
{
  eAnchorStatus success = ANCHOR_System::init();

  if (success) {
    m_displayManager = new ANCHOR_DisplayManagerSDL(this);

    if (m_displayManager) {
      return ANCHOR_SUCCESS;
    }
  }

  return ANCHOR_ERROR;
}

bool ANCHOR_SystemSDL::generateWindowExposeEvents()
{
  // TODO
  // std::vector<ANCHOR_WindowSDL *>::iterator w_start = m_dirty_windows.begin();
  // std::vector<ANCHOR_WindowSDL *>::const_iterator w_end = m_dirty_windows.end();
  // bool anyProcessed = false;

  // for (; w_start != w_end; ++w_start) {
  //   ANCHOR_Event *a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowUpdate, *w_start);

  //   (*w_start)->validate();

  //   if (a_event) {
  //     pushEvent(a_event);
  //     anyProcessed = true;
  //   }
  // }

  // m_dirty_windows.clear();
  // return anyProcessed;
  return false;
}

ANCHOR_WindowSDL *ANCHOR_SystemSDL::findAnchorWindow(SDL_Window *sdl_win)
{
  if (sdl_win == NULL)
    return NULL;

  // It is not entirely safe to do this as the backptr may point
  // to a window that has recently been removed.
  // We should always check the window manager's list of windows
  // and only process events on these windows.

  const std::vector<ANCHOR_ISystemWindow *> &win_vec = m_windowManager->getWindows();

  std::vector<ANCHOR_ISystemWindow *>::const_iterator win_it = win_vec.begin();
  std::vector<ANCHOR_ISystemWindow *>::const_iterator win_end = win_vec.end();

  for (; win_it != win_end; ++win_it) {
    ANCHOR_WindowSDL *window = static_cast<ANCHOR_WindowSDL *>(*win_it);
    if (window->getSDLWindow() == sdl_win) {
      return window;
    }
  }
  return NULL;
}

/**
 * Events don't always have valid windows,
 * but ANCHOR needs a window _always_.
 * fallback to the Vulkan window. */
static SDL_Window *SDL_GetWindowFromID_fallback(Uint32 id)
{
  SDL_Window *sdl_win = SDL_GetWindowFromID(id);
  if (sdl_win == NULL) {
    sdl_win = SDL_GL_GetCurrentWindow();
  }
  return sdl_win;
}

#define GXMAP(k, x, y) \
  case x: \
    k = y; \
    break

static eAnchorKey convertSDLKey(SDL_Scancode key)
{
  eAnchorKey type;

  if ((key >= SDL_SCANCODE_A) && (key <= SDL_SCANCODE_Z)) {
    type = eAnchorKey(key - SDL_SCANCODE_A + int(ANCHOR_KeyA));
  }
  else if ((key >= SDL_SCANCODE_1) && (key <= SDL_SCANCODE_0)) {
    type = (key == SDL_SCANCODE_0) ? ANCHOR_Key0 : eAnchorKey(key - SDL_SCANCODE_1 + int(ANCHOR_Key1));
  }
  else if ((key >= SDL_SCANCODE_F1) && (key <= SDL_SCANCODE_F12)) {
    type = eAnchorKey(key - SDL_SCANCODE_F1 + int(ANCHOR_KeyF1));
  }
  else if ((key >= SDL_SCANCODE_F13) && (key <= SDL_SCANCODE_F24)) {
    type = eAnchorKey(key - SDL_SCANCODE_F13 + int(ANCHOR_KeyF13));
  }
  else {
    switch (key) {
      /* TODO SDL_SCANCODE_NONUSBACKSLASH */

      GXMAP(type, SDL_SCANCODE_BACKSPACE, ANCHOR_KeyBackSpace);
      GXMAP(type, SDL_SCANCODE_TAB, ANCHOR_KeyTab);
      GXMAP(type, SDL_SCANCODE_RETURN, ANCHOR_KeyEnter);
      GXMAP(type, SDL_SCANCODE_ESCAPE, ANCHOR_KeyEsc);
      GXMAP(type, SDL_SCANCODE_SPACE, ANCHOR_KeySpace);

      GXMAP(type, SDL_SCANCODE_SEMICOLON, ANCHOR_KeySemicolon);
      GXMAP(type, SDL_SCANCODE_PERIOD, ANCHOR_KeyPeriod);
      GXMAP(type, SDL_SCANCODE_COMMA, ANCHOR_KeyComma);
      GXMAP(type, SDL_SCANCODE_APOSTROPHE, ANCHOR_KeyQuote);
      GXMAP(type, SDL_SCANCODE_GRAVE, ANCHOR_KeyAccentGrave);
      GXMAP(type, SDL_SCANCODE_MINUS, ANCHOR_KeyMinus);
      GXMAP(type, SDL_SCANCODE_EQUALS, ANCHOR_KeyEqual);

      GXMAP(type, SDL_SCANCODE_SLASH, ANCHOR_KeySlash);
      GXMAP(type, SDL_SCANCODE_BACKSLASH, ANCHOR_KeyBackslash);
      GXMAP(type, SDL_SCANCODE_KP_EQUALS, ANCHOR_KeyEqual);
      GXMAP(type, SDL_SCANCODE_LEFTBRACKET, ANCHOR_KeyLeftBracket);
      GXMAP(type, SDL_SCANCODE_RIGHTBRACKET, ANCHOR_KeyRightBracket);
      GXMAP(type, SDL_SCANCODE_PAUSE, ANCHOR_KeyPause);

      GXMAP(type, SDL_SCANCODE_LSHIFT, ANCHOR_KeyLeftShift);
      GXMAP(type, SDL_SCANCODE_RSHIFT, ANCHOR_KeyRightShift);
      GXMAP(type, SDL_SCANCODE_LCTRL, ANCHOR_KeyLeftControl);
      GXMAP(type, SDL_SCANCODE_RCTRL, ANCHOR_KeyRightControl);
      GXMAP(type, SDL_SCANCODE_LALT, ANCHOR_KeyLeftAlt);
      GXMAP(type, SDL_SCANCODE_RALT, ANCHOR_KeyRightAlt);
      GXMAP(type, SDL_SCANCODE_LGUI, ANCHOR_KeyOS);
      GXMAP(type, SDL_SCANCODE_RGUI, ANCHOR_KeyOS);
      GXMAP(type, SDL_SCANCODE_APPLICATION, ANCHOR_KeyApp);

      GXMAP(type, SDL_SCANCODE_INSERT, ANCHOR_KeyInsert);
      GXMAP(type, SDL_SCANCODE_DELETE, ANCHOR_KeyDelete);
      GXMAP(type, SDL_SCANCODE_HOME, ANCHOR_KeyHome);
      GXMAP(type, SDL_SCANCODE_END, ANCHOR_KeyEnd);
      GXMAP(type, SDL_SCANCODE_PAGEUP, ANCHOR_KeyUpPage);
      GXMAP(type, SDL_SCANCODE_PAGEDOWN, ANCHOR_KeyDownPage);

      GXMAP(type, SDL_SCANCODE_LEFT, ANCHOR_KeyLeftArrow);
      GXMAP(type, SDL_SCANCODE_RIGHT, ANCHOR_KeyRightArrow);
      GXMAP(type, SDL_SCANCODE_UP, ANCHOR_KeyUpArrow);
      GXMAP(type, SDL_SCANCODE_DOWN, ANCHOR_KeyDownArrow);

      GXMAP(type, SDL_SCANCODE_CAPSLOCK, ANCHOR_KeyCapsLock);
      GXMAP(type, SDL_SCANCODE_SCROLLLOCK, ANCHOR_KeyScrollLock);
      GXMAP(type, SDL_SCANCODE_NUMLOCKCLEAR, ANCHOR_KeyNumLock);
      GXMAP(type, SDL_SCANCODE_PRINTSCREEN, ANCHOR_KeyPrintScreen);

      /**
       * Keypad Events ---------------- */

      /**
       * NOTE: SDL defines a bunch of key-pad identifiers
       * that aren't supported by ANCHOR, such as:
       * - #SDL_SCANCODE_KP_PERCENT
       * - #SDL_SCANCODE_KP_XOR. */
      GXMAP(type, SDL_SCANCODE_KP_0, ANCHOR_KeyNumpad0);
      GXMAP(type, SDL_SCANCODE_KP_1, ANCHOR_KeyNumpad1);
      GXMAP(type, SDL_SCANCODE_KP_2, ANCHOR_KeyNumpad2);
      GXMAP(type, SDL_SCANCODE_KP_3, ANCHOR_KeyNumpad3);
      GXMAP(type, SDL_SCANCODE_KP_4, ANCHOR_KeyNumpad4);
      GXMAP(type, SDL_SCANCODE_KP_5, ANCHOR_KeyNumpad5);
      GXMAP(type, SDL_SCANCODE_KP_6, ANCHOR_KeyNumpad6);
      GXMAP(type, SDL_SCANCODE_KP_7, ANCHOR_KeyNumpad7);
      GXMAP(type, SDL_SCANCODE_KP_8, ANCHOR_KeyNumpad8);
      GXMAP(type, SDL_SCANCODE_KP_9, ANCHOR_KeyNumpad9);
      GXMAP(type, SDL_SCANCODE_KP_PERIOD, ANCHOR_KeyNumpadPeriod);

      GXMAP(type, SDL_SCANCODE_KP_ENTER, ANCHOR_KeyNumpadEnter);
      GXMAP(type, SDL_SCANCODE_KP_PLUS, ANCHOR_KeyNumpadPlus);
      GXMAP(type, SDL_SCANCODE_KP_MINUS, ANCHOR_KeyNumpadMinus);
      GXMAP(type, SDL_SCANCODE_KP_MULTIPLY, ANCHOR_KeyNumpadAsterisk);
      GXMAP(type, SDL_SCANCODE_KP_DIVIDE, ANCHOR_KeyNumpadSlash);

      /**
       * Media keys in some keyboards and laptops with XFree86/Xorg */
      GXMAP(type, SDL_SCANCODE_AUDIOPLAY, ANCHOR_KeyMediaPlay);
      GXMAP(type, SDL_SCANCODE_AUDIOSTOP, ANCHOR_KeyMediaStop);
      GXMAP(type, SDL_SCANCODE_AUDIOPREV, ANCHOR_KeyMediaFirst);
      // GXMAP(type,XF86XK_AudioRewind,       ANCHOR_KeyMediaFirst);
      GXMAP(type, SDL_SCANCODE_AUDIONEXT, ANCHOR_KeyMediaLast);

      default:
        printf("Unknown\n");
        type = ANCHOR_KeyUnknown;
        break;
    }
  }

  return type;
}
#undef GXMAP

void ANCHOR_SystemSDL::processEvent(SDL_Event *sdl_event)
{
  ANCHOR_Event *a_event = NULL;

  switch (sdl_event->type) {
    case SDL_WINDOWEVENT: {
      SDL_WindowEvent &sdl_sub_evt = sdl_event->window;
      ANCHOR_WindowSDL *window = findAnchorWindow(SDL_GetWindowFromID_fallback(sdl_sub_evt.windowID));

      switch (sdl_sub_evt.event) {
        case SDL_WINDOWEVENT_EXPOSED:
          a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowUpdate, window);
          break;
        case SDL_WINDOWEVENT_RESIZED:
          a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowSize, window);
          break;
        case SDL_WINDOWEVENT_MOVED:
          a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowMove, window);
          break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
          a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowActivate, window);
          break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
          a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowDeactivate, window);
          break;
        case SDL_WINDOWEVENT_CLOSE:
          a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventWindowClose, window);
          break;
      }

      break;
    }

    case SDL_QUIT: {
      ANCHOR_ISystemWindow *window = m_windowManager->getActiveWindow();
      a_event = new ANCHOR_Event(ANCHOR::GetTime(), ANCHOR_EventQuitRequest, window);
      break;
    }

    case SDL_MOUSEMOTION: {
      SDL_MouseMotionEvent &sdl_sub_evt = sdl_event->motion;
      SDL_Window *sdl_win = SDL_GetWindowFromID_fallback(sdl_sub_evt.windowID);
      ANCHOR_WindowSDL *window = findAnchorWindow(sdl_win);
      assert(window != NULL);

      int x_win, y_win;
      SDL_GetWindowPosition(sdl_win, &x_win, &y_win);

      AnchorS32 x_root = sdl_sub_evt.x + x_win;
      AnchorS32 y_root = sdl_sub_evt.y + y_win;
      {
        a_event = new ANCHOR_EventCursor(
          ANCHOR::GetTime(), ANCHOR_EventCursorMove, window, x_root, y_root, ANCHOR_TABLET_DATA_NONE);
      }
      break;
    }
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN: {
      SDL_MouseButtonEvent &sdl_sub_evt = sdl_event->button;
      eAnchorButtonMask gbmask = ANCHOR_ButtonMaskLeft;
      eAnchorEventType type = (sdl_sub_evt.state == SDL_PRESSED) ? ANCHOR_EventButtonDown :
                                                                   ANCHOR_EventButtonUp;

      ANCHOR_WindowSDL *window = findAnchorWindow(SDL_GetWindowFromID_fallback(sdl_sub_evt.windowID));
      assert(window != NULL);

      /* process rest of normal mouse buttons */
      if (sdl_sub_evt.button == SDL_BUTTON_LEFT)
        gbmask = ANCHOR_ButtonMaskLeft;
      else if (sdl_sub_evt.button == SDL_BUTTON_MIDDLE)
        gbmask = ANCHOR_ButtonMaskMiddle;
      else if (sdl_sub_evt.button == SDL_BUTTON_RIGHT)
        gbmask = ANCHOR_ButtonMaskRight;
      /* these buttons are untested! */
      else if (sdl_sub_evt.button == SDL_BUTTON_X1)
        gbmask = ANCHOR_ButtonMaskButton4;
      else if (sdl_sub_evt.button == SDL_BUTTON_X2)
        gbmask = ANCHOR_ButtonMaskButton5;
      else
        break;

      a_event = new ANCHOR_EventButton(ANCHOR::GetTime(), type, window, gbmask, ANCHOR_TABLET_DATA_NONE);
      break;
    }
    case SDL_MOUSEWHEEL: {
      SDL_MouseWheelEvent &sdl_sub_evt = sdl_event->wheel;
      ANCHOR_WindowSDL *window = findAnchorWindow(SDL_GetWindowFromID_fallback(sdl_sub_evt.windowID));
      assert(window != NULL);
      a_event = new ANCHOR_EventWheel(ANCHOR::GetTime(), window, sdl_sub_evt.y);
      break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      SDL_KeyboardEvent &sdl_sub_evt = sdl_event->key;
      SDL_Keycode sym = sdl_sub_evt.keysym.sym;
      eAnchorEventType type = (sdl_sub_evt.state == SDL_PRESSED) ? ANCHOR_EventKeyDown : ANCHOR_EventKeyUp;

      ANCHOR_WindowSDL *window = findAnchorWindow(SDL_GetWindowFromID_fallback(sdl_sub_evt.windowID));
      assert(window != NULL);

      eAnchorKey akey = convertSDLKey(sdl_sub_evt.keysym.scancode);
      /**
       * note, the sdl_sub_evt.keysym.sym is truncated,
       * for unicode support anchor has to be modified */
      if (sym > 127) {
        switch (sym) {
          case SDLK_KP_DIVIDE:
            sym = '/';
            break;
          case SDLK_KP_MULTIPLY:
            sym = '*';
            break;
          case SDLK_KP_MINUS:
            sym = '-';
            break;
          case SDLK_KP_PLUS:
            sym = '+';
            break;
          case SDLK_KP_1:
            sym = '1';
            break;
          case SDLK_KP_2:
            sym = '2';
            break;
          case SDLK_KP_3:
            sym = '3';
            break;
          case SDLK_KP_4:
            sym = '4';
            break;
          case SDLK_KP_5:
            sym = '5';
            break;
          case SDLK_KP_6:
            sym = '6';
            break;
          case SDLK_KP_7:
            sym = '7';
            break;
          case SDLK_KP_8:
            sym = '8';
            break;
          case SDLK_KP_9:
            sym = '9';
            break;
          case SDLK_KP_0:
            sym = '0';
            break;
          case SDLK_KP_PERIOD:
            sym = '.';
            break;
          default:
            sym = 0;
            break;
        }
      }
      else {
        if (sdl_sub_evt.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) {
          /* lame US keyboard assumptions */
          if (sym >= 'a' && sym <= ('a' + 32)) {
            sym -= 32;
          }
          else {
            switch (sym) {
              case '`':
                sym = '~';
                break;
              case '1':
                sym = '!';
                break;
              case '2':
                sym = '@';
                break;
              case '3':
                sym = '#';
                break;
              case '4':
                sym = '$';
                break;
              case '5':
                sym = '%';
                break;
              case '6':
                sym = '^';
                break;
              case '7':
                sym = '&';
                break;
              case '8':
                sym = '*';
                break;
              case '9':
                sym = '(';
                break;
              case '0':
                sym = ')';
                break;
              case '-':
                sym = '_';
                break;
              case '=':
                sym = '+';
                break;
              case '[':
                sym = '{';
                break;
              case ']':
                sym = '}';
                break;
              case '\\':
                sym = '|';
                break;
              case ';':
                sym = ':';
                break;
              case '\'':
                sym = '"';
                break;
              case ',':
                sym = '<';
                break;
              case '.':
                sym = '>';
                break;
              case '/':
                sym = '?';
                break;
              default:
                break;
            }
          }
        }
      }

      a_event = new ANCHOR_EventKey(ANCHOR::GetTime(), type, window, akey, sym, NULL, false);
      break;
    }
  }

  if (a_event) {
    pushEvent(a_event);
  }
}

bool ANCHOR_SystemSDL::processEvents(bool waitForEvent)
{
  bool anyProcessed = false;

  do {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ANCHOR_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(m_sdl_window->getSDLWindow()))
        processEvent(&event);
      anyProcessed = true;
    }

    // if (generateWindowExposeEvents()) {
    //   anyProcessed = true;
    // }
  } while (waitForEvent && !anyProcessed);

  m_sdl_window->frameUpdate();

  return anyProcessed;
}

void ANCHOR_WindowSDL::frameUpdate()
{
  /**
   * Resize swap chain? */
  if (g_SwapChainRebuild) {
    int width, height;
    SDL_GetWindowSize(getSDLWindow(), &width, &height);
    if (width > 0 && height > 0) {
      ANCHOR_ImplVulkan_SetMinImageCount(g_MinImageCount);
      ANCHOR_ImplVulkanH_CreateOrResizeWindow(g_PixarVkInstance->GetVulkanInstance(),
                                              g_PhysicalDevice,
                                              g_Device,
                                              &g_MainWindowData,
                                              g_QueueFamily,
                                              g_Allocator,
                                              width,
                                              height,
                                              g_MinImageCount);
      g_MainWindowData.FrameIndex = 0;
      g_SwapChainRebuild = false;
    }
  }

  /**
   * Start the ANCHOR frame. */
  ANCHOR_ImplVulkan_NewFrame();
  ANCHOR_SystemSDL::ANCHOR_ImplSDL2_NewFrame(getSDLWindow());
  ANCHOR::NewFrame();
}

/**
 * Vulkan :: Swap that Chain!
 *
 * On Vulkan this preforms exceptionally well,
 * through testing this runs @ approx 4000 FPS
 * on an old NVIDIA GTX 1080Ti (other GPU was
 * not even enabled in Vulkan Device selection). */

eAnchorStatus ANCHOR_WindowSDL::swapBuffers()
{
  ANCHOR::Render();
  ImDrawData *draw_data = ANCHOR::GetDrawData();
  const bool is_minimized = (draw_data->DisplaySize[0] <= 0.0f || draw_data->DisplaySize[1] <= 0.0f);
  if (!is_minimized) {
    m_vulkan_context->ClearValue.color.float32[0] = g_HDPARAMS_Apollo.clearColor[0];
    m_vulkan_context->ClearValue.color.float32[1] = g_HDPARAMS_Apollo.clearColor[1];
    m_vulkan_context->ClearValue.color.float32[2] = g_HDPARAMS_Apollo.clearColor[2];
    m_vulkan_context->ClearValue.color.float32[3] = g_HDPARAMS_Apollo.clearColor[3];
    FrameRender(m_vulkan_context, draw_data);
    FramePresent(m_vulkan_context);
  }
  return ANCHOR_SUCCESS;
}

void ANCHOR_clean_vulkan(ANCHOR_WindowSDL *window /**Todo::VkResult &err*/)
{
  // err = vkDeviceWaitIdle(g_Device);
  // check_vk_result(err);
  vkDeviceWaitIdle(g_Device);
  ANCHOR_ImplVulkan_Shutdown();
  ANCHOR_SystemSDL::ANCHOR_ImplSDL2_Shutdown();
  ANCHOR::DestroyContext();

  CleanupVulkanWindow();
  CleanupVulkan();

  SDL_DestroyWindow(window->getSDLWindow());
  SDL_Quit();
}