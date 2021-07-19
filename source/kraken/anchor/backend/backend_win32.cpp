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
 * ⚓︎ Anchor.
 * Bare Metal.
 */

#define winmax(a, b) (((a) > (b)) ? (a) : (b))
#define winmin(a, b) (((a) < (b)) ? (a) : (b))

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "ANCHOR_BACKEND_vulkan.h"
#include "ANCHOR_BACKEND_win32.h"
#include "ANCHOR_rect.h"
#include "ANCHOR_api.h"
#include "ANCHOR_buttons.h"
#include "ANCHOR_debug_codes.h"
#include "ANCHOR_event.h"
#include "ANCHOR_window.h"

#include "utfconv.h"

#include "KKE_main.h"

#include <wabi/base/arch/systemInfo.h>
#include <wabi/base/tf/diagnostic.h>
#include <wabi/base/tf/envSetting.h>
#include <wabi/imaging/hgiVulkan/diagnostic.h>
#include <wabi/imaging/hgiVulkan/graphicsPipeline.h>
#include <wabi/imaging/hgiVulkan/hgi.h>
#include <wabi/imaging/hgiVulkan/instance.h>
#include <wabi/imaging/hgiVulkan/commandBuffer.h>
#include <wabi/imaging/hgiVulkan/commandQueue.h>
#include <wabi/imaging/hgiVulkan/pipelineCache.h>
#include <wabi/imaging/hgiVulkan/texture.h>
#include <wabi/imaging/hgiVulkan/garbageCollector.h>
#include <wabi/imaging/hgiVulkan/capabilities.h>

#define VMA_IMPLEMENTATION
#include <wabi/imaging/hgiVulkan/vk_mem_alloc.h>
#undef VMA_IMPLEMENTATION

#ifndef _WIN32_IE
/**
 * shipped before XP, so doesn't
 * impose addnl requirements. */
#  define _WIN32_IE 0x0501
#endif

#include <Dwmapi.h>

#include <assert.h>
#include <math.h>
#include <string.h>

#include <commctrl.h>
#include <psapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <strsafe.h>
#include <tlhelp32.h>
#include <windowsx.h>

#ifndef GET_POINTERID_WPARAM
/**
 * GET_POINTERID_WPARAM */
#  define GET_POINTERID_WPARAM(wParam) (LOWORD(wParam))
#endif

#include <tchar.h>
#include <winnt.h>

#include <cstdio>
#include <cstring>

/**
 * Using XInput for gamepad (will load DLL dynamically) */
#ifndef ANCHOR_IMPL_WIN32_DISABLE_GAMEPAD
#  include <xinput.h>
typedef DWORD(WINAPI *PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES *);
typedef DWORD(WINAPI *PFN_XInputGetState)(DWORD, XINPUT_STATE *);
#endif

WABI_NAMESPACE_USING

/**
 * HYDRA RENDERING PARAMS. */
static UsdApolloRenderParams g_HDPARAMS_Apollo;

static VkDescriptorSetLayout g_DescriptorSetLayout = VK_NULL_HANDLE;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
static VkDescriptorSet g_DescriptorSet = VK_NULL_HANDLE;

static ANCHOR_VulkanGPU_Surface g_MainWindowData;
static bool g_SwapChainRebuild = false;

struct AnchorBackendWin32Data
{
  HWND hWnd;
  INT64 Time;
  INT64 TicksPerSecond;
  AnchorMouseCursor LastMouseCursor;
  bool HasGamepad;
  bool WantUpdateHasGamepad;

#ifndef ANCHOR_IMPL_WIN32_DISABLE_GAMEPAD
  HMODULE XInputDLL;
  PFN_XInputGetCapabilities XInputGetCapabilities;
  PFN_XInputGetState XInputGetState;
#endif

  AnchorBackendWin32Data()
  {
    memset(this, 0, sizeof(*this));
  }
};

/**
 * Backend data stored in io.BackendPlatformUserData
 * to allow support for multiple ANCHOR contexts It
 * is STRONGLY preferred that you use docking branch
 * with multi-viewports (== single ANCHOR context +
 * multiple windows) instead of multiple ANCHOR
 * contexts.
 * 
 * - FIXME: multi-context support is not well
 *          tested and probably dysfunctional
 *          in this backend.
 * - FIXME: some shared resources (mouse cursor
 *          shape, gamepad) are mishandled when
 *          using multi-context. */
static AnchorBackendWin32Data *AnchorBackendWin32GetBackendData()
{
  return ANCHOR::GetCurrentContext() ? (AnchorBackendWin32Data *)ANCHOR::GetIO().BackendPlatformUserData : NULL;
}

/**
 * Functions */
static bool AnchorBackendWin32Init(void *hwnd)
{
  AnchorIO &io = ANCHOR::GetIO();
  ANCHOR_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

  INT64 perf_frequency, perf_counter;
  if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&perf_frequency))
    return false;
  if (!::QueryPerformanceCounter((LARGE_INTEGER *)&perf_counter))
    return false;

  /**
   * Setup backend capabilities flags.
   *
   * - We can honor GetMouseCursor() values (optional).
   * - We can honor io.WantSetMousePos requests (optional,
   *   rarely used) */
  AnchorBackendWin32Data *bd = IM_NEW(AnchorBackendWin32Data)();
  io.BackendPlatformUserData = (void *)bd;
  io.BackendPlatformName = "imgui_impl_win32";
  io.BackendFlags |= AnchorBackendFlags_HasMouseCursors;
  io.BackendFlags |= AnchorBackendFlags_HasSetMousePos;

  bd->hWnd = (HWND)hwnd;
  bd->WantUpdateHasGamepad = true;
  bd->TicksPerSecond = perf_frequency;
  bd->Time = perf_counter;
  bd->LastMouseCursor = AnchorMouseCursor_COUNT;

  io.ImeWindowHandle = hwnd;

  /**
   * Keyboard mapping. ANCHOR will use those
   * indices to peek into the io.KeysDown[]
   * array that we will update during the
   * application lifetime. */
  io.KeyMap[AnchorKey_Tab] = VK_TAB;
  io.KeyMap[AnchorKey_LeftArrow] = VK_LEFT;
  io.KeyMap[AnchorKey_RightArrow] = VK_RIGHT;
  io.KeyMap[AnchorKey_UpArrow] = VK_UP;
  io.KeyMap[AnchorKey_DownArrow] = VK_DOWN;
  io.KeyMap[AnchorKey_PageUp] = VK_PRIOR;
  io.KeyMap[AnchorKey_PageDown] = VK_NEXT;
  io.KeyMap[AnchorKey_Home] = VK_HOME;
  io.KeyMap[AnchorKey_End] = VK_END;
  io.KeyMap[AnchorKey_Insert] = VK_INSERT;
  io.KeyMap[AnchorKey_Delete] = VK_DELETE;
  io.KeyMap[AnchorKey_Backspace] = VK_BACK;
  io.KeyMap[AnchorKey_Space] = VK_SPACE;
  io.KeyMap[AnchorKey_Enter] = VK_RETURN;
  io.KeyMap[AnchorKey_Escape] = VK_ESCAPE;
  io.KeyMap[AnchorKey_KeyPadEnter] = VK_RETURN;
  io.KeyMap[AnchorKey_A] = 'A';
  io.KeyMap[AnchorKey_C] = 'C';
  io.KeyMap[AnchorKey_V] = 'V';
  io.KeyMap[AnchorKey_X] = 'X';
  io.KeyMap[AnchorKey_Y] = 'Y';
  io.KeyMap[AnchorKey_Z] = 'Z';

  /**
   * Dynamically load XInput library */
  const char *xinput_dll_names[] = {
    /* Windows 8+ */
    "xinput1_4.dll",
    /* DirectX SDK */
    "xinput1_3.dll",
    /* Windows Vista, Windows 7 */
    "xinput9_1_0.dll",
    /* DirectX SDK */
    "xinput1_2.dll",
    /* DirectX SDK */
    "xinput1_1.dll"};

  for (int n = 0; n < ANCHOR_ARRAYSIZE(xinput_dll_names); n++)
  {
    if (HMODULE dll = ::LoadLibraryA(xinput_dll_names[n]))
    {
      bd->XInputDLL = dll;
      bd->XInputGetCapabilities = (PFN_XInputGetCapabilities)::GetProcAddress(dll, "XInputGetCapabilities");
      bd->XInputGetState = (PFN_XInputGetState)::GetProcAddress(dll, "XInputGetState");
      break;
    }
  }
  return true;
}

static void AnchorBackendWin32Shutdown()
{
  AnchorIO &io = ANCHOR::GetIO();
  AnchorBackendWin32Data *bd = AnchorBackendWin32GetBackendData();

  if (bd->XInputDLL)
    ::FreeLibrary(bd->XInputDLL);

  io.BackendPlatformName = NULL;
  io.BackendPlatformUserData = NULL;
  IM_DELETE(bd);
}

static bool AnchorBackendWin32UpdateMouseCursor()
{
  AnchorIO &io = ANCHOR::GetIO();
  if (io.ConfigFlags & AnchorConfigFlags_NoMouseCursorChange)
    return false;

  AnchorMouseCursor imgui_cursor = ANCHOR::GetMouseCursor();
  if (imgui_cursor == AnchorMouseCursor_None || io.MouseDrawCursor)
  {
    /** 
     * Hide OS mouse cursor if
     * anchor is drawing it or
     * if it wants no cursor. */
    ::SetCursor(NULL);
  }
  else
  {
    /**
     * Show OS mouse cursor */
    LPTSTR win32_cursor = IDC_ARROW;
    switch (imgui_cursor)
    {
      case AnchorMouseCursor_Arrow:
        win32_cursor = IDC_ARROW;
        break;
      case AnchorMouseCursor_TextInput:
        win32_cursor = IDC_IBEAM;
        break;
      case AnchorMouseCursor_ResizeAll:
        win32_cursor = IDC_SIZEALL;
        break;
      case AnchorMouseCursor_ResizeEW:
        win32_cursor = IDC_SIZEWE;
        break;
      case AnchorMouseCursor_ResizeNS:
        win32_cursor = IDC_SIZENS;
        break;
      case AnchorMouseCursor_ResizeNESW:
        win32_cursor = IDC_SIZENESW;
        break;
      case AnchorMouseCursor_ResizeNWSE:
        win32_cursor = IDC_SIZENWSE;
        break;
      case AnchorMouseCursor_Hand:
        win32_cursor = IDC_HAND;
        break;
      case AnchorMouseCursor_NotAllowed:
        win32_cursor = IDC_NO;
        break;
    }
    ::SetCursor(::LoadCursor(NULL, win32_cursor));
  }
  return true;
}

static void AnchorBackendWin32UpdateMousePos()
{
  AnchorIO &io = ANCHOR::GetIO();
  AnchorBackendWin32Data *bd = AnchorBackendWin32GetBackendData();
  ANCHOR_ASSERT(bd->hWnd != 0);

  /**
   * Set OS mouse position if requested
   * (rarely used, only when SetMousePos
   * is enabled by user). */
  if (io.WantSetMousePos)
  {
    POINT pos = {(int)io.MousePos[0], (int)io.MousePos[1]};
    if (::ClientToScreen(bd->hWnd, &pos))
      ::SetCursorPos(pos.x, pos.y);
  }

  /**
   * Set mouse position. */
  io.MousePos = wabi::GfVec2f(-FLT_MAX, -FLT_MAX);
  POINT pos;
  if (HWND active_window = ::GetForegroundWindow())
    if (active_window == bd->hWnd || ::IsChild(active_window, bd->hWnd))
      if (::GetCursorPos(&pos) && ::ScreenToClient(bd->hWnd, &pos))
        io.MousePos = wabi::GfVec2f((float)pos.x, (float)pos.y);
}

/**
 * Gamepad navigation mapping */
static void AnchorBackendWin32UpdateGamepads()
{
#ifndef ANCHOR_IMPL_WIN32_DISABLE_GAMEPAD
  AnchorIO &io = ANCHOR::GetIO();
  AnchorBackendWin32Data *bd = AnchorBackendWin32GetBackendData();
  memset(io.NavInputs, 0, sizeof(io.NavInputs));
  if ((io.ConfigFlags & AnchorConfigFlags_NavEnableGamepad) == 0)
    return;

  /**
   * Calling XInputGetState() every frame on
   * disconnected gamepads is unfortunately
   * too slow. Instead we refresh gamepad
   * availability by calling the function
   * XInputGetCapabilities() _only_ after
   * receiving WM_DEVICECHANGE. */
  if (bd->WantUpdateHasGamepad)
  {
    XINPUT_CAPABILITIES caps;
    bd->HasGamepad = bd->XInputGetCapabilities ? (bd->XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS) : false;
    bd->WantUpdateHasGamepad = false;
  }

  io.BackendFlags &= ~AnchorBackendFlags_HasGamepad;
  XINPUT_STATE xinput_state;
  if (bd->HasGamepad && bd->XInputGetState && bd->XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
  {
    const XINPUT_GAMEPAD &gamepad = xinput_state.Gamepad;
    io.BackendFlags |= AnchorBackendFlags_HasGamepad;

#  define MAP_BUTTON(NAV_NO, BUTTON_ENUM) \
    { \
      io.NavInputs[NAV_NO] = (gamepad.wButtons & BUTTON_ENUM) ? 1.0f : 0.0f; \
    }
#  define MAP_ANALOG(NAV_NO, VALUE, V0, V1) \
    { \
      float vn = (float)(VALUE - V0) / (float)(V1 - V0); \
      if (vn > 1.0f) \
        vn = 1.0f; \
      if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) \
        io.NavInputs[NAV_NO] = vn; \
    }


    /**
     * Cross / A */
    MAP_BUTTON(AnchorNavInput_Activate, XINPUT_GAMEPAD_A);
    /**
     * Circle / B */
    MAP_BUTTON(AnchorNavInput_Cancel, XINPUT_GAMEPAD_B);
    /**
     * Square / X */
    MAP_BUTTON(AnchorNavInput_Menu, XINPUT_GAMEPAD_X);
    /**
     * Triangle / Y */
    MAP_BUTTON(AnchorNavInput_Input, XINPUT_GAMEPAD_Y);
    /**
     * D-Pad Left */
    MAP_BUTTON(AnchorNavInput_DpadLeft, XINPUT_GAMEPAD_DPAD_LEFT);
    /**
     * D-Pad Right */
    MAP_BUTTON(AnchorNavInput_DpadRight, XINPUT_GAMEPAD_DPAD_RIGHT);
    /**
     * D-Pad Up */
    MAP_BUTTON(AnchorNavInput_DpadUp, XINPUT_GAMEPAD_DPAD_UP);
    /**
     * D-Pad Down */
    MAP_BUTTON(AnchorNavInput_DpadDown, XINPUT_GAMEPAD_DPAD_DOWN);
    /**
     * L1 / LB */
    MAP_BUTTON(AnchorNavInput_FocusPrev, XINPUT_GAMEPAD_LEFT_SHOULDER);
    /**
     * R1 / RB */
    MAP_BUTTON(AnchorNavInput_FocusNext, XINPUT_GAMEPAD_RIGHT_SHOULDER);
    /**
     * L1 / LB */
    MAP_BUTTON(AnchorNavInput_TweakSlow, XINPUT_GAMEPAD_LEFT_SHOULDER);
    /**
     * R1 / RB */
    MAP_BUTTON(AnchorNavInput_TweakFast, XINPUT_GAMEPAD_RIGHT_SHOULDER);
    MAP_ANALOG(AnchorNavInput_LStickLeft, gamepad.sThumbLX, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
    MAP_ANALOG(AnchorNavInput_LStickRight, gamepad.sThumbLX, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
    MAP_ANALOG(AnchorNavInput_LStickUp, gamepad.sThumbLY, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
    MAP_ANALOG(AnchorNavInput_LStickDown, gamepad.sThumbLY, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32767);
#  undef MAP_BUTTON
#  undef MAP_ANALOG
  }
#endif
}

static void AnchorBackendWin32NewFrame()
{
  AnchorIO &io = ANCHOR::GetIO();
  AnchorBackendWin32Data *bd = AnchorBackendWin32GetBackendData();
  ANCHOR_ASSERT(bd != NULL && "Did you call AnchorBackendWin32Init()?");

  /**
   * Setup display size (every frame
   * to accommodate for window resizing) */
  RECT rect = {0, 0, 0, 0};
  ::GetClientRect(bd->hWnd, &rect);
  io.DisplaySize = wabi::GfVec2f((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

  /**
   * Setup time step */
  INT64 current_time = 0;
  ::QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
  io.DeltaTime = (float)(current_time - bd->Time) / bd->TicksPerSecond;
  bd->Time = current_time;

  /**
   * Read keyboard modifiers inputs
   * 
   * filled by the WndProc handler below:
   *  -  io.KeysDown[], 
   *  -  io.MousePos,
   *  -  io.MouseDown[],
   *  -  io.MouseWheel: */
  io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
  io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
  io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
  io.KeySuper = false;

  /**
   * Update OS mouse position */
  AnchorBackendWin32UpdateMousePos();

  /**
   * Update OS mouse cursor with the cursor requested by imgui */
  AnchorMouseCursor mouse_cursor = io.MouseDrawCursor ? AnchorMouseCursor_None : ANCHOR::GetMouseCursor();
  if (bd->LastMouseCursor != mouse_cursor)
  {
    bd->LastMouseCursor = mouse_cursor;
    AnchorBackendWin32UpdateMouseCursor();
  }

  /**
   * Update game controllers (if enabled and available) */
  AnchorBackendWin32UpdateGamepads();
}

/**
 * Allow compilation with old Windows SDK. MinGW doesn't have default _WIN32_WINNT/WINVER versions. */
#ifndef WM_MOUSEHWHEEL
#  define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef DBT_DEVNODES_CHANGED
#  define DBT_DEVNODES_CHANGED 0x0007
#endif


ANCHOR_BACKEND_API LRESULT AnchorBackendWin32WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (ANCHOR::GetCurrentContext() == NULL)
    return 0;

  AnchorIO &io = ANCHOR::GetIO();
  AnchorBackendWin32Data *bd = AnchorBackendWin32GetBackendData();

  switch (msg)
  {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK: {
      int button = 0;
      if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
      {
        button = 0;
      }
      if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK)
      {
        button = 1;
      }
      if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK)
      {
        button = 2;
      }
      if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK)
      {
        button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
      }
      if (!ANCHOR::IsAnyMouseDown() && ::GetCapture() == NULL)
        ::SetCapture(hwnd);
      io.MouseDown[button] = true;
      return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP: {
      int button = 0;
      if (msg == WM_LBUTTONUP)
      {
        button = 0;
      }
      if (msg == WM_RBUTTONUP)
      {
        button = 1;
      }
      if (msg == WM_MBUTTONUP)
      {
        button = 2;
      }
      if (msg == WM_XBUTTONUP)
      {
        button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
      }
      io.MouseDown[button] = false;
      if (!ANCHOR::IsAnyMouseDown() && ::GetCapture() == hwnd)
        ::ReleaseCapture();
      return 0;
    }
    case WM_MOUSEWHEEL:
      io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
      return 0;
    case WM_MOUSEHWHEEL:
      io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
      return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      if (wParam < 256)
        io.KeysDown[wParam] = 1;
      return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
      if (wParam < 256)
        io.KeysDown[wParam] = 0;
      return 0;
    case WM_KILLFOCUS:
      memset(io.KeysDown, 0, sizeof(io.KeysDown));
      return 0;
    case WM_CHAR:
      // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
      if (wParam > 0 && wParam < 0x10000)
        io.AddInputCharacterUTF16((unsigned short)wParam);
      return 0;
    case WM_SETCURSOR:
      if (LOWORD(lParam) == HTCLIENT && AnchorBackendWin32UpdateMouseCursor())
        return 1;
      return 0;
    case WM_DEVICECHANGE:
      if ((UINT)wParam == DBT_DEVNODES_CHANGED)
        bd->WantUpdateHasGamepad = true;
      return 0;
  }
  return 0;
}

static BOOL _IsWindowsVersionOrGreater(WORD major, WORD minor, WORD)
{
  typedef LONG(WINAPI * PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW *, ULONG, ULONGLONG);
  static PFN_RtlVerifyVersionInfo RtlVerifyVersionInfoFn = NULL;
  if (RtlVerifyVersionInfoFn == NULL)
    if (HMODULE ntdllModule = ::GetModuleHandleA("ntdll.dll"))
      RtlVerifyVersionInfoFn = (PFN_RtlVerifyVersionInfo)GetProcAddress(ntdllModule, "RtlVerifyVersionInfo");
  if (RtlVerifyVersionInfoFn == NULL)
    return FALSE;

  RTL_OSVERSIONINFOEXW versionInfo = {};
  ULONGLONG conditionMask = 0;
  versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
  versionInfo.dwMajorVersion = major;
  versionInfo.dwMinorVersion = minor;
  VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
  VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
  return (RtlVerifyVersionInfoFn(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask) == 0) ? TRUE : FALSE;
}

#define _IsWindowsVistaOrGreater() _IsWindowsVersionOrGreater(HIBYTE(0x0600), LOBYTE(0x0600), 0)    // _WIN32_WINNT_VISTA
#define _IsWindows8OrGreater() _IsWindowsVersionOrGreater(HIBYTE(0x0602), LOBYTE(0x0602), 0)        // _WIN32_WINNT_WIN8
#define _IsWindows8Point1OrGreater() _IsWindowsVersionOrGreater(HIBYTE(0x0603), LOBYTE(0x0603), 0)  // _WIN32_WINNT_WINBLUE
#define _IsWindows10OrGreater() _IsWindowsVersionOrGreater(HIBYTE(0x0A00), LOBYTE(0x0A00), 0)       // _WIN32_WINNT_WINTHRESHOLD / _WIN32_WINNT_WIN10

#ifndef DPI_ENUMS_DECLARED
typedef enum
{
  PROCESS_DPI_UNAWARE = 0,
  PROCESS_SYSTEM_DPI_AWARE = 1,
  PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
typedef enum
{
  MDT_EFFECTIVE_DPI = 0,
  MDT_ANGULAR_DPI = 1,
  MDT_RAW_DPI = 2,
  MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
#endif
#ifndef _DPI_AWARENESS_CONTEXTS_
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
#  define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE (DPI_AWARENESS_CONTEXT) - 3
#endif
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#  define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 (DPI_AWARENESS_CONTEXT) - 4
#endif
typedef HRESULT(WINAPI *PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);                      // Shcore.lib + dll, Windows 8.1+
typedef HRESULT(WINAPI *PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *);       // Shcore.lib + dll, Windows 8.1+
typedef DPI_AWARENESS_CONTEXT(WINAPI *PFN_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);  // User32.lib + dll, Windows 10 v1607+ (Creators Update)

// Helper function to enable DPI awareness without setting up a manifest
static void AnchorBackendWin32EnableDpiAwareness()
{
  if (_IsWindows10OrGreater())
  {
    static HINSTANCE user32_dll = ::LoadLibraryA("user32.dll");  // Reference counted per-process
    if (PFN_SetThreadDpiAwarenessContext SetThreadDpiAwarenessContextFn = (PFN_SetThreadDpiAwarenessContext)::GetProcAddress(user32_dll, "SetThreadDpiAwarenessContext"))
    {
      SetThreadDpiAwarenessContextFn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
      return;
    }
  }
  if (_IsWindows8Point1OrGreater())
  {
    static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll");  // Reference counted per-process
    if (PFN_SetProcessDpiAwareness SetProcessDpiAwarenessFn = (PFN_SetProcessDpiAwareness)::GetProcAddress(shcore_dll, "SetProcessDpiAwareness"))
    {
      SetProcessDpiAwarenessFn(PROCESS_PER_MONITOR_DPI_AWARE);
      return;
    }
  }
#if _WIN32_WINNT >= 0x0600
  ::SetProcessDPIAware();
#endif
}

#if defined(_MSC_VER) && !defined(NOGDI)
#  pragma comment(lib, "gdi32")
#endif

static float AnchorBackendWin32GetDpiScaleForMonitor(void *monitor)
{
  UINT xdpi = 96, ydpi = 96;
  if (_IsWindows8Point1OrGreater())
  {
    static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll");  // Reference counted per-process
    static PFN_GetDpiForMonitor GetDpiForMonitorFn = NULL;
    if (GetDpiForMonitorFn == NULL && shcore_dll != NULL)
      GetDpiForMonitorFn = (PFN_GetDpiForMonitor)::GetProcAddress(shcore_dll, "GetDpiForMonitor");
    if (GetDpiForMonitorFn != NULL)
    {
      GetDpiForMonitorFn((HMONITOR)monitor, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
      ANCHOR_ASSERT(xdpi == ydpi);  // Please contact me if you hit this assert!
      return xdpi / 96.0f;
    }
  }
#ifndef NOGDI
  const HDC dc = ::GetDC(NULL);
  xdpi = ::GetDeviceCaps(dc, LOGPIXELSX);
  ydpi = ::GetDeviceCaps(dc, LOGPIXELSY);
  ANCHOR_ASSERT(xdpi == ydpi);  // Please contact me if you hit this assert!
  ::ReleaseDC(NULL, dc);
#endif
  return xdpi / 96.0f;
}

static float AnchorBackendWin32GetDpiScaleForHwnd(void *hwnd)
{
  HMONITOR monitor = ::MonitorFromWindow((HWND)hwnd, MONITOR_DEFAULTTONEAREST);
  return AnchorBackendWin32GetDpiScaleForMonitor(monitor);
}

//---------------------------------------------------------------------------------------------------------
// Transparency related helpers (optional)
//--------------------------------------------------------------------------------------------------------

#if defined(_MSC_VER)
#  pragma comment(lib, "dwmapi")  // Link with dwmapi.lib. MinGW will require linking with '-ldwmapi'
#endif

static void AnchorBackendWin32EnableAlphaCompositing(void *hwnd)
{
  if (!_IsWindowsVistaOrGreater())
    return;

  BOOL composition;
  if (FAILED(::DwmIsCompositionEnabled(&composition)) || !composition)
    return;

  BOOL opaque;
  DWORD color;
  if (_IsWindows8OrGreater() || (SUCCEEDED(::DwmGetColorizationColor(&color, &opaque)) && !opaque))
  {
    HRGN region = ::CreateRectRgn(0, 0, -1, -1);
    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = region;
    bb.fEnable = TRUE;
    ::DwmEnableBlurBehindWindow((HWND)hwnd, &bb);
    ::DeleteObject(region);
  }
  else
  {
    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE;
    ::DwmEnableBlurBehindWindow((HWND)hwnd, &bb);
  }
}


// --------------------------------------------------------------------------------------------------------


AnchorDisplayManagerWin32::AnchorDisplayManagerWin32(void)
{}

eAnchorStatus AnchorDisplayManagerWin32::getNumDisplays(AnchorU8 &numDisplays) const
{
  numDisplays = ::GetSystemMetrics(SM_CMONITORS);
  return numDisplays > 0 ? ANCHOR_SUCCESS : ANCHOR_FAILURE;
}

static BOOL get_dd(DWORD d, DISPLAY_DEVICE *dd)
{
  dd->cb = sizeof(DISPLAY_DEVICE);
  return ::EnumDisplayDevices(NULL, d, dd, 0);
}

/*
 * When you call EnumDisplaySettings with iModeNum set to zero, the operating system
 * initializes and caches information about the display device. When you call
 * EnumDisplaySettings with iModeNum set to a non-zero value, the function returns
 * the information that was cached the last time the function was called with iModeNum
 * set to zero. */
eAnchorStatus AnchorDisplayManagerWin32::getNumDisplaySettings(AnchorU8 display,
                                                               AnchorS32 &numSettings) const
{
  DISPLAY_DEVICE display_device;
  if (!get_dd(display, &display_device))
    return ANCHOR_FAILURE;

  numSettings = 0;
  DEVMODE dm;
  while (::EnumDisplaySettings(display_device.DeviceName, numSettings, &dm))
  {
    numSettings++;
  }
  return ANCHOR_SUCCESS;
}

eAnchorStatus AnchorDisplayManagerWin32::getDisplaySetting(AnchorU8 display,
                                                           AnchorS32 index,
                                                           ANCHOR_DisplaySetting &setting) const
{
  DISPLAY_DEVICE display_device;
  if (!get_dd(display, &display_device))
    return ANCHOR_FAILURE;

  eAnchorStatus success;
  DEVMODE dm;
  if (::EnumDisplaySettings(display_device.DeviceName, index, &dm))
  {
    setting.xPixels = dm.dmPelsWidth;
    setting.yPixels = dm.dmPelsHeight;
    setting.bpp = dm.dmBitsPerPel;

    /**
     * When you call the EnumDisplaySettings function, the dmDisplayFrequency member
     * may return with the value 0 or 1. These values represent the display hardware's
     * default refresh rate. This default rate is typically set by switches on a display
     * card or computer motherboard, or by a configuration program that does not use
     * Win32 display functions such as ChangeDisplaySettings. */

    /**
     * First, we tried to explicitly set the frequency to 60 if EnumDisplaySettings
     * returned 0 or 1 but this doesn't work since later on an exact match will
     * be searched. And this will never happen if we change it to 60. Now we rely
     * on the default h/w setting. */
    setting.frequency = dm.dmDisplayFrequency;
    success = ANCHOR_SUCCESS;
  }
  else
  {
    success = ANCHOR_FAILURE;
  }
  return success;
}

eAnchorStatus AnchorDisplayManagerWin32::getCurrentDisplaySetting(AnchorU8 display,
                                                                  ANCHOR_DisplaySetting &setting) const
{
  return getDisplaySetting(display, ENUM_CURRENT_SETTINGS, setting);
}

eAnchorStatus AnchorDisplayManagerWin32::setCurrentDisplaySetting(AnchorU8 display,
                                                                  const ANCHOR_DisplaySetting &setting)
{
  DISPLAY_DEVICE display_device;
  if (!get_dd(display, &display_device))
    return ANCHOR_FAILURE;

  ANCHOR_DisplaySetting match;
  findMatch(display, setting, match);
  DEVMODE dm;
  int i = 0;
  while (::EnumDisplaySettings(display_device.DeviceName, i++, &dm))
  {
    if ((dm.dmBitsPerPel == match.bpp) && (dm.dmPelsWidth == match.xPixels) &&
        (dm.dmPelsHeight == match.yPixels) && (dm.dmDisplayFrequency == match.frequency))
    {
      break;
    }
  }

  /**
   * dm.dmBitsPerPel = match.bpp;
   * dm.dmPelsWidth = match.xPixels;
   * dm.dmPelsHeight = match.yPixels;
   * dm.dmDisplayFrequency = match.frequency;
   * dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
   * dm.dmSize = sizeof(DEVMODE);
   * dm.dmDriverExtra = 0; */

  LONG status = ::ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
  return status == DISP_CHANGE_SUCCESSFUL ? ANCHOR_SUCCESS : ANCHOR_FAILURE;
}

//---------------------------------------------------------------------------------------------------------

/**
 * Key code values not found in winuser.h */
#ifndef VK_MINUS
#  define VK_MINUS 0xBD
#endif  // VK_MINUS
#ifndef VK_SEMICOLON
#  define VK_SEMICOLON 0xBA
#endif  // VK_SEMICOLON
#ifndef VK_PERIOD
#  define VK_PERIOD 0xBE
#endif  // VK_PERIOD
#ifndef VK_COMMA
#  define VK_COMMA 0xBC
#endif  // VK_COMMA
#ifndef VK_QUOTE
#  define VK_QUOTE 0xDE
#endif  // VK_QUOTE
#ifndef VK_BACK_QUOTE
#  define VK_BACK_QUOTE 0xC0
#endif  // VK_BACK_QUOTE
#ifndef VK_SLASH
#  define VK_SLASH 0xBF
#endif  // VK_SLASH
#ifndef VK_BACK_SLASH
#  define VK_BACK_SLASH 0xDC
#endif  // VK_BACK_SLASH
#ifndef VK_EQUALS
#  define VK_EQUALS 0xBB
#endif  // VK_EQUALS
#ifndef VK_OPEN_BRACKET
#  define VK_OPEN_BRACKET 0xDB
#endif  // VK_OPEN_BRACKET
#ifndef VK_CLOSE_BRACKET
#  define VK_CLOSE_BRACKET 0xDD
#endif  // VK_CLOSE_BRACKET
#ifndef VK_GR_LESS
#  define VK_GR_LESS 0xE2
#endif  // VK_GR_LESS

/**
 * Workaround for some laptop touchpads, some of which seems to
 * have driver issues which makes it so window function receives
 * the message, but PeekMessage doesn't pick those messages for
 * some reason.
 *
 * We send a dummy WM_USER message to force PeekMessage to receive
 * something, making it so kraken's window manager sees the new
 * messages coming in. */
#define BROKEN_PEEK_TOUCHPAD

static void initRawInput()
{
#define DEVICE_COUNT 1

  RAWINPUTDEVICE devices[DEVICE_COUNT];
  memset(devices, 0, DEVICE_COUNT * sizeof(RAWINPUTDEVICE));

  // Initiates WM_INPUT messages from keyboard
  // That way ANCHOR can retrieve true keys
  devices[0].usUsagePage = 0x01;
  devices[0].usUsage = 0x06; /* http://msdn.microsoft.com/en-us/windows/hardware/gg487473.aspx */

  RegisterRawInputDevices(devices, DEVICE_COUNT, sizeof(RAWINPUTDEVICE));

#undef DEVICE_COUNT
}

typedef BOOL(WINAPI *ANCHOR_WIN32_EnableNonClientDpiScaling)(HWND);

AnchorSystemWin32::AnchorSystemWin32()
  : m_hasPerformanceCounter(false),
    m_freq(0),
    m_start(0),
    m_lfstart(0)
{
  m_displayManager = new AnchorDisplayManagerWin32();
  ANCHOR_ASSERT(m_displayManager);
  m_displayManager->initialize();

  m_consoleStatus = 1;

  /**
   * Tell Windows we are per monitor DPI aware. This
   * disables the default blurry scaling and enables
   * WM_DPICHANGED to allow us to draw at proper DPI. */
  SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

  /**
   * Check if current keyboard layout uses AltGr and save keylayout ID for
   * specialized handling if keys like VK_OEM_*. I.e. french keylayout
   * generates VK_OEM_8 for their exclamation key (key left of right shift). */
  this->handleKeyboardChange();

  /**
   * Require COM for ANCHOR_DropTargetWin32. */
  OleInitialize(0);
}

AnchorSystemWin32::~AnchorSystemWin32()
{
  /**
   * Shutdown COM. */
  OleUninitialize();
  toggleConsole(1);
}

bool AnchorSystemWin32::processEvents(bool waitForEvent)
{
  MSG msg;
  bool hasEventHandled = false;

  do
  {
    if (waitForEvent && !::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
      ::Sleep(1);
    }

    if (ANCHOR::GetCurrentContext())
    {
      hasEventHandled = true;
    }

    /**
     * Process all the events waiting for us. */
    while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) != 0)
    {
      /**
       * TranslateMessage doesn't alter the message, and doesn't change our raw keyboard data.
       * Needed for MapVirtualKey or if we ever need to get chars from wm_ime_char or similar. */
      ::TranslateMessage(&msg);
      ::DispatchMessageW(&msg);
      hasEventHandled = true;
    }

    /* PeekMessage above is allowed to dispatch messages to the wndproc without us
     * noticing, so we need to check the event manager here to see if there are
     * events waiting in the queue.
     */
    hasEventHandled |= this->m_eventManager->getNumEvents() > 0;

  } while (waitForEvent && !hasEventHandled);


  // if (g_SwapChainRebuild)
  // {
  //   AnchorRect winrect;
  //   AnchorWindowWin32 *window = (AnchorWindowWin32 *)m_windowManager->getActiveWindow();
  //   window->getWindowBounds(winrect);
  //   if (winrect.getWidth() > 0 && winrect.getHeight() > 0)
  //   {
  //     ANCHOR_ImplVulkan_SetMinImageCount(window->getMinImageCount());
  //     ANCHOR_ImplVulkanH_CreateOrResizeWindow(window->getHydraInstance()->GetVulkanInstance(),
  //                                             window->getHydraDevice()->GetVulkanPhysicalDevice(),
  //                                             window->getHydraDevice()->GetVulkanDevice(),
  //                                             window->updateVulkanSurface(&g_MainWindowData),
  //                                             window->getHydraDevice()->GetGfxQueueFamilyIndex(),
  //                                             HgiVulkanAllocator(),
  //                                             winrect.getWidth(),
  //                                             winrect.getHeight(),
  //                                             window->getMinImageCount());
  //     g_MainWindowData.FrameIndex = 0;
  //     g_SwapChainRebuild = false;
  //   }
  // }

  ANCHOR::NewFrame();

  ANCHOR::Begin("Kraken");
  ANCHOR::Text("Computer Graphics of the Modern Age.");
  ANCHOR::End();

  return hasEventHandled;
}

eAnchorStatus AnchorSystemWin32::getModifierKeys(AnchorModifierKeys &keys) const
{
  bool down = HIBYTE(::GetKeyState(VK_LSHIFT)) != 0;
  keys.set(ANCHOR_ModifierKeyLeftShift, down);
  down = HIBYTE(::GetKeyState(VK_RSHIFT)) != 0;
  keys.set(ANCHOR_ModifierKeyRightShift, down);

  down = HIBYTE(::GetKeyState(VK_LMENU)) != 0;
  keys.set(ANCHOR_ModifierKeyLeftAlt, down);
  down = HIBYTE(::GetKeyState(VK_RMENU)) != 0;
  keys.set(ANCHOR_ModifierKeyRightAlt, down);

  down = HIBYTE(::GetKeyState(VK_LCONTROL)) != 0;
  keys.set(ANCHOR_ModifierKeyLeftControl, down);
  down = HIBYTE(::GetKeyState(VK_RCONTROL)) != 0;
  keys.set(ANCHOR_ModifierKeyRightControl, down);

  bool lwindown = HIBYTE(::GetKeyState(VK_LWIN)) != 0;
  bool rwindown = HIBYTE(::GetKeyState(VK_RWIN)) != 0;
  if (lwindown || rwindown)
    keys.set(ANCHOR_ModifierKeyOS, true);
  else
    keys.set(ANCHOR_ModifierKeyOS, false);
  return ANCHOR_SUCCESS;
}

eAnchorStatus AnchorSystemWin32::getButtons(AnchorButtons &buttons) const
{
  /**
   * Check for swapped buttons (left-handed mouse buttons)
   * GetAsyncKeyState() will give back the state of the
   * physical mouse buttons. */
  bool swapped = ::GetSystemMetrics(SM_SWAPBUTTON) == TRUE;

  bool down = HIBYTE(::GetAsyncKeyState(VK_LBUTTON)) != 0;
  buttons.set(swapped ? ANCHOR_ButtonMaskRight : ANCHOR_ButtonMaskLeft, down);

  down = HIBYTE(::GetAsyncKeyState(VK_MBUTTON)) != 0;
  buttons.set(ANCHOR_ButtonMaskMiddle, down);

  down = HIBYTE(::GetAsyncKeyState(VK_RBUTTON)) != 0;
  buttons.set(swapped ? ANCHOR_ButtonMaskLeft : ANCHOR_ButtonMaskRight, down);
  return ANCHOR_SUCCESS;
}

AnchorU8 AnchorSystemWin32::getNumDisplays() const
{
  ANCHOR_ASSERT(m_displayManager);
  AnchorU8 numDisplays;
  m_displayManager->getNumDisplays(numDisplays);
  return numDisplays;
}

void AnchorSystemWin32::getMainDisplayDimensions(AnchorU32 &width, AnchorU32 &height) const
{
  width = ::GetSystemMetrics(SM_CXSCREEN);
  height = ::GetSystemMetrics(SM_CYSCREEN);
}

void AnchorSystemWin32::getAllDisplayDimensions(AnchorU32 &width, AnchorU32 &height) const
{
  width = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
  height = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

eAnchorStatus AnchorSystemWin32::init()
{
  eAnchorStatus success = AnchorSystem::init();
  InitCommonControls();

  /* Disable scaling on high DPI displays on Vista */
  SetProcessDPIAware();
  initRawInput();

  m_lfstart = ::GetTickCount();

  /* Determine whether this system has a high frequency performance counter. */
  m_hasPerformanceCounter = ::QueryPerformanceFrequency((LARGE_INTEGER *)&m_freq) == TRUE;
  if (m_hasPerformanceCounter)
  {
    if (TfDebug::IsEnabled(ANCHOR_WIN32))
    {
      TF_SUCCESS_MSG("Anchor -- High Frequency Performance Timer available");
    }
    ::QueryPerformanceCounter((LARGE_INTEGER *)&m_start);
  }
  else
  {
    if (TfDebug::IsEnabled(ANCHOR_WIN32))
    {
      TF_WARN("Anchor -- High Frequency Performance Timer not available");
    }
  }

  if (success == ANCHOR_SUCCESS)
  {
    WNDCLASSW wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = s_wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ::GetModuleHandle(0);
    wc.hIcon = ::LoadIcon(wc.hInstance, "APPICON");

    if (!wc.hIcon)
    {
      ::LoadIcon(NULL, IDI_APPLICATION);
    }
    wc.hCursor = ::LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
    wc.lpszMenuName = 0;
    wc.lpszClassName = L"AnchorWindowClass";

    if (::RegisterClassW(&wc) == 0)
    {
      success = ANCHOR_FAILURE;
    }
  }

  return success;
}

AnchorISystemWindow *AnchorSystemWin32::createWindow(const char *title,
                                                     const char *icon,
                                                     AnchorS32 left,
                                                     AnchorS32 top,
                                                     AnchorU32 width,
                                                     AnchorU32 height,
                                                     eAnchorWindowState state,
                                                     eAnchorDrawingContextType type,
                                                     int vkSettings,
                                                     const bool exclusive,
                                                     const bool is_dialog,
                                                     const AnchorISystemWindow *parentWindow)
{
  AnchorWindowWin32 *window = new AnchorWindowWin32(this,
                                                    title,
                                                    icon,
                                                    left,
                                                    top,
                                                    width,
                                                    height,
                                                    state,
                                                    type,
                                                    false,
                                                    (AnchorWindowWin32 *)parentWindow,
                                                    is_dialog);

  if (window->getValid())
  {
    /**
     * Store the pointer to the window. */
    m_windowManager->addWindow(window);
    m_windowManager->setActiveWindow(window);
  }
  else
  {
    if (TfDebug::IsEnabled(ANCHOR_WIN32))
    {
      TF_ERROR_MSG("Window invalid");
    }
    delete window;
    window = NULL;
  }

  return window;
}

eAnchorStatus AnchorSystemWin32::getCursorPosition(AnchorS32 &x, AnchorS32 &y) const
{
  POINT point;
  if (::GetCursorPos(&point))
  {
    x = point.x;
    y = point.y;
    return ANCHOR_SUCCESS;
  }
  return ANCHOR_FAILURE;
}

eAnchorStatus AnchorSystemWin32::setCursorPosition(AnchorS32 x, AnchorS32 y)
{
  if (!::GetActiveWindow())
    return ANCHOR_FAILURE;
  return ::SetCursorPos(x, y) == TRUE ? ANCHOR_SUCCESS : ANCHOR_FAILURE;
}

void AnchorSystemWin32::processMinMaxInfo(MINMAXINFO *minmax)
{
  minmax->ptMinTrackSize.x = 320;
  minmax->ptMinTrackSize.y = 240;
}

static DWORD GetParentProcessID(void)
{
  HANDLE snapshot;
  PROCESSENTRY32 pe32 = {0};
  DWORD ppid = 0, pid = GetCurrentProcessId();
  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
  {
    return -1;
  }
  pe32.dwSize = sizeof(pe32);
  if (!Process32First(snapshot, &pe32))
  {
    CloseHandle(snapshot);
    return -1;
  }
  do
  {
    if (pe32.th32ProcessID == pid)
    {
      ppid = pe32.th32ParentProcessID;
      break;
    }
  } while (Process32Next(snapshot, &pe32));
  CloseHandle(snapshot);
  return ppid;
}

static bool getProcessName(int pid, char *buffer, int max_len)
{
  bool result = false;
  HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (handle)
  {
    GetModuleFileNameEx(handle, 0, buffer, max_len);
    result = true;
  }
  CloseHandle(handle);
  return result;
}

static bool isStartedFromCommandPrompt()
{
  HWND hwnd = GetConsoleWindow();

  if (hwnd)
  {
    DWORD pid = (DWORD)-1;
    DWORD ppid = GetParentProcessID();
    char parent_name[MAX_PATH];
    bool start_from_launcher = false;

    GetWindowThreadProcessId(hwnd, &pid);
    if (getProcessName(ppid, parent_name, sizeof(parent_name)))
    {
      char *filename = strrchr(parent_name, '\\');
      if (filename != NULL)
      {
        start_from_launcher = strstr(filename, "kraken.exe") != NULL;
      }
    }

    /* When we're starting from a wrapper we need to compare with parent process ID. */
    if (pid != (start_from_launcher ? ppid : GetCurrentProcessId()))
      return true;
  }

  return false;
}

int AnchorSystemWin32::toggleConsole(int action)
{
  HWND wnd = GetConsoleWindow();

  switch (action)
  {
    case 3:  // startup: hide if not started from command prompt
    {
      if (!isStartedFromCommandPrompt())
      {
        ShowWindow(wnd, SW_HIDE);
        m_consoleStatus = 0;
      }
      break;
    }
    case 0:  // hide
      ShowWindow(wnd, SW_HIDE);
      m_consoleStatus = 0;
      break;
    case 1:  // show
      ShowWindow(wnd, SW_SHOW);
      if (!isStartedFromCommandPrompt())
      {
        DeleteMenu(GetSystemMenu(wnd, FALSE), SC_CLOSE, MF_BYCOMMAND);
      }
      m_consoleStatus = 1;
      break;
    case 2:  // toggle
      ShowWindow(wnd, m_consoleStatus ? SW_HIDE : SW_SHOW);
      m_consoleStatus = !m_consoleStatus;
      if (m_consoleStatus && !isStartedFromCommandPrompt())
      {
        DeleteMenu(GetSystemMenu(wnd, FALSE), SC_CLOSE, MF_BYCOMMAND);
      }
      break;
  }

  return m_consoleStatus;
}

LRESULT WINAPI AnchorSystemWin32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  AnchorEvent *event = NULL;
  bool eventHandled = false;

  LRESULT lResult = 0;
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();
#ifdef WITH_INPUT_IME
  AnchorEventManager *eventManager = system->getEventManager();
#endif
  ANCHOR_ASSERT(system);

  if (hwnd)
  {

    if (msg == WM_NCCREATE)
    {
      // Tell Windows to automatically handle scaling of non-client areas
      // such as the caption bar. EnableNonClientDpiScaling was introduced in Windows 10
      HMODULE m_user32 = ::LoadLibrary("User32.dll");
      if (m_user32)
      {
        ANCHOR_WIN32_EnableNonClientDpiScaling fpEnableNonClientDpiScaling =
          (ANCHOR_WIN32_EnableNonClientDpiScaling)::GetProcAddress(m_user32, "EnableNonClientDpiScaling");
        if (fpEnableNonClientDpiScaling)
        {
          fpEnableNonClientDpiScaling(hwnd);
        }
      }
    }

    AnchorWindowWin32 *window = (AnchorWindowWin32 *)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (window)
    {
      switch (msg)
      {
        // we need to check if new key layout has AltGr
        case WM_INPUTLANGCHANGE: {
          system->handleKeyboardChange();
#ifdef WITH_INPUT_IME
          window->getImeInput()->SetInputLanguage();
#endif
          break;
        }
        ////////////////////////////////////////////////////////////////////////
        // Keyboard events, processed
        ////////////////////////////////////////////////////////////////////////
        case WM_INPUT: {
          RAWINPUT raw;
          RAWINPUT *raw_ptr = &raw;
          UINT rawSize = sizeof(RAWINPUT);

          GetRawInputData((HRAWINPUT)lParam, RID_INPUT, raw_ptr, &rawSize, sizeof(RAWINPUTHEADER));

          switch (raw.header.dwType)
          {
            case RIM_TYPEKEYBOARD:
              event = processKeyEvent(window, raw);
              if (!event)
              {
                if (TfDebug::IsEnabled(ANCHOR_WIN32))
                {
                  TF_WARN("AnchorSystemWin32::wndProc: key event ");
                  TF_WARN(std::to_string(msg));
                  TF_WARN(" key ignored");
                }
              }
              break;
#ifdef WITH_INPUT_NDOF
            case RIM_TYPEHID:
              if (system->processNDOF(raw))
              {
                eventHandled = true;
              }
              break;
#endif
          }
          break;
        }
#ifdef WITH_INPUT_IME
        ////////////////////////////////////////////////////////////////////////
        // IME events, processed, read more in ANCHOR_IME.h
        ////////////////////////////////////////////////////////////////////////
        case WM_IME_SETCONTEXT: {
          ANCHOR_ImeWin32 *ime = window->getImeInput();
          ime->SetInputLanguage();
          ime->CreateImeWindow(hwnd);
          ime->CleanupComposition(hwnd);
          ime->CheckFirst(hwnd);
          break;
        }
        case WM_IME_STARTCOMPOSITION: {
          ANCHOR_ImeWin32 *ime = window->getImeInput();
          eventHandled = true;
          /* remove input event before start comp event, avoid redundant input */
          eventManager->removeTypeEvents(AnchorEventTypeKeyDown, window);
          ime->CreateImeWindow(hwnd);
          ime->ResetComposition(hwnd);
          event = processImeEvent(AnchorEventTypeImeCompositionStart, window, &ime->eventImeData);
          break;
        }
        case WM_IME_COMPOSITION: {
          ANCHOR_ImeWin32 *ime = window->getImeInput();
          eventHandled = true;
          ime->UpdateImeWindow(hwnd);
          ime->UpdateInfo(hwnd);
          if (ime->eventImeData.result_len)
          {
            /* remove redundant IME event */
            eventManager->removeTypeEvents(AnchorEventTypeImeComposition, window);
          }
          event = processImeEvent(AnchorEventTypeImeComposition, window, &ime->eventImeData);
          break;
        }
        case WM_IME_ENDCOMPOSITION: {
          ANCHOR_ImeWin32 *ime = window->getImeInput();
          eventHandled = true;
          /* remove input event after end comp event, avoid redundant input */
          eventManager->removeTypeEvents(AnchorEventTypeKeyDown, window);
          ime->ResetComposition(hwnd);
          ime->DestroyImeWindow(hwnd);
          event = processImeEvent(AnchorEventTypeImeCompositionEnd, window, &ime->eventImeData);
          break;
        }
#endif /* WITH_INPUT_IME */
        ////////////////////////////////////////////////////////////////////////
        // Keyboard events, ignored
        ////////////////////////////////////////////////////////////////////////
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        /* These functions were replaced by #WM_INPUT. */
        case WM_CHAR:
        /* The WM_CHAR message is posted to the window with the keyboard focus when
         * a WM_KEYDOWN message is translated by the TranslateMessage function. WM_CHAR
         * contains the character code of the key that was pressed.
         */
        case WM_DEADCHAR:
          /* The WM_DEADCHAR message is posted to the window with the keyboard focus when a
           * WM_KEYUP message is translated by the TranslateMessage function. WM_DEADCHAR
           * specifies a character code generated by a dead key. A dead key is a key that
           * generates a character, such as the umlaut (double-dot), that is combined with
           * another character to form a composite character. For example, the umlaut-O
           * character (Ö) is generated by typing the dead key for the umlaut character, and
           * then typing the O key.
           */
          break;
        case WM_SYSDEADCHAR:
        /* The WM_SYSDEADCHAR message is sent to the window with the keyboard focus when
         * a WM_SYSKEYDOWN message is translated by the TranslateMessage function.
         * WM_SYSDEADCHAR specifies the character code of a system dead key - that is,
         * a dead key that is pressed while holding down the alt key.
         */
        case WM_SYSCHAR:
          /* The WM_SYSCHAR message is sent to the window with the keyboard focus when
           * a WM_SYSCHAR message is translated by the TranslateMessage function.
           * WM_SYSCHAR specifies the character code of a dead key - that is,
           * a dead key that is pressed while holding down the alt key.
           * To prevent the sound, DefWindowProc must be avoided by return
           */
          break;
        case WM_SYSCOMMAND:
          /* The WM_SYSCOMMAND message is sent to the window when system commands such as
           * maximize, minimize  or close the window are triggered. Also it is sent when ALT
           * button is press for menu. To prevent this we must return preventing DefWindowProc.
           *
           * Note that the four low-order bits of the wParam parameter are used internally by the
           * OS. To obtain the correct result when testing the value of wParam, an application
           * must combine the value 0xFFF0 with the wParam value by using the bitwise AND operator.
           */
          switch (wParam & 0xFFF0)
          {
            case SC_KEYMENU:
              eventHandled = true;
              break;
            case SC_RESTORE: {
              ::ShowWindow(hwnd, SW_RESTORE);
              window->setState(window->getState());

#ifdef WINDOWS_NEEDS_TABLET_SUPPORT
              ANCHOR_Wintab *wt = window->getWintab();
              if (wt)
              {
                wt->enable();
              }
#endif /* WINDOWS_NEEDS_TABLET_SUPPORT */

              eventHandled = true;
              break;
            }
            case SC_MAXIMIZE: {

#ifdef WINDOWS_NEEDS_TABLET_SUPPORT
              ANCHOR_Wintab *wt = window->getWintab();
              if (wt)
              {
                wt->enable();
              }
#endif /* WINDOWS_NEEDS_TABLET_SUPPORT */

              /* Don't report event as handled so that default handling occurs. */
              break;
            }
            case SC_MINIMIZE: {

#ifdef WINDOWS_NEEDS_TABLET_SUPPORT
              ANCHOR_Wintab *wt = window->getWintab();
              if (wt)
              {
                wt->disable();
              }
#endif /* WINDOWS_NEEDS_TABLET_SUPPORT */

              /* Don't report event as handled so that default handling occurs. */
              break;
            }
          }
          break;

#ifdef WINDOWS_NEEDS_TABLET_SUPPORT

        ////////////////////////////////////////////////////////////////////////
        // Wintab events, processed
        ////////////////////////////////////////////////////////////////////////
        case WT_CSRCHANGE: {
          ANCHOR_Wintab *wt = window->getWintab();
          if (wt)
          {
            wt->updateCursorInfo();
          }
          eventHandled = true;
          break;
        }
        case WT_PROXIMITY: {
          ANCHOR_Wintab *wt = window->getWintab();
          if (wt)
          {
            bool inRange = LOWORD(lParam);
            if (inRange)
            {
              /* Some devices don't emit WT_CSRCHANGE events, so update cursor info here. */
              wt->updateCursorInfo();
            }
            else
            {
              wt->leaveRange();
            }
          }
          eventHandled = true;
          break;
        }
        case WT_INFOCHANGE: {
          ANCHOR_Wintab *wt = window->getWintab();
          if (wt)
          {
            wt->processInfoChange(lParam);

            if (window->usingTabletAPI(AnchorTabletWintab))
            {
              window->resetPointerPenInfo();
            }
          }
          eventHandled = true;
          break;
        }
        case WT_PACKET:
          processWintabEvent(window);
          eventHandled = true;
          break;

#endif /* WINDOWS_NEEDS_TABLET_SUPPORT */

        ////////////////////////////////////////////////////////////////////////
        // Pointer events, processed
        ////////////////////////////////////////////////////////////////////////
        case WM_POINTERUPDATE:
        case WM_POINTERDOWN:
        case WM_POINTERUP:
          processPointerEvent(msg, window, wParam, lParam, eventHandled);
          break;
        case WM_POINTERLEAVE: {
          AnchorU32 pointerId = GET_POINTERID_WPARAM(wParam);
          POINTER_INFO pointerInfo;
          if (!GetPointerInfo(pointerId, &pointerInfo))
          {
            break;
          }

          /* Reset pointer pen info if pen device has left tracking range. */
          if (pointerInfo.pointerType == PT_PEN)
          {
            window->resetPointerPenInfo();
            eventHandled = true;
          }
          break;
        }
        ////////////////////////////////////////////////////////////////////////
        // Mouse events, processed
        ////////////////////////////////////////////////////////////////////////
        case WM_LBUTTONDOWN:
          event = processButtonEvent(AnchorEventTypeButtonDown, window, ANCHOR_ButtonMaskLeft);
          break;
        case WM_MBUTTONDOWN:
          event = processButtonEvent(AnchorEventTypeButtonDown, window, ANCHOR_ButtonMaskMiddle);
          break;
        case WM_RBUTTONDOWN:
          event = processButtonEvent(AnchorEventTypeButtonDown, window, ANCHOR_ButtonMaskRight);
          break;
        case WM_XBUTTONDOWN:
          if ((short)HIWORD(wParam) == XBUTTON1)
          {
            event = processButtonEvent(AnchorEventTypeButtonDown, window, ANCHOR_ButtonMaskButton4);
          }
          else if ((short)HIWORD(wParam) == XBUTTON2)
          {
            event = processButtonEvent(AnchorEventTypeButtonDown, window, ANCHOR_ButtonMaskButton5);
          }
          break;
        case WM_LBUTTONUP:
          event = processButtonEvent(AnchorEventTypeButtonUp, window, ANCHOR_ButtonMaskLeft);
          break;
        case WM_MBUTTONUP:
          event = processButtonEvent(AnchorEventTypeButtonUp, window, ANCHOR_ButtonMaskMiddle);
          break;
        case WM_RBUTTONUP:
          event = processButtonEvent(AnchorEventTypeButtonUp, window, ANCHOR_ButtonMaskRight);
          break;
        case WM_XBUTTONUP:
          if ((short)HIWORD(wParam) == XBUTTON1)
          {
            event = processButtonEvent(AnchorEventTypeButtonUp, window, ANCHOR_ButtonMaskButton4);
          }
          else if ((short)HIWORD(wParam) == XBUTTON2)
          {
            event = processButtonEvent(AnchorEventTypeButtonUp, window, ANCHOR_ButtonMaskButton5);
          }
          break;
        case WM_MOUSEMOVE:
          if (!window->m_mousePresent)
          {
            TRACKMOUSEEVENT tme = {sizeof(tme)};
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            window->m_mousePresent = true;

#ifdef WINDOWS_NEEDS_TABLET_SUPPORT
            ANCHOR_Wintab *wt = window->getWintab();
            if (wt)
            {
              wt->gainFocus();
            }
#endif /* WINDOWS_NEEDS_TABLET_SUPPORT */
          }
          event = processCursorEvent(window);

          break;
        case WM_MOUSEWHEEL: {
          /* The WM_MOUSEWHEEL message is sent to the focus window
           * when the mouse wheel is rotated. The DefWindowProc
           * function propagates the message to the window's parent.
           * There should be no internal forwarding of the message,
           * since DefWindowProc propagates it up the parent chain
           * until it finds a window that processes it.
           */
          processWheelEvent(window, wParam, lParam);
          eventHandled = true;
#ifdef BROKEN_PEEK_TOUCHPAD
          PostMessage(hwnd, WM_USER, 0, 0);
#endif
          break;
        }
        case WM_SETCURSOR:
          /* The WM_SETCURSOR message is sent to a window if the mouse causes the cursor
           * to move within a window and mouse input is not captured.
           * This means we have to set the cursor shape every time the mouse moves!
           * The DefWindowProc function uses this message to set the cursor to an
           * arrow if it is not in the client area.
           */
          if (LOWORD(lParam) == HTCLIENT)
          {
            // Load the current cursor
            window->loadCursor(window->getCursorVisibility(), window->getCursorShape());
            // Bypass call to DefWindowProc
            return 0;
          }
          else
          {
            // Outside of client area show standard cursor
            window->loadCursor(true, ANCHOR_StandardCursorDefault);
          }
          break;
        case WM_MOUSELEAVE: {
          window->m_mousePresent = false;
          if (window->getTabletData().Active == AnchorTabletModeNone)
          {
            processCursorEvent(window);
          }
#ifdef WINDOWS_NEEDS_TABLET_SUPPORT
          ANCHOR_Wintab *wt = window->getWintab();
          if (wt)
          {
            wt->loseFocus();
          }
#endif /* WINDOWS_NEEDS_TABLET_SUPPORT */
          break;
        }
        ////////////////////////////////////////////////////////////////////////
        // Mouse events, ignored
        ////////////////////////////////////////////////////////////////////////
        case WM_NCMOUSEMOVE:
        /* The WM_NCMOUSEMOVE message is posted to a window when the cursor is moved
         * within the non-client area of the window. This message is posted to the window that
         * contains the cursor. If a window has captured the mouse, this message is not posted.
         */
        case WM_NCHITTEST:
          /* The WM_NCHITTEST message is sent to a window when the cursor moves, or
           * when a mouse button is pressed or released. If the mouse is not captured,
           * the message is sent to the window beneath the cursor. Otherwise, the message
           * is sent to the window that has captured the mouse.
           */
          break;

        ////////////////////////////////////////////////////////////////////////
        // Window events, processed
        ////////////////////////////////////////////////////////////////////////
        case WM_CLOSE:
          /* The WM_CLOSE message is sent as a signal that a window
           * or an application should terminate. Restore if minimized. */
          if (IsIconic(hwnd))
          {
            ShowWindow(hwnd, SW_RESTORE);
          }
          event = processWindowEvent(AnchorEventTypeWindowClose, window);
          break;
        case WM_ACTIVATE:
          /* The WM_ACTIVATE message is sent to both the window being activated and the window
           * being deactivated. If the windows use the same input queue, the message is sent
           * synchronously, first to the window procedure of the top-level window being
           * deactivated, then to the window procedure of the top-level window being activated.
           * If the windows use different input queues, the message is sent asynchronously,
           * so the window is activated immediately. */
          {
            AnchorModifierKeys modifiers;
            modifiers.clear();
            system->storeModifierKeys(modifiers);
            system->m_wheelDeltaAccum = 0;
            system->m_keycode_last_repeat_key = 0;
            event = processWindowEvent(LOWORD(wParam) ? AnchorEventTypeWindowActivate :
                                                        AnchorEventTypeWindowDeactivate,
                                       window);
            /* WARNING: Let DefWindowProc handle WM_ACTIVATE, otherwise WM_MOUSEWHEEL
             * will not be dispatched to OUR active window if we minimize one of OUR windows. */
            if (LOWORD(wParam) == WA_INACTIVE)
              window->lostMouseCapture();

            lResult = ::DefWindowProc(hwnd, msg, wParam, lParam);
            break;
          }
        case WM_ENTERSIZEMOVE:
          /* The WM_ENTERSIZEMOVE message is sent one time to a window after it enters the moving
           * or sizing modal loop. The window enters the moving or sizing modal loop when the user
           * clicks the window's title bar or sizing border, or when the window passes the
           * WM_SYSCOMMAND message to the DefWindowProc function and the wParam parameter of the
           * message specifies the SC_MOVE or SC_SIZE value. The operation is complete when
           * DefWindowProc returns.
           */
          window->m_inLiveResize = 1;
          break;
        case WM_EXITSIZEMOVE:
          window->m_inLiveResize = 0;
          break;
        case WM_PAINT:
          /* An application sends the WM_PAINT message when the system or another application
           * makes a request to paint a portion of an application's window. The message is sent
           * when the UpdateWindow or RedrawWindow function is called, or by the DispatchMessage
           * function when the application obtains a WM_PAINT message by using the GetMessage or
           * PeekMessage function.
           */
          if (!window->m_inLiveResize)
          {
            event = processWindowEvent(AnchorEventTypeWindowUpdate, window);
            ::ValidateRect(hwnd, NULL);
          }
          else
          {
            eventHandled = true;
          }
          break;
        case WM_GETMINMAXINFO:
          /* The WM_GETMINMAXINFO message is sent to a window when the size or
           * position of the window is about to change. An application can use
           * this message to override the window's default maximized size and
           * position, or its default minimum or maximum tracking size.
           */
          processMinMaxInfo((MINMAXINFO *)lParam);
          /* Let DefWindowProc handle it. */
          break;
        case WM_SIZING:
          event = processWindowSizeEvent(window);
          break;
        case WM_SIZE:
          /* The WM_SIZE message is sent to a window after its size has changed.
           * The WM_SIZE and WM_MOVE messages are not sent if an application handles the
           * WM_WINDOWPOSCHANGED message without calling DefWindowProc. It is more efficient
           * to perform any move or size change processing during the WM_WINDOWPOSCHANGED
           * message without calling DefWindowProc.
           */
          event = processWindowSizeEvent(window);
          break;
        case WM_CAPTURECHANGED:
          window->lostMouseCapture();
          break;
        case WM_MOVING:
          /* The WM_MOVING message is sent to a window that the user is moving. By processing
           * this message, an application can monitor the size and position of the drag rectangle
           * and, if needed, change its size or position.
           */
        case WM_MOVE:
          /* The WM_SIZE and WM_MOVE messages are not sent if an application handles the
           * WM_WINDOWPOSCHANGED message without calling DefWindowProc. It is more efficient
           * to perform any move or size change processing during the WM_WINDOWPOSCHANGED
           * message without calling DefWindowProc.
           */
          /* See #WM_SIZE comment. */
          if (window->m_inLiveResize)
          {
            system->pushEvent(processWindowEvent(AnchorEventTypeWindowMove, window));
            system->dispatchEvents();
          }
          else
          {
            event = processWindowEvent(AnchorEventTypeWindowMove, window);
          }

          break;
        case WM_DPICHANGED:
          /* The WM_DPICHANGED message is sent when the effective dots per inch (dpi) for a
           * window has changed. The DPI is the scale factor for a window. There are multiple
           * events that can cause the DPI to change such as when the window is moved to a monitor
           * with a different DPI.
           */
          {
            // The suggested new size and position of the window.
            RECT *const suggestedWindowRect = (RECT *)lParam;

            // Push DPI change event first
            system->pushEvent(processWindowEvent(AnchorEventTypeWindowDPIHintChanged, window));
            system->dispatchEvents();
            eventHandled = true;

            // Then move and resize window
            SetWindowPos(hwnd,
                         NULL,
                         suggestedWindowRect->left,
                         suggestedWindowRect->top,
                         suggestedWindowRect->right - suggestedWindowRect->left,
                         suggestedWindowRect->bottom - suggestedWindowRect->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
          }
          break;
        case WM_DISPLAYCHANGE: {
#ifdef WINDOWS_NEEDS_TABLET_SUPPORT
          ANCHOR_Wintab *wt = window->getWintab();
          if (wt)
          {
            wt->remapCoordinates();
          }
#endif /* WINDOWS_NEEDS_TABLET_SUPPORT */
          break;
        }
        case WM_KILLFOCUS:
          /* The WM_KILLFOCUS message is sent to a window immediately before it loses the keyboard
           * focus. We want to prevent this if a window is still active and it loses focus to
           * nowhere. */
          if (!wParam && hwnd == ::GetActiveWindow())
          {
            ::SetFocus(hwnd);
          }
          break;
        ////////////////////////////////////////////////////////////////////////
        // Window events, ignored
        ////////////////////////////////////////////////////////////////////////
        case WM_WINDOWPOSCHANGED:
        /* The WM_WINDOWPOSCHANGED message is sent to a window whose size, position, or place
         * in the Z order has changed as a result of a call to the SetWindowPos function or
         * another window-management function.
         * The WM_SIZE and WM_MOVE messages are not sent if an application handles the
         * WM_WINDOWPOSCHANGED message without calling DefWindowProc. It is more efficient
         * to perform any move or size change processing during the WM_WINDOWPOSCHANGED
         * message without calling DefWindowProc.
         */
        case WM_ERASEBKGND:
        /* An application sends the WM_ERASEBKGND message when the window background must be
         * erased (for example, when a window is resized). The message is sent to prepare an
         * invalidated portion of a window for painting.
         */
        case WM_NCPAINT:
        /* An application sends the WM_NCPAINT message to a window
         * when its frame must be painted. */
        case WM_NCACTIVATE:
        /* The WM_NCACTIVATE message is sent to a window when its non-client area needs to be
         * changed to indicate an active or inactive state. */
        case WM_DESTROY:
        /* The WM_DESTROY message is sent when a window is being destroyed. It is sent to the
         * window procedure of the window being destroyed after the window is removed from the
         * screen. This message is sent first to the window being destroyed and then to the child
         * windows (if any) as they are destroyed. During the processing of the message, it can
         * be assumed that all child windows still exist. */
        case WM_NCDESTROY:
          /* The WM_NCDESTROY message informs a window that its non-client area is being
           * destroyed. The DestroyWindow function sends the WM_NCDESTROY message to the window
           * following the WM_DESTROY message. WM_DESTROY is used to free the allocated memory
           * object associated with the window.
           */
          break;
        case WM_SHOWWINDOW:
        /* The WM_SHOWWINDOW message is sent to a window when the window is
         * about to be hidden or shown. */
        case WM_WINDOWPOSCHANGING:
        /* The WM_WINDOWPOSCHANGING message is sent to a window whose size, position, or place in
         * the Z order is about to change as a result of a call to the SetWindowPos function or
         * another window-management function.
         */
        case WM_SETFOCUS:
          /* The WM_SETFOCUS message is sent to a window after it has gained the keyboard focus. */
          break;
        ////////////////////////////////////////////////////////////////////////
        // Other events
        ////////////////////////////////////////////////////////////////////////
        case WM_GETTEXT:
        /* An application sends a WM_GETTEXT message to copy the text that
         * corresponds to a window into a buffer provided by the caller.
         */
        case WM_ACTIVATEAPP:
        /* The WM_ACTIVATEAPP message is sent when a window belonging to a
         * different application than the active window is about to be activated.
         * The message is sent to the application whose window is being activated
         * and to the application whose window is being deactivated.
         */
        case WM_TIMER:
          /* The WIN32 docs say:
           * The WM_TIMER message is posted to the installing thread's message queue
           * when a timer expires. You can process the message by providing a WM_TIMER
           * case in the window procedure. Otherwise, the default window procedure will
           * call the TimerProc callback function specified in the call to the SetTimer
           * function used to install the timer.
           *
           * In ANCHOR, we let DefWindowProc call the timer callback.
           */
          break;
      }
    }
    else
    {
      // Event found for a window before the pointer to the class has been set.
      // TF_DEBUG(ANCHOR_WIN32).Msg("[Anchor] recieved a window event before creation\n");
      /* These are events we typically miss at this point:
       * WM_GETMINMAXINFO 0x24
       * WM_NCCREATE          0x81
       * WM_NCCALCSIZE        0x83
       * WM_CREATE            0x01
       * We let DefWindowProc do the work.
       */
    }
  }
  else
  {
    // Events without valid hwnd
    TF_DEBUG(ANCHOR_WIN32).Msg("[Anchor] recieved event without valid hwnd\n");
  }

  if (event)
  {
    system->pushEvent(event);
    eventHandled = true;
  }

  if (!eventHandled)
    lResult = ::DefWindowProcW(hwnd, msg, wParam, lParam);

  return lResult;
}

eAnchorKey AnchorSystemWin32::convertKey(short vKey, short scanCode, short extend) const
{
  eAnchorKey key;

  if ((vKey >= '0') && (vKey <= '9'))
  {
    // VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39)
    key = (eAnchorKey)(vKey - '0' + AnchorKey0);
  }
  else if ((vKey >= 'A') && (vKey <= 'Z'))
  {
    // VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)
    key = (eAnchorKey)(vKey - 'A' + AnchorKeyA);
  }
  else if ((vKey >= VK_F1) && (vKey <= VK_F24))
  {
    key = (eAnchorKey)(vKey - VK_F1 + AnchorKeyF1);
  }
  else
  {
    switch (vKey)
    {
      case VK_RETURN:
        key = (extend) ? AnchorKeyNumpadEnter : AnchorKeyEnter;
        break;

      case VK_BACK:
        key = AnchorKeyBackSpace;
        break;
      case VK_TAB:
        key = AnchorKeyTab;
        break;
      case VK_ESCAPE:
        key = AnchorKeyEsc;
        break;
      case VK_SPACE:
        key = AnchorKeySpace;
        break;

      case VK_INSERT:
      case VK_NUMPAD0:
        key = (extend) ? AnchorKeyInsert : AnchorKeyNumpad0;
        break;
      case VK_END:
      case VK_NUMPAD1:
        key = (extend) ? AnchorKeyEnd : AnchorKeyNumpad1;
        break;
      case VK_DOWN:
      case VK_NUMPAD2:
        key = (extend) ? AnchorKeyDownArrow : AnchorKeyNumpad2;
        break;
      case VK_NEXT:
      case VK_NUMPAD3:
        key = (extend) ? AnchorKeyDownPage : AnchorKeyNumpad3;
        break;
      case VK_LEFT:
      case VK_NUMPAD4:
        key = (extend) ? AnchorKeyLeftArrow : AnchorKeyNumpad4;
        break;
      case VK_CLEAR:
      case VK_NUMPAD5:
        key = (extend) ? AnchorKeyUnknown : AnchorKeyNumpad5;
        break;
      case VK_RIGHT:
      case VK_NUMPAD6:
        key = (extend) ? AnchorKeyRightArrow : AnchorKeyNumpad6;
        break;
      case VK_HOME:
      case VK_NUMPAD7:
        key = (extend) ? AnchorKeyHome : AnchorKeyNumpad7;
        break;
      case VK_UP:
      case VK_NUMPAD8:
        key = (extend) ? AnchorKeyUpArrow : AnchorKeyNumpad8;
        break;
      case VK_PRIOR:
      case VK_NUMPAD9:
        key = (extend) ? AnchorKeyUpPage : AnchorKeyNumpad9;
        break;
      case VK_DECIMAL:
      case VK_DELETE:
        key = (extend) ? AnchorKeyDelete : AnchorKeyNumpadPeriod;
        break;

      case VK_SNAPSHOT:
        key = AnchorKeyPrintScreen;
        break;
      case VK_PAUSE:
        key = AnchorKeyPause;
        break;
      case VK_MULTIPLY:
        key = AnchorKeyNumpadAsterisk;
        break;
      case VK_SUBTRACT:
        key = AnchorKeyNumpadMinus;
        break;
      case VK_DIVIDE:
        key = AnchorKeyNumpadSlash;
        break;
      case VK_ADD:
        key = AnchorKeyNumpadPlus;
        break;

      case VK_SEMICOLON:
        key = AnchorKeySemicolon;
        break;
      case VK_EQUALS:
        key = AnchorKeyEqual;
        break;
      case VK_COMMA:
        key = AnchorKeyComma;
        break;
      case VK_MINUS:
        key = AnchorKeyMinus;
        break;
      case VK_PERIOD:
        key = AnchorKeyPeriod;
        break;
      case VK_SLASH:
        key = AnchorKeySlash;
        break;
      case VK_BACK_QUOTE:
        key = AnchorKeyAccentGrave;
        break;
      case VK_OPEN_BRACKET:
        key = AnchorKeyLeftBracket;
        break;
      case VK_BACK_SLASH:
        key = AnchorKeyBackslash;
        break;
      case VK_CLOSE_BRACKET:
        key = AnchorKeyRightBracket;
        break;
      case VK_QUOTE:
        key = AnchorKeyQuote;
        break;
      case VK_GR_LESS:
        key = AnchorKeyGrLess;
        break;

      case VK_SHIFT:
        /* Check single shift presses */
        if (scanCode == 0x36)
        {
          key = AnchorKeyRightShift;
        }
        else if (scanCode == 0x2a)
        {
          key = AnchorKeyLeftShift;
        }
        else
        {
          /* Must be a combination SHIFT (Left or Right) + a Key
           * Ignore this as the next message will contain
           * the desired "Key" */
          key = AnchorKeyUnknown;
        }
        break;
      case VK_CONTROL:
        key = (extend) ? AnchorKeyRightControl : AnchorKeyLeftControl;
        break;
      case VK_MENU:
        key = (extend) ? AnchorKeyRightAlt : AnchorKeyLeftAlt;
        break;
      case VK_LWIN:
      case VK_RWIN:
        key = AnchorKeyOS;
        break;
      case VK_APPS:
        key = AnchorKeyApp;
        break;
      case VK_NUMLOCK:
        key = AnchorKeyNumLock;
        break;
      case VK_SCROLL:
        key = AnchorKeyScrollLock;
        break;
      case VK_CAPITAL:
        key = AnchorKeyCapsLock;
        break;
      case VK_OEM_8:
        key = ((AnchorSystemWin32 *)getSystem())->processSpecialKey(vKey, scanCode);
        break;
      case VK_MEDIA_PLAY_PAUSE:
        key = AnchorKeyMediaPlay;
        break;
      case VK_MEDIA_STOP:
        key = AnchorKeyMediaStop;
        break;
      case VK_MEDIA_PREV_TRACK:
        key = AnchorKeyMediaFirst;
        break;
      case VK_MEDIA_NEXT_TRACK:
        key = AnchorKeyMediaLast;
        break;
      default:
        key = AnchorKeyUnknown;
        break;
    }
  }

  return key;
}

eAnchorStatus AnchorWindowWin32::getPointerInfo(std::vector<AnchorBackendWin32PointerInfo> &outPointerInfo,
                                                WPARAM wParam,
                                                LPARAM lParam)
{
  AnchorS32 pointerId = GET_POINTERID_WPARAM(wParam);
  AnchorS32 isPrimary = IS_POINTER_PRIMARY_WPARAM(wParam);
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)AnchorSystem::getSystem();
  AnchorU32 outCount = 0;

  if (!(GetPointerPenInfoHistory(pointerId, &outCount, NULL)))
  {
    return ANCHOR_FAILURE;
  }

  std::vector<POINTER_PEN_INFO> pointerPenInfo(outCount);
  outPointerInfo.resize(outCount);

  if (!(GetPointerPenInfoHistory(pointerId, &outCount, pointerPenInfo.data())))
  {
    return ANCHOR_FAILURE;
  }

  for (AnchorU32 i = 0; i < outCount; i++)
  {
    POINTER_INFO pointerApiInfo = pointerPenInfo[i].pointerInfo;
    // Obtain the basic information from the event
    outPointerInfo[i].pointerId = pointerId;
    outPointerInfo[i].isPrimary = isPrimary;

    switch (pointerApiInfo.ButtonChangeType)
    {
      case POINTER_CHANGE_FIRSTBUTTON_DOWN:
      case POINTER_CHANGE_FIRSTBUTTON_UP:
        outPointerInfo[i].buttonMask = ANCHOR_ButtonMaskLeft;
        break;
      case POINTER_CHANGE_SECONDBUTTON_DOWN:
      case POINTER_CHANGE_SECONDBUTTON_UP:
        outPointerInfo[i].buttonMask = ANCHOR_ButtonMaskRight;
        break;
      case POINTER_CHANGE_THIRDBUTTON_DOWN:
      case POINTER_CHANGE_THIRDBUTTON_UP:
        outPointerInfo[i].buttonMask = ANCHOR_ButtonMaskMiddle;
        break;
      case POINTER_CHANGE_FOURTHBUTTON_DOWN:
      case POINTER_CHANGE_FOURTHBUTTON_UP:
        outPointerInfo[i].buttonMask = ANCHOR_ButtonMaskButton4;
        break;
      case POINTER_CHANGE_FIFTHBUTTON_DOWN:
      case POINTER_CHANGE_FIFTHBUTTON_UP:
        outPointerInfo[i].buttonMask = ANCHOR_ButtonMaskButton5;
        break;
      default:
        break;
    }

    outPointerInfo[i].pixelLocation = pointerApiInfo.ptPixelLocation;
    outPointerInfo[i].tabletData.Active = AnchorTabletModeStylus;
    outPointerInfo[i].tabletData.Pressure = 1.0f;
    outPointerInfo[i].tabletData.Xtilt = 0.0f;
    outPointerInfo[i].tabletData.Ytilt = 0.0f;
    outPointerInfo[i].time = system->performanceCounterToMillis(pointerApiInfo.PerformanceCount);

    if (pointerPenInfo[i].penMask & PEN_MASK_PRESSURE)
    {
      outPointerInfo[i].tabletData.Pressure = pointerPenInfo[i].pressure / 1024.0f;
    }

    if (pointerPenInfo[i].penFlags & PEN_FLAG_ERASER)
    {
      outPointerInfo[i].tabletData.Active = AnchorTabletModeEraser;
    }

    if (pointerPenInfo[i].penMask & PEN_MASK_TILT_X)
    {
      outPointerInfo[i].tabletData.Xtilt = fmin(fabs(pointerPenInfo[i].tiltX / 90.0f), 1.0f);
    }

    if (pointerPenInfo[i].penMask & PEN_MASK_TILT_Y)
    {
      outPointerInfo[i].tabletData.Ytilt = fmin(fabs(pointerPenInfo[i].tiltY / 90.0f), 1.0f);
    }
  }

  if (!outPointerInfo.empty())
  {
    m_lastPointerTabletData = outPointerInfo.back().tabletData;
  }

  return ANCHOR_SUCCESS;
}

/**
 * @note this function can be extended to include other exotic cases as they arise. */
eAnchorKey AnchorSystemWin32::processSpecialKey(short vKey, short scanCode) const
{
  eAnchorKey key = AnchorKeyUnknown;
  switch (PRIMARYLANGID(m_langId))
  {
    case LANG_FRENCH:
      if (vKey == VK_OEM_8)
        key = AnchorKeyF13;  // oem key; used purely for shortcuts .
      break;
    case LANG_ENGLISH:
      if (SUBLANGID(m_langId) == SUBLANG_ENGLISH_UK && vKey == VK_OEM_8)  // "`¬"
        key = AnchorKeyAccentGrave;
      break;
  }

  return key;
}

AnchorEvent *AnchorSystemWin32::processWindowEvent(eAnchorEventType type,
                                                   AnchorWindowWin32 *window)
{
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();

  if (type == AnchorEventTypeWindowActivate)
  {
    system->getWindowManager()->setActiveWindow(window);
  }

  return new AnchorEvent(ANCHOR::GetTime(), type, window);
}

void AnchorSystemWin32::processWheelEvent(AnchorWindowWin32 *window, WPARAM wParam, LPARAM lParam)
{
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();

  int acc = system->m_wheelDeltaAccum;
  int delta = GET_WHEEL_DELTA_WPARAM(wParam);

  if (acc * delta < 0)
  {
    // scroll direction reversed.
    acc = 0;
  }
  acc += delta;
  int direction = (acc >= 0) ? 1 : -1;
  acc = abs(acc);

  while (acc >= WHEEL_DELTA)
  {
    system->pushEvent(new AnchorEventWheel(ANCHOR::GetTime(), window, direction));
    acc -= WHEEL_DELTA;
  }
  system->m_wheelDeltaAccum = acc * direction;
}

void AnchorSystemWin32::processPointerEvent(UINT type, AnchorWindowWin32 *window, WPARAM wParam, LPARAM lParam, bool &eventHandled)
{
  /* Pointer events might fire when changing windows for a device which is set to use Wintab,
   * even when Wintab is left enabled but set to the bottom of Wintab overlap order. */
  // if (!window->usingTabletAPI(AnchorTabletWinPointer)) {
  //   return;
  // }

  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();
  std::vector<AnchorBackendWin32PointerInfo> pointerInfo;

  if (window->getPointerInfo(pointerInfo, wParam, lParam) != ANCHOR_SUCCESS)
  {
    return;
  }

  switch (type)
  {
    case WM_POINTERUPDATE:
      /* Coalesced pointer events are reverse chronological order, reorder chronologically.
       * Only contiguous move events are coalesced. */
      for (AnchorU32 i = pointerInfo.size(); i-- > 0;)
      {
        system->pushEvent(new AnchorEventCursor(pointerInfo[i].time,
                                                AnchorEventTypeCursorMove,
                                                window,
                                                pointerInfo[i].pixelLocation.x,
                                                pointerInfo[i].pixelLocation.y,
                                                pointerInfo[i].tabletData));
      }

      /* Leave event unhandled so that system cursor is moved. */

      break;
    case WM_POINTERDOWN:
      /* Move cursor to point of contact because AnchorEventButton does not include position. */
      system->pushEvent(new AnchorEventCursor(pointerInfo[0].time,
                                              AnchorEventTypeCursorMove,
                                              window,
                                              pointerInfo[0].pixelLocation.x,
                                              pointerInfo[0].pixelLocation.y,
                                              pointerInfo[0].tabletData));
      system->pushEvent(new AnchorEventButton(pointerInfo[0].time,
                                              AnchorEventTypeButtonDown,
                                              window,
                                              pointerInfo[0].buttonMask,
                                              pointerInfo[0].tabletData));
      window->updateMouseCapture(MousePressed);

      /* Mark event handled so that mouse button events are not generated. */
      eventHandled = true;

      break;
    case WM_POINTERUP:
      system->pushEvent(new AnchorEventButton(pointerInfo[0].time,
                                              AnchorEventTypeButtonUp,
                                              window,
                                              pointerInfo[0].buttonMask,
                                              pointerInfo[0].tabletData));
      window->updateMouseCapture(MouseReleased);

      /* Mark event handled so that mouse button events are not generated. */
      eventHandled = true;

      break;
    default:
      break;
  }
}

AnchorEventCursor *AnchorSystemWin32::processCursorEvent(AnchorWindowWin32 *window)
{
  AnchorS32 x_screen, y_screen;
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();

  if (window->getTabletData().Active != AnchorTabletModeNone)
  {
    /* While pen devices are in range, cursor movement is handled by tablet input processing. */
    return NULL;
  }

  system->getCursorPosition(x_screen, y_screen);

  if (window->getCursorGrabModeIsWarp())
  {
    AnchorS32 x_new = x_screen;
    AnchorS32 y_new = y_screen;
    AnchorS32 x_accum, y_accum;
    AnchorRect bounds;

    /* Fallback to window bounds. */
    if (window->getCursorGrabBounds(bounds) == ANCHOR_FAILURE)
    {
      window->getClientBounds(bounds);
    }

    /* Could also clamp to screen bounds wrap with a window outside the view will fail atm.
     * Use inset in case the window is at screen bounds. */
    bounds.wrapPoint(x_new, y_new, 2, window->getCursorGrabAxis());

    window->getCursorGrabAccum(x_accum, y_accum);
    if (x_new != x_screen || y_new != y_screen)
    {
      /* When wrapping we don't need to add an event because the setCursorPosition call will cause
       * a new event after. */
      system->setCursorPosition(x_new, y_new); /* wrap */
      window->setCursorGrabAccum(x_accum + (x_screen - x_new), y_accum + (y_screen - y_new));
    }
    else
    {
      return new AnchorEventCursor(ANCHOR::GetTime(),
                                   AnchorEventTypeCursorMove,
                                   window,
                                   x_screen + x_accum,
                                   y_screen + y_accum,
                                   ANCHOR_TABLET_DATA_NONE);
    }
  }
  else
  {
    return new AnchorEventCursor(ANCHOR::GetTime(),
                                 AnchorEventTypeCursorMove,
                                 window,
                                 x_screen,
                                 y_screen,
                                 ANCHOR_TABLET_DATA_NONE);
  }
  return NULL;
}

AnchorEventButton *AnchorSystemWin32::processButtonEvent(eAnchorEventType type,
                                                         AnchorWindowWin32 *window,
                                                         eAnchorButtonMask mask)
{
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();

  AnchorTabletData td = window->getTabletData();

  /* Move mouse to button event position. */
  if (window->getTabletData().Active != AnchorTabletModeNone)
  {
    /* Tablet should be handling in between mouse moves, only move to event position. */
    DWORD msgPos = ::GetMessagePos();
    int msgPosX = GET_X_LPARAM(msgPos);
    int msgPosY = GET_Y_LPARAM(msgPos);
    system->pushEvent(new AnchorEventCursor(
      ::GetMessageTime(), AnchorEventTypeCursorMove, window, msgPosX, msgPosY, td));
  }

  window->updateMouseCapture(type == AnchorEventTypeButtonDown ? MousePressed : MouseReleased);
  return new AnchorEventButton(ANCHOR::GetTime(), type, window, mask, td);
}

eAnchorKey AnchorSystemWin32::hardKey(RAWINPUT const &raw, bool *r_keyDown, bool *r_is_repeated_modifier)
{
  bool is_repeated_modifier = false;

  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();
  eAnchorKey key = AnchorKeyUnknown;
  AnchorModifierKeys modifiers;
  system->retrieveModifierKeys(modifiers);

  // RI_KEY_BREAK doesn't work for sticky keys release, so we also
  // check for the up message
  unsigned int msg = raw.data.keyboard.Message;
  *r_keyDown = !(raw.data.keyboard.Flags & RI_KEY_BREAK) && msg != WM_KEYUP && msg != WM_SYSKEYUP;

  key = this->convertKey(raw.data.keyboard.VKey,
                         raw.data.keyboard.MakeCode,
                         (raw.data.keyboard.Flags & (RI_KEY_E1 | RI_KEY_E0)));

  // extra handling of modifier keys: don't send repeats out from ANCHOR
  if (key >= AnchorKeyLeftShift && key <= AnchorKeyRightAlt)
  {
    bool changed = false;
    eAnchorModifierKeyMask modifier;
    switch (key)
    {
      case AnchorKeyLeftShift: {
        changed = (modifiers.get(ANCHOR_ModifierKeyLeftShift) != *r_keyDown);
        modifier = ANCHOR_ModifierKeyLeftShift;
        break;
      }
      case AnchorKeyRightShift: {
        changed = (modifiers.get(ANCHOR_ModifierKeyRightShift) != *r_keyDown);
        modifier = ANCHOR_ModifierKeyRightShift;
        break;
      }
      case AnchorKeyLeftControl: {
        changed = (modifiers.get(ANCHOR_ModifierKeyLeftControl) != *r_keyDown);
        modifier = ANCHOR_ModifierKeyLeftControl;
        break;
      }
      case AnchorKeyRightControl: {
        changed = (modifiers.get(ANCHOR_ModifierKeyRightControl) != *r_keyDown);
        modifier = ANCHOR_ModifierKeyRightControl;
        break;
      }
      case AnchorKeyLeftAlt: {
        changed = (modifiers.get(ANCHOR_ModifierKeyLeftAlt) != *r_keyDown);
        modifier = ANCHOR_ModifierKeyLeftAlt;
        break;
      }
      case AnchorKeyRightAlt: {
        changed = (modifiers.get(ANCHOR_ModifierKeyRightAlt) != *r_keyDown);
        modifier = ANCHOR_ModifierKeyRightAlt;
        break;
      }
      default:
        break;
    }

    if (changed)
    {
      modifiers.set(modifier, *r_keyDown);
      system->storeModifierKeys(modifiers);
    }
    else
    {
      is_repeated_modifier = true;
    }
  }

  *r_is_repeated_modifier = is_repeated_modifier;
  return key;
}

AnchorEvent *AnchorSystemWin32::processWindowSizeEvent(AnchorWindowWin32 *window)
{
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();
  AnchorEvent *sizeEvent = new AnchorEvent(
    ANCHOR::GetTime(), AnchorEventTypeWindowSize, window);

  /* We get WM_SIZE before we fully init. Do not dispatch before we are continuously resizing. */
  if (window->m_inLiveResize)
  {
    system->pushEvent(sizeEvent);
    system->dispatchEvents();
    return NULL;
  }
  else
  {
    return sizeEvent;
  }
}

AnchorEventKey *AnchorSystemWin32::processKeyEvent(AnchorWindowWin32 *window, RAWINPUT const &raw)
{
  bool keyDown = false;
  bool is_repeated_modifier = false;
  AnchorSystemWin32 *system = (AnchorSystemWin32 *)getSystem();
  eAnchorKey key = system->hardKey(raw, &keyDown, &is_repeated_modifier);
  AnchorEventKey *event;

  /* We used to check `if (key != AnchorKeyUnknown)`, but since the message
   * values `WM_SYSKEYUP`, `WM_KEYUP` and `WM_CHAR` are ignored, we capture
   * those events here as well. */
  if (!is_repeated_modifier)
  {
    char vk = raw.data.keyboard.VKey;
    char utf8_char[6] = {0};
    char ascii = 0;
    bool is_repeat = false;

    /* Unlike on Linux, not all keys can send repeat events. E.g. modifier keys don't. */
    if (keyDown)
    {
      if (system->m_keycode_last_repeat_key == vk)
      {
        is_repeat = true;
      }
      system->m_keycode_last_repeat_key = vk;
    }
    else
    {
      if (system->m_keycode_last_repeat_key == vk)
      {
        system->m_keycode_last_repeat_key = 0;
      }
    }

    wchar_t utf16[3] = {0};
    BYTE state[256] = {0};
    int r;
    GetKeyboardState((PBYTE)state);
    bool ctrl_pressed = state[VK_CONTROL] & 0x80;
    bool alt_pressed = state[VK_MENU] & 0x80;

    /* No text with control key pressed (Alt can be used to insert special characters though!). */
    if (ctrl_pressed && !alt_pressed)
    {
      utf8_char[0] = '\0';
    }
    // Don't call ToUnicodeEx on dead keys as it clears the buffer and so won't allow diacritical
    // composition.
    else if (MapVirtualKeyW(vk, 2) != 0)
    {
      // todo: ToUnicodeEx can respond with up to 4 utf16 chars (only 2 here).
      // Could be up to 24 utf8 bytes.
      if ((r = ToUnicodeEx(
             vk, raw.data.keyboard.MakeCode, state, utf16, 2, 0, system->m_keylayout)))
      {
        if ((r > 0 && r < 3))
        {
          utf16[r] = 0;
          conv_utf_16_to_8(utf16, utf8_char, 6);
        }
        else if (r == -1)
        {
          utf8_char[0] = '\0';
        }
      }
    }

    if (!keyDown)
    {
      utf8_char[0] = '\0';
      ascii = '\0';
    }
    else
    {
      ascii = utf8_char[0] & 0x80 ? '?' : utf8_char[0];
    }

    event = new AnchorEventKey(ANCHOR::GetTime(),
                               keyDown ? AnchorEventTypeKeyDown : AnchorEventTypeKeyUp,
                               window,
                               key,
                               ascii,
                               utf8_char,
                               is_repeat);
  }
  else
  {
    event = NULL;
  }

  return event;
}

AnchorU64 AnchorSystemWin32::performanceCounterToMillis(__int64 perf_ticks) const
{
  // Calculate the time passed since system initialization.
  __int64 delta = (perf_ticks - m_start) * 1000;

  AnchorU64 t = (AnchorU64)(delta / m_freq);
  return t;
}

AnchorU64 AnchorSystemWin32::tickCountToMillis(__int64 ticks) const
{
  return ticks - m_lfstart;
}

AnchorU64 AnchorSystemWin32::getMilliSeconds() const
{
  // Hardware does not support high resolution timers. We will use GetTickCount instead then.
  if (!m_hasPerformanceCounter)
  {
    return tickCountToMillis(::GetTickCount());
  }

  // Retrieve current count
  __int64 count = 0;
  ::QueryPerformanceCounter((LARGE_INTEGER *)&count);

  return performanceCounterToMillis(count);
}

//---------------------------------------------------------------------------------------------------------

const wchar_t *AnchorWindowWin32::s_windowClassName = L"AnchorWindowClass";
const int AnchorWindowWin32::s_maxTitleLength = 128;

/* force NVidia Optimus to use dedicated graphics */
extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

AnchorWindowWin32::AnchorWindowWin32(AnchorSystemWin32 *system,
                                     const char *title,
                                     const char *icon,
                                     AnchorS32 left,
                                     AnchorS32 top,
                                     AnchorU32 width,
                                     AnchorU32 height,
                                     eAnchorWindowState state,
                                     eAnchorDrawingContextType type,
                                     bool alphaBackground,
                                     AnchorWindowWin32 *parentWindow,
                                     bool dialog)
  : AnchorSystemWindow(width, height, state, false, false),
    m_mousePresent(false),
    m_inLiveResize(false),
    m_system(system),
    m_hDC(0),
    m_isDialog(dialog),
    m_hasMouseCaptured(false),
    m_hasGrabMouse(false),
    m_nPressedButtons(0),
    m_vulkan_context(nullptr),
    m_customCursor(0),
    m_wantAlphaBackground(alphaBackground),
    m_lastPointerTabletData(ANCHOR_TABLET_DATA_NONE),
    m_normal_state(AnchorWindowStateNormal),
    m_user32(::LoadLibrary("user32.dll")),
    m_parentWindowHwnd(parentWindow ? parentWindow->m_hWnd : HWND_DESKTOP),
    m_hgi(nullptr),
    m_vkinstance(nullptr),
    m_device(nullptr),
    m_commandQueue(nullptr),
    m_pipelineCache(nullptr),
    m_instance(nullptr)
{
  DWORD style = parentWindow ?
                  WS_POPUPWINDOW | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX :
                  WS_OVERLAPPEDWINDOW;

  if (state == AnchorWindowStateFullScreen)
  {
    style |= WS_MAXIMIZE;
  }

  /* Forces owned windows onto taskbar and allows minimization. */
  DWORD extended_style = parentWindow ? WS_EX_APPWINDOW : 0;

  RECT win_rect = {left, top, (long)(left + width), (long)(top + height)};
  adjustWindowRectForClosestMonitor(&win_rect, style, extended_style);

  wchar_t *title_16 = alloc_utf16_from_8((char *)title, 0);
  m_hWnd = ::CreateWindowExW(extended_style,                  // window extended style
                             s_windowClassName,               // pointer to registered class name
                             title_16,                        // pointer to window name
                             style,                           // window style
                             win_rect.left,                   // horizontal position of window
                             win_rect.top,                    // vertical position of window
                             win_rect.right - win_rect.left,  // window width
                             win_rect.bottom - win_rect.top,  // window height
                             m_parentWindowHwnd,              // handle to parent or owner window
                             0,                               // handle to menu or child-window identifier
                             ::GetModuleHandle(0),            // handle to application instance
                             0);                              // pointer to window-creation data
  free(title_16);

  if (m_hWnd == NULL)
  {
    return;
  }

  /*  Store the device context. */
  m_hDC = ::GetDC(m_hWnd);

  if (!setDrawingContextType(type))
  {
    ::DestroyWindow(m_hWnd);
    m_hWnd = NULL;
    return;
  }

  RegisterTouchWindow(m_hWnd, 0);

  /* Register as drop-target. #OleInitialize(0) required first. */
  // m_dropTarget = new ANCHOR_DropTargetWin32(this, m_system);
  // ::RegisterDragDrop(m_hWnd, m_dropTarget);

  /* Store a pointer to this class in the window structure. */
  ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

  if (!m_system->m_windowFocus)
  {
    /* If we don't want focus then lower to bottom. */
    ::SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  }

  /* Show the window. */
  int nCmdShow;
  switch (state)
  {
    case AnchorWindowStateMaximized:
      nCmdShow = SW_SHOWMAXIMIZED;
      break;
    case AnchorWindowStateMinimized:
      nCmdShow = (m_system->m_windowFocus) ? SW_SHOWMINIMIZED : SW_SHOWMINNOACTIVE;
      break;
    case AnchorWindowStateNormal:
    default:
      nCmdShow = (m_system->m_windowFocus) ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE;
      break;
  }

  ::ShowWindow(m_hWnd, nCmdShow);

  if (m_wantAlphaBackground && m_parentWindowHwnd == 0)
  {
    HRESULT hr = S_OK;

    /* Create and populate the Blur Behind structure. */
    DWM_BLURBEHIND bb = {0};

    /* Enable Blur Behind and apply to the entire client area. */
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = true;
    bb.hRgnBlur = CreateRectRgn(0, 0, -1, -1);

    /* Apply Blur Behind. */
    hr = DwmEnableBlurBehindWindow(m_hWnd, &bb);
    DeleteObject(bb.hRgnBlur);
  }

  /* Force an initial paint of the window. */
  ::UpdateWindow(m_hWnd);

  /* Initialize Wintab. */
  // if (system->getTabletAPI() != AnchorTabletWinPointer) {
  // loadWintab(AnchorWindowStateMinimized != state);
  // }

  /* Allow the showing of a progress bar on the taskbar. */
  CoCreateInstance(CLSID_TaskbarList,
                   NULL,
                   CLSCTX_INPROC_SERVER,
                   IID_ITaskbarList3,
                   (LPVOID *)&m_Bar);
}

AnchorWindowWin32::~AnchorWindowWin32()
{
  if (m_Bar)
  {
    m_Bar->SetProgressState(m_hWnd, TBPF_NOPROGRESS);
    m_Bar->Release();
    m_Bar = NULL;
  }

  // closeWintab();

  if (m_user32)
  {
    FreeLibrary(m_user32);
    m_user32 = NULL;
  }

  if (m_customCursor)
  {
    DestroyCursor(m_customCursor);
    m_customCursor = NULL;
  }

  if (m_hWnd != NULL && m_hDC != NULL && releaseNativeHandles())
  {
    ::ReleaseDC(m_hWnd, m_hDC);
    m_hDC = NULL;
  }

  if (m_hWnd)
  {
    /* If this window is referenced by others as parent, clear that relation or windows will free
     * the handle while we still reference it. */
    for (AnchorISystemWindow *iter_win : m_system->getWindowManager()->getWindows())
    {
      AnchorWindowWin32 *iter_winwin = (AnchorWindowWin32 *)iter_win;
      if (iter_winwin->m_parentWindowHwnd == m_hWnd)
      {
        ::SetWindowLongPtr(iter_winwin->m_hWnd, GWLP_HWNDPARENT, NULL);
        iter_winwin->m_parentWindowHwnd = 0;
      }
    }

    // if (m_dropTarget) {
    //   // Disable DragDrop
    //   RevokeDragDrop(m_hWnd);
    //   // Release our reference of the DropTarget and it will delete itself eventually.
    //   m_dropTarget->Release();
    //   m_dropTarget = NULL;
    // }
    ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
    ::DestroyWindow(m_hWnd);
    m_hWnd = 0;
  }
}

eAnchorStatus AnchorWindowWin32::releaseNativeHandles()
{
  m_hWnd = NULL;
  m_hDC = NULL;

  return ANCHOR_SUCCESS;
}

void AnchorWindowWin32::adjustWindowRectForClosestMonitor(LPRECT win_rect,
                                                          DWORD dwStyle,
                                                          DWORD dwExStyle)
{
  /* Get Details of the closest monitor. */
  HMONITOR hmonitor = MonitorFromRect(win_rect, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEX monitor;
  monitor.cbSize = sizeof(MONITORINFOEX);
  monitor.dwFlags = 0;
  GetMonitorInfo(hmonitor, &monitor);

  /* Constrain requested size and position to fit within this monitor. */
  LONG width = winmin(monitor.rcWork.right - monitor.rcWork.left, win_rect->right - win_rect->left);
  LONG height = winmin(monitor.rcWork.bottom - monitor.rcWork.top, win_rect->bottom - win_rect->top);
  win_rect->left = winmin(winmax(monitor.rcWork.left, win_rect->left), monitor.rcWork.right - width);
  win_rect->right = win_rect->left + width;
  win_rect->top = winmin(winmax(monitor.rcWork.top, win_rect->top), monitor.rcWork.bottom - height);
  win_rect->bottom = win_rect->top + height;

  /* With Windows 10 and newer we can adjust for chrome that differs with DPI and scale. */
  AnchorAdjustWindowRectExForDpiCallback fpAdjustWindowRectExForDpi = nullptr;
  if (m_user32)
  {
    fpAdjustWindowRectExForDpi = (AnchorAdjustWindowRectExForDpiCallback)::GetProcAddress(
      m_user32, "AdjustWindowRectExForDpi");
  }

  /* Adjust to allow for caption, borders, shadows, scaling, etc. Resulting values can be
   * correctly outside of monitor bounds. Note: You cannot specify WS_OVERLAPPED when calling. */
  if (fpAdjustWindowRectExForDpi)
  {
    UINT dpiX, dpiY;
    GetDpiForMonitor(hmonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    fpAdjustWindowRectExForDpi(win_rect, dwStyle & ~WS_OVERLAPPED, FALSE, dwExStyle, dpiX);
  }
  else
  {
    AdjustWindowRectEx(win_rect, dwStyle & ~WS_OVERLAPPED, FALSE, dwExStyle);
  }

  /* But never allow a top position that can hide part of the title bar. */
  win_rect->top = winmax(monitor.rcWork.top, win_rect->top);
}

bool AnchorWindowWin32::getValid() const
{
  return AnchorSystemWindow::getValid() && m_hWnd != 0 && m_hDC != 0;
}

HWND AnchorWindowWin32::getHWND() const
{
  return m_hWnd;
}

void AnchorWindowWin32::resetPointerPenInfo()
{
  m_lastPointerTabletData = ANCHOR_TABLET_DATA_NONE;
}

AnchorTabletData AnchorWindowWin32::getTabletData()
{
  // if (usingTabletAPI(AnchorTabletWintab)) {
  //   return m_wintab ? m_wintab->getLastTabletData() : ANCHOR_TABLET_DATA_NONE;
  // }
  // else {
  return m_lastPointerTabletData;
  // }
}

void AnchorWindowWin32::getClientBounds(AnchorRect &bounds) const
{
  RECT rect;
  POINT coord;
  if (!IsIconic(m_hWnd))
  {
    ::GetClientRect(m_hWnd, &rect);

    coord.x = rect.left;
    coord.y = rect.top;
    ::ClientToScreen(m_hWnd, &coord);

    bounds.m_l = coord.x;
    bounds.m_t = coord.y;

    coord.x = rect.right;
    coord.y = rect.bottom;
    ::ClientToScreen(m_hWnd, &coord);

    bounds.m_r = coord.x;
    bounds.m_b = coord.y;
  }
  else
  {
    bounds.m_b = 0;
    bounds.m_l = 0;
    bounds.m_r = 0;
    bounds.m_t = 0;
  }
}

void AnchorWindowWin32::setTitle(const char *title)
{
  wchar_t *title_16 = alloc_utf16_from_8((char *)title, 0);
  ::SetWindowTextW(m_hWnd, (wchar_t *)title_16);
  free(title_16);
}

std::string AnchorWindowWin32::getTitle() const
{
  std::wstring wtitle(::GetWindowTextLengthW(m_hWnd) + 1, L'\0');
  ::GetWindowTextW(m_hWnd, &wtitle[0], wtitle.capacity());

  std::string title(count_utf_8_from_16(wtitle.c_str()) + 1, '\0');
  conv_utf_16_to_8(wtitle.c_str(), &title[0], title.capacity());

  return title;
}

void AnchorWindowWin32::setIcon(const char *icon)
{}

void AnchorWindowWin32::getWindowBounds(AnchorRect &bounds) const
{
  RECT rect;
  ::GetWindowRect(m_hWnd, &rect);
  bounds.m_b = rect.bottom;
  bounds.m_l = rect.left;
  bounds.m_r = rect.right;
  bounds.m_t = rect.top;
}

static void check_vk_result(VkResult err)
{
  if (err == VK_ERROR_INCOMPATIBLE_DRIVER)
  {

    TF_ERROR_MSG(
      "Cannot find a compatible Vulkan installable client"
      "driver (ICD). Please make sure your driver supports"
      "Vulkan before continuing. The vulkan call which has"
      "issued a crash was:  vkCreateInstance()");

    fflush(stdout);
    exit(ANCHOR_FAILURE);
  }
  else if (err != VK_SUCCESS)
  {

    TF_ERROR_MSG(
      "The call to vkCreateInstance failed. Please make"
      "sure you have a Vulkan installable client driver"
      "(ICD) before continuing.");

    fflush(stdout);
    exit(ANCHOR_FAILURE);
  }
}

void AnchorWindowWin32::SetupVulkan()
{
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = NULL;
  app_info.apiVersion = VK_VERSION_1_2;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = NULL;
  create_info.flags = 0;
  create_info.pApplicationInfo = &app_info;

  /**
   * ------------------------------------------- Setup VULKAN extensions ----- */

  std::vector<const char *> instance_exts{
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
  };

  if (HgiVulkanIsDebugEnabled())
  {
    instance_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    const char *debugLayers[] = {"VK_LAYER_KHRONOS_validation"};
    create_info.ppEnabledLayerNames = debugLayers;
    create_info.enabledLayerCount = (uint32_t)TfArraySize(debugLayers);
  }

  create_info.ppEnabledExtensionNames = instance_exts.data();
  create_info.enabledExtensionCount = (uint32_t)instance_exts.size();

  AnchorRect winrect;
  getWindowBounds(winrect);

  /**
   * ------------------------------------------- Create Vulkan Instance ----- */

  VkResult err;
  err = vkCreateInstance(&create_info, HgiVulkanAllocator(), &m_instance);
  m_hgi = new HgiVulkan(m_vkinstance = new HgiVulkanInstance(m_instance));
  check_vk_result(err);

  VkSurfaceKHR surface;

  VkWin32SurfaceCreateInfoKHR surface_create_info = {};
  surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surface_create_info.pNext = NULL;
  surface_create_info.flags = 0;
  surface_create_info.hinstance = GetModuleHandle(0);
  surface_create_info.hwnd = m_hWnd;

  err = vkCreateWin32SurfaceKHR(m_instance, &surface_create_info, HgiVulkanAllocator(), &surface);
  ANCHOR_ASSERT(err == VK_SUCCESS);

  m_vulkan_context = new ANCHOR_VulkanGPU_Surface();
  m_vulkan_context->Surface = surface;

  /**
   * Pixar Device */
  m_device = m_hgi->GetPrimaryDevice();

  uint32_t queueCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_device->GetVulkanPhysicalDevice(), &queueCount, NULL);
  assert(queueCount >= 1);

  std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(m_device->GetVulkanPhysicalDevice(), &queueCount, queueProperties.data());

  uint32_t queueIndex;

  std::vector<VkBool32> supportsPresenting(queueCount);
  for (uint32_t i = 0; i < queueCount; i++)
  {
    vkGetPhysicalDeviceSurfaceSupportKHR(m_device->GetVulkanPhysicalDevice(), i, surface, &supportsPresenting[i]);

    if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
    {
      if (supportsPresenting[i] == VK_TRUE)
      {
        queueIndex = i;
        break;
      }
    }
  }
  assert(queueIndex != UINT32_MAX);

  /**
   * Select Surface Format. */
  uint32_t formatCount = 0;
  err = vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->GetVulkanPhysicalDevice(), surface, &formatCount, NULL);
  assert(err == VK_SUCCESS);
  assert(formatCount >= 1);

  std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
  err = vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->GetVulkanPhysicalDevice(),
                                             surface,
                                             &formatCount,
                                             surfaceFormats.data());
  assert(err == VK_SUCCESS);

  VkFormat colorFormat;
  VkColorSpaceKHR colorSpace;

  if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
  {
    colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
  }
  else
  {
    colorFormat = surfaceFormats[0].format;
  }

  colorSpace = surfaceFormats[0].colorSpace;


  m_vulkan_context->SurfaceFormat = surfaceFormats[0];

  VkSurfaceCapabilitiesKHR caps = {};
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->GetVulkanPhysicalDevice(), surface, &caps);
  assert(result == VK_SUCCESS);

  VkExtent2D swapchainExtent = {};

  AnchorRect rectBounds;
  getWindowBounds(rectBounds);
  if (caps.currentExtent.width == -1 || caps.currentExtent.height == -1)
  {
    swapchainExtent.width = rectBounds.getWidth();
    swapchainExtent.height = rectBounds.getHeight();
  }
  else
  {
    swapchainExtent = caps.currentExtent;
  }

  uint32_t presentModeCount = 0;
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->GetVulkanPhysicalDevice(),
                                                     surface,
                                                     &presentModeCount,
                                                     NULL);
  assert(result == VK_SUCCESS);
  assert(presentModeCount >= 1);

  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->GetVulkanPhysicalDevice(),
                                                     surface,
                                                     &presentModeCount,
                                                     presentModes.data());

  assert(result == VK_SUCCESS);

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
  for (uint32_t i = 0; i < presentModeCount; i++)
  {
    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }
    if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
      presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
  }

  assert(caps.maxImageCount >= 1);
  uint32_t imageCount = caps.minImageCount + 1;
  if (imageCount > caps.maxImageCount)
  {
    imageCount = caps.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = imageCount;
  swapchainCreateInfo.imageFormat = colorFormat;
  swapchainCreateInfo.imageColorSpace = colorSpace;
  swapchainCreateInfo.imageExtent = {swapchainExtent.width, swapchainExtent.height};
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.queueFamilyIndexCount = 1;
  swapchainCreateInfo.pQueueFamilyIndices = {0};
  swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;

  m_vulkan_context->ImageCount = imageCount;
  m_vulkan_context->PresentMode = presentMode;

  if (TfDebug::IsEnabled(ANCHOR_WIN32))
  {
    TF_SUCCESS_MSG("Anchor -- Rendering at maximum possible frames per second.");
    TF_MSG("Anchor -- Selected PresentMode = %d", m_vulkan_context->PresentMode);
  }

  VkSwapchainKHR swapchain;
  result = vkCreateSwapchainKHR(m_device->GetVulkanDevice(), &swapchainCreateInfo, NULL, &swapchain);
  assert(result == VK_SUCCESS);

  m_vulkan_context->Swapchain = swapchain;

  result = vkGetSwapchainImagesKHR(m_device->GetVulkanDevice(), m_vulkan_context->Swapchain, &m_vulkan_context->ImageCount, NULL);
  assert(result == VK_SUCCESS);
  assert(m_vulkan_context->ImageCount > 0);

  struct SwapChainBuffer
  {
    VkImage image;
    VkImageView view;
    VkFramebuffer frameBuffer;
  };

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
    err = vkCreateDescriptorPool(m_device->GetVulkanDevice(),
                                 &pool_info,
                                 HgiVulkanAllocator(),
                                 &g_DescriptorPool);
    check_vk_result(err);
  }

  /**
   * Create SwapChain, RenderPass, Framebuffer, etc. */
  // ANCHOR_ASSERT(getMinImageCount() >= 2);
  // ANCHOR_ImplVulkanH_CreateOrResizeWindow(m_instance,
  //                                         m_device->GetVulkanPhysicalDevice(),
  //                                         m_device->GetVulkanDevice(),
  //                                         m_vulkan_context,
  //                                         m_device->GetGfxQueueFamilyIndex(),
  //                                         HgiVulkanAllocator(),
  //                                         winrect.getWidth(),
  //                                         winrect.getHeight(),
  //                                         getMinImageCount());

  m_commandQueue = m_hgi->GetPrimaryDevice()->GetCommandQueue();
  m_pipelineCache = m_hgi->GetPrimaryDevice()->GetPipelineCache();
}

static void SetFont()
{
  AnchorIO &io = ANCHOR::GetIO();
  io.Fonts->AddFontDefault();

  /* Gotham Font. */
  const static std::string gm_path = STRCAT(G.main->fonts_path, "GothamPro.ttf");
  io.Fonts->AddFontFromFileTTF(CHARALL(gm_path), 11.0f);

  /* Dankmono Font. */
  const static std::string dm_path = STRCAT(G.main->fonts_path, "dankmono.ttf");
  io.Fonts->AddFontFromFileTTF(CHARALL(dm_path), 12.0f);

  /* San Francisco Font (Default). */
  const static std::string sf_path = STRCAT(G.main->fonts_path, "SFProText-Medium.ttf");
  io.FontDefault = io.Fonts->AddFontFromFileTTF(CHARALL(sf_path), 14.0f);
}

void AnchorWindowWin32::SetupVulkanWindow()
{
}

void AnchorWindowWin32::CreateVulkanFontTexture(VkCommandBuffer command_buffer)
{
  AnchorIO &io = ANCHOR::GetIO();

  unsigned char *pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
  size_t upload_size = width * height * 4 * sizeof(char);

  VkResult err;

  // Create the Image:
  {
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = VK_FORMAT_R8G8B8A8_UNORM;
    info.extent.width = width;
    info.extent.height = height;
    info.extent.depth = 1;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    err = vkCreateImage(m_device->GetVulkanDevice(), &info, HgiVulkanAllocator(), &m_fontImage);
    check_vk_result(err);
    VkMemoryRequirements req;
    vkGetImageMemoryRequirements(m_device->GetVulkanDevice(), m_fontImage, &req);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = req.size;
    alloc_info.memoryTypeIndex = GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits); 
    err = vkAllocateMemory(m_device->GetVulkanDevice(), &alloc_info, HgiVulkanAllocator(), &m_fontMemory);
    check_vk_result(err);
    err = vkBindImageMemory(m_device->GetVulkanDevice(), m_fontImage, m_fontMemory, 0);
    check_vk_result(err);
  }

  // Create the Image View:
  {
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = m_fontImage;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = VK_FORMAT_R8G8B8A8_UNORM;
    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.layerCount = 1;
    err = vkCreateImageView(m_device->GetVulkanDevice(), &info, HgiVulkanAllocator(), &m_fontView);
    check_vk_result(err);
  }

  // Update the Descriptor Set:
  {
    VkDescriptorImageInfo desc_image[1] = {};
    desc_image[0].sampler = m_fontSampler;
    desc_image[0].imageView = m_fontView;
    desc_image[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkWriteDescriptorSet write_desc[1] = {};
    write_desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_desc[0].dstSet = g_DescriptorSet;
    write_desc[0].descriptorCount = 1;
    write_desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_desc[0].pImageInfo = desc_image;
    vkUpdateDescriptorSets(m_device->GetVulkanDevice(), 1, write_desc, 0, NULL);
  }

  // Create the Upload Buffer:
  {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = upload_size;
    buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    err = vkCreateBuffer(m_device->GetVulkanDevice(), &buffer_info, HgiVulkanAllocator(), &m_fontUploadBuffer);
    check_vk_result(err);
    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(m_device->GetVulkanDevice(), m_fontUploadBuffer, &req);
    m_bufferMemoryAlignment = (m_bufferMemoryAlignment > req.alignment) ? m_bufferMemoryAlignment : req.alignment;
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = req.size;
    alloc_info.memoryTypeIndex = GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                                      req.memoryTypeBits);
    err = vkAllocateMemory(m_device->GetVulkanDevice(), &alloc_info, HgiVulkanAllocator(), &m_fontUploadBufferMemory);
    check_vk_result(err);
    err = vkBindBufferMemory(m_device->GetVulkanDevice(), m_fontUploadBuffer, m_fontUploadBufferMemory, 0);
    check_vk_result(err);
  }

  // Upload to Buffer:
  {
    char *map = NULL;
    err = vkMapMemory(m_device->GetVulkanDevice(), m_fontUploadBufferMemory, 0, upload_size, 0, (void **)(&map));
    check_vk_result(err);
    memcpy(map, pixels, upload_size);
    VkMappedMemoryRange range[1] = {};
    range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range[0].memory = m_fontUploadBufferMemory;
    range[0].size = upload_size;
    err = vkFlushMappedMemoryRanges(m_device->GetVulkanDevice(), 1, range);
    check_vk_result(err);
    vkUnmapMemory(m_device->GetVulkanDevice(), m_fontUploadBufferMemory);
  }

  // Copy to Image:
  {
    VkImageMemoryBarrier copy_barrier[1] = {};
    copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    copy_barrier[0].image = m_fontImage;
    copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_barrier[0].subresourceRange.levelCount = 1;
    copy_barrier[0].subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(command_buffer,
                        VK_PIPELINE_STAGE_HOST_BIT,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0,
                        0,
                        NULL,
                        0,
                        NULL,
                        1,
                        copy_barrier);

    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;
    vkCmdCopyBufferToImage(command_buffer,
                            m_fontUploadBuffer,
                            m_fontImage,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            1,
                            &region);

    VkImageMemoryBarrier use_barrier[1] = {};
    use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    use_barrier[0].image = m_fontImage;
    use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    use_barrier[0].subresourceRange.levelCount = 1;
    use_barrier[0].subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(command_buffer,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        0,
                        0,
                        NULL,
                        0,
                        NULL,
                        1,
                        use_barrier);
  }

  // Store our identifier
  io.Fonts->SetTexID((AnchorTextureID)(intptr_t)m_fontImage);
}

void AnchorWindowWin32::DestroyVulkanFontTexture()
{
  if (m_fontUploadBuffer)
  {
    vkDestroyBuffer(m_device->GetVulkanDevice(), m_fontUploadBuffer, HgiVulkanAllocator());
    m_fontUploadBuffer = VK_NULL_HANDLE;
  }
  if (m_fontUploadBufferMemory)
  {
    vkFreeMemory(m_device->GetVulkanDevice(), m_fontUploadBufferMemory, HgiVulkanAllocator());
    m_fontUploadBufferMemory = VK_NULL_HANDLE;
  }
}

void AnchorWindowWin32::CreateVulkanDescriptorSetLayout()
{
  VkResult err;

  {
    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.minLod = -1000;
    info.maxLod = 1000;
    info.maxAnisotropy = 1.0f;
    err = vkCreateSampler(m_device->GetVulkanDevice(), &info, HgiVulkanAllocator(), &m_fontSampler);
    check_vk_result(err);
  }

  {
    VkSampler sampler[1] = {m_fontSampler};
    VkDescriptorSetLayoutBinding binding[1] = {};
    binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding[0].descriptorCount = 1;
    binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding[0].pImmutableSamplers = sampler;
    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 1;
    info.pBindings = binding;
    err = vkCreateDescriptorSetLayout(m_device->GetVulkanDevice(), &info, HgiVulkanAllocator(), &g_DescriptorSetLayout);
    check_vk_result(err);
  }

  // Create Descriptor Set:
  {
    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = g_DescriptorPool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &g_DescriptorSetLayout;
    err = vkAllocateDescriptorSets(m_device->GetVulkanDevice(), &alloc_info, &g_DescriptorSet);
    check_vk_result(err);
  }
}

uint32_t AnchorWindowWin32::GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
{
  VkPhysicalDeviceMemoryProperties prop;
  vkGetPhysicalDeviceMemoryProperties(m_device->GetVulkanPhysicalDevice(), &prop);
  for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
      return i;
  return 0xFFFFFFFF;  // Unable to find memoryType
}

void AnchorWindowWin32::newDrawingContext(eAnchorDrawingContextType type)
{
  if (type == ANCHOR_DrawingContextTypeVulkan)
  {
    /**
     * Create Vulkan Instance. */
    SetupVulkan();

    /**
     * Create Vulkan Surface. */
    SetupVulkanWindow();

    /**
     * Setup ANCHOR context. */

    ANCHOR_CHECKVERSION();
    ANCHOR::CreateContext();

    /**
     * Setup Keyboard & Gamepad controls. */

    AnchorIO &io = ANCHOR::GetIO();
    io.ConfigFlags |= AnchorConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= AnchorConfigFlags_NavEnableGamepad;

    /**
     * Setup Default Kraken theme.
     *   Themes::
     *     - ANCHOR::StyleColorsDefault()
     *     - ANCHOR::StyleColorsLight()
     *     - ANCHOR::StyleColorsDark() */

    ANCHOR::StyleColorsDefault();

    /**
     * Setup Platform/Renderer backends. */

    AnchorBackendWin32Init(m_hWnd);

    ANCHOR_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                   = m_instance;
    init_info.PhysicalDevice             = m_device->GetVulkanPhysicalDevice();
    init_info.Device                     = m_device->GetVulkanDevice();
    init_info.QueueFamily                = m_device->GetGfxQueueFamilyIndex();
    init_info.Queue                      = m_commandQueue->GetVulkanGraphicsQueue();
    init_info.PipelineCache              = m_pipelineCache->GetVulkanPipelineCache();
    init_info.DescriptorPool             = g_DescriptorPool;
    init_info.Allocator                  = HgiVulkanAllocator();
    init_info.MinImageCount              = getMinImageCount();
    init_info.ImageCount                 = m_vulkan_context->ImageCount;
    init_info.CheckVkResultFn            = check_vk_result;
    // ANCHOR_ImplVulkan_Init(&init_info, m_vulkan_context->RenderPass);


    /* clang-format on */


    /**
     * Create Pixar Hydra Graphics Interface. */

    HdDriver driver;
    HgiUniquePtr hgi = HgiUniquePtr(m_hgi);
    driver.name = HgiTokens->renderDriver;
    driver.driver = VtValue(hgi.get());

    /**
     * Setup Pixar Driver & Engine. */

    ANCHOR::GetPixarDriver().name = driver.name;
    ANCHOR::GetPixarDriver().driver = driver.driver;

    SetFont();

    /**
     * ------------------------------------------------------ Upload Fonts ----- */
    {
      HgiVulkanCommandBuffer *cmdbuffer = m_commandQueue->AcquireCommandBuffer();
      VkCommandBuffer command_buffer = cmdbuffer->GetVulkanCommandBuffer();

      /**
       * Create a texture with all our fonts. */
      CreateVulkanDescriptorSetLayout();
      CreateVulkanFontTexture(command_buffer);

      /* ------ */


      /**
       * Commit the command buffer to the GPU queue for
       * processing. */

      m_commandQueue->SubmitToQueue(cmdbuffer);

      /**
       * Destroy the objects used for font texture upload. */

      m_device->WaitForIdle();
      DestroyVulkanFontTexture();
    }
  }
}

eAnchorStatus AnchorWindowWin32::activateDrawingContext()
{
  AnchorIO &io = ANCHOR::GetIO();
  ANCHOR_ASSERT(io.Fonts->IsBuilt() &&
                "Font atlas not built! It is generally built by the renderer backend. Missing "
                "call to renderer _NewFrame() function? e.g. ANCHOR_ImplOpenGL3_NewFrame().");

  /**
   * Setup display size (every frame to accommodate for window resizing). */

  AnchorRect rect;
  getWindowBounds(rect);
  if (getState() == AnchorWindowStateMinimized)
    rect.set(0, 0, 0, 0);
  io.DisplaySize = GfVec2f((float)rect.getWidth(), (float)rect.getHeight());
  if (rect.getWidth() > 0 && rect.getHeight() > 0)
    io.DisplayFramebufferScale = GfVec2f((float)rect.getWidth(), (float)rect.getHeight());

  /** 
   * Setup time step. */

  io.DeltaTime = m_system->getMilliSeconds();

  return ANCHOR_SUCCESS;
}

bool AnchorWindowWin32::isDialog() const
{
  return m_isDialog;
}

void AnchorWindowWin32::FrameRender(AnchorDrawData *draw_data)
{
  VkResult err;

  HgiVulkanCommandBuffer *cmdBuf = m_commandQueue->AcquireCommandBuffer();

  VkCommandBuffer vkcmdbuf = cmdBuf->GetVulkanCommandBuffer();
  VkCommandPool vkcmdpool = cmdBuf->GetVulkanCommandPool();

  VkSemaphore image_acquired_semaphore = m_vulkan_context->FrameSemaphores[m_vulkan_context->SemaphoreIndex].ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore = m_vulkan_context->FrameSemaphores[m_vulkan_context->SemaphoreIndex].RenderCompleteSemaphore;
  err = vkAcquireNextImageKHR(m_device->GetVulkanDevice(),
                              m_vulkan_context->Swapchain,
                              UINT64_MAX,
                              image_acquired_semaphore,
                              VK_NULL_HANDLE,
                              &m_vulkan_context->FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
  {
    g_SwapChainRebuild = true;
    return;
  }
  check_vk_result(err);

  ANCHOR_VulkanGPU_Frame *fd = &m_vulkan_context->Frames[m_vulkan_context->FrameIndex];
  {
    err = vkWaitForFences(m_device->GetVulkanDevice(),
                          1,
                          &fd->Fence,
                          VK_TRUE,
                          /* wait indefinitely==**/ UINT64_MAX);
    check_vk_result(err);

    err = vkResetFences(m_device->GetVulkanDevice(), 1, &fd->Fence);
    check_vk_result(err);
  }
  {
    err = vkResetCommandPool(m_device->GetVulkanDevice(), vkcmdpool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(vkcmdbuf, &info);
    check_vk_result(err);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = m_vulkan_context->RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = m_vulkan_context->Width;
    info.renderArea.extent.height = m_vulkan_context->Height;
    info.clearValueCount = 1;
    info.pClearValues = &m_vulkan_context->ClearValue;
    vkCmdBeginRenderPass(vkcmdbuf, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  /**
   * Record ANCHOR primitives into command buffer. */

  ANCHOR_ImplVulkan_RenderDrawData(draw_data, vkcmdbuf);

  /**
   * Submit command buffer. */
  vkCmdEndRenderPass(vkcmdbuf);
  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &vkcmdbuf;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;
    
    m_commandQueue->SubmitToQueue(cmdBuf);

  //   err = vkEndCommandBuffer(fd->CommandBuffer);
  //   check_vk_result(err);
  //   err = vkQueueSubmit(m_commandQueue->GetVulkanGraphicsQueue(), 1, &info, fd->Fence);
  //   check_vk_result(err);
  }
}

void AnchorWindowWin32::FramePresent()
{
  if (g_SwapChainRebuild)
  {
    return;
  }
  VkSemaphore render_complete_semaphore = m_vulkan_context->FrameSemaphores[m_vulkan_context->SemaphoreIndex].RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &m_vulkan_context->Swapchain;
  info.pImageIndices = &m_vulkan_context->FrameIndex;
  VkResult err = vkQueuePresentKHR(m_commandQueue->GetVulkanGraphicsQueue(), &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
  {
    g_SwapChainRebuild = true;
    return;
  }
  check_vk_result(err);
  /**
   * Now we can use the next set of semaphores. */
  m_vulkan_context->SemaphoreIndex = (m_vulkan_context->SemaphoreIndex + 1) % m_vulkan_context->ImageCount;
}

eAnchorStatus AnchorWindowWin32::swapBuffers()
{
  if (ANCHOR::GetCurrentContext() == NULL) {
    return ANCHOR_FAILURE;
  }

  ANCHOR::Render();
  AnchorDrawData *draw_data = ANCHOR::GetDrawData();
  const bool is_minimized = (draw_data->DisplaySize[0] <= 0.0f || draw_data->DisplaySize[1] <= 0.0f);
  if (!is_minimized)
  {
    m_vulkan_context->ClearValue.color.float32[0] = g_HDPARAMS_Apollo.clearColor[0];
    m_vulkan_context->ClearValue.color.float32[1] = g_HDPARAMS_Apollo.clearColor[1];
    m_vulkan_context->ClearValue.color.float32[2] = g_HDPARAMS_Apollo.clearColor[2];
    m_vulkan_context->ClearValue.color.float32[3] = g_HDPARAMS_Apollo.clearColor[3];
    FrameRender(draw_data);
    FramePresent();
  }
  return ANCHOR_SUCCESS;
}

void AnchorWindowWin32::screenToClient(AnchorS32 inX,
                                       AnchorS32 inY,
                                       AnchorS32 &outX,
                                       AnchorS32 &outY) const
{
  POINT point = {inX, inY};
  ::ScreenToClient(m_hWnd, &point);
  outX = point.x;
  outY = point.y;
}

void AnchorWindowWin32::clientToScreen(AnchorS32 inX,
                                       AnchorS32 inY,
                                       AnchorS32 &outX,
                                       AnchorS32 &outY) const
{
  POINT point = {inX, inY};
  ::ClientToScreen(m_hWnd, &point);
  outX = point.x;
  outY = point.y;
}

int AnchorWindowWin32::getMinImageCount()
{
  return ANCHOR_ImplVulkanH_GetMinImageCountFromPresentMode(m_vulkan_context->PresentMode);
}

eAnchorStatus AnchorWindowWin32::setOrder(eAnchorWindowOrder order)
{
  HWND hWndInsertAfter, hWndToRaise;

  if (order == AnchorWindowOrderBottom)
  {
    hWndInsertAfter = HWND_BOTTOM;
    hWndToRaise = ::GetWindow(m_hWnd, GW_HWNDNEXT); /* the window to raise */
  }
  else
  {
    if (getState() == AnchorWindowStateMinimized)
    {
      setState(AnchorWindowStateNormal);
    }
    hWndInsertAfter = HWND_TOP;
    hWndToRaise = NULL;
  }

  if (::SetWindowPos(m_hWnd, hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) == FALSE)
  {
    return ANCHOR_FAILURE;
  }

  if (hWndToRaise &&
      ::SetWindowPos(hWndToRaise, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) == FALSE)
  {
    return ANCHOR_FAILURE;
  }
  return ANCHOR_SUCCESS;
}

void AnchorWindowWin32::lostMouseCapture()
{
  if (m_hasMouseCaptured)
  {
    m_hasGrabMouse = false;
    m_nPressedButtons = 0;
    m_hasMouseCaptured = false;
  }
}

void AnchorWindowWin32::updateMouseCapture(eAnchorMouseCaptureEventWin32 event)
{
  switch (event)
  {
    case MousePressed:
      m_nPressedButtons++;
      break;
    case MouseReleased:
      if (m_nPressedButtons)
        m_nPressedButtons--;
      break;
    case OperatorGrab:
      m_hasGrabMouse = true;
      break;
    case OperatorUngrab:
      m_hasGrabMouse = false;
      break;
  }

  if (!m_nPressedButtons && !m_hasGrabMouse && m_hasMouseCaptured)
  {
    ::ReleaseCapture();
    m_hasMouseCaptured = false;
  }
  else if ((m_nPressedButtons || m_hasGrabMouse) && !m_hasMouseCaptured)
  {
    ::SetCapture(m_hWnd);
    m_hasMouseCaptured = true;
  }
}

HCURSOR AnchorWindowWin32::getStandardCursor(eAnchorStandardCursor shape) const
{
  // Convert ANCHOR cursor to Windows OEM cursor
  HANDLE cursor = NULL;
  HMODULE module = ::GetModuleHandle(0);
  AnchorU32 flags = LR_SHARED | LR_DEFAULTSIZE;
  int cx = 0, cy = 0;

  switch (shape)
  {
    case ANCHOR_StandardCursorCustom:
      if (m_customCursor)
      {
        return m_customCursor;
      }
      else
      {
        return NULL;
      }
    case ANCHOR_StandardCursorRightArrow:
      cursor = ::LoadImage(module, "arrowright_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorLeftArrow:
      cursor = ::LoadImage(module, "arrowleft_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorUpArrow:
      cursor = ::LoadImage(module, "arrowup_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorDownArrow:
      cursor = ::LoadImage(module, "arrowdown_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorVerticalSplit:
      cursor = ::LoadImage(module, "splitv_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorHorizontalSplit:
      cursor = ::LoadImage(module, "splith_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorKnife:
      cursor = ::LoadImage(module, "knife_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorEyedropper:
      cursor = ::LoadImage(module, "eyedropper_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorZoomIn:
      cursor = ::LoadImage(module, "zoomin_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorZoomOut:
      cursor = ::LoadImage(module, "zoomout_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorMove:
      cursor = ::LoadImage(module, "handopen_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorNSEWScroll:
      cursor = ::LoadImage(module, "scrollnsew_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorNSScroll:
      cursor = ::LoadImage(module, "scrollns_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorEWScroll:
      cursor = ::LoadImage(module, "scrollew_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorHelp:
      cursor = ::LoadImage(NULL, IDC_HELP, IMAGE_CURSOR, cx, cy, flags);
      break;  // Arrow and question mark
    case ANCHOR_StandardCursorWait:
      cursor = ::LoadImage(NULL, IDC_WAIT, IMAGE_CURSOR, cx, cy, flags);
      break;  // Hourglass
    case ANCHOR_StandardCursorText:
      cursor = ::LoadImage(NULL, IDC_IBEAM, IMAGE_CURSOR, cx, cy, flags);
      break;  // I-beam
    case ANCHOR_StandardCursorCrosshair:
      cursor = ::LoadImage(module, "cross_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;  // Standard Cross
    case ANCHOR_StandardCursorCrosshairA:
      cursor = ::LoadImage(module, "crossA_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;  // Crosshair A
    case ANCHOR_StandardCursorCrosshairB:
      cursor = ::LoadImage(module, "crossB_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;  // Diagonal Crosshair B
    case ANCHOR_StandardCursorCrosshairC:
      cursor = ::LoadImage(module, "crossC_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;  // Minimal Crosshair C
    case ANCHOR_StandardCursorBottomSide:
    case ANCHOR_StandardCursorUpDown:
      cursor = ::LoadImage(module, "movens_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;  // Double-pointed arrow pointing north and south
    case ANCHOR_StandardCursorLeftSide:
    case ANCHOR_StandardCursorLeftRight:
      cursor = ::LoadImage(module, "moveew_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;  // Double-pointed arrow pointing west and east
    case ANCHOR_StandardCursorTopSide:
      cursor = ::LoadImage(NULL, IDC_UPARROW, IMAGE_CURSOR, cx, cy, flags);
      break;  // Vertical arrow
    case ANCHOR_StandardCursorTopLeftCorner:
      cursor = ::LoadImage(NULL, IDC_SIZENWSE, IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorTopRightCorner:
      cursor = ::LoadImage(NULL, IDC_SIZENESW, IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorBottomRightCorner:
      cursor = ::LoadImage(NULL, IDC_SIZENWSE, IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorBottomLeftCorner:
      cursor = ::LoadImage(NULL, IDC_SIZENESW, IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorPencil:
      cursor = ::LoadImage(module, "pencil_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorEraser:
      cursor = ::LoadImage(module, "eraser_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;
    case ANCHOR_StandardCursorDestroy:
    case ANCHOR_StandardCursorStop:
      cursor = ::LoadImage(module, "forbidden_cursor", IMAGE_CURSOR, cx, cy, flags);
      break;  // Slashed circle
    case ANCHOR_StandardCursorDefault:
      cursor = NULL;
      break;
    default:
      return NULL;
  }

  if (cursor == NULL)
  {
    cursor = ::LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, cx, cy, flags);
  }

  return (HCURSOR)cursor;
}

eAnchorStatus AnchorWindowWin32::setWindowCursorShape(eAnchorStandardCursor cursorShape)
{
  if (::GetForegroundWindow() == m_hWnd)
  {
    loadCursor(getCursorVisibility(), cursorShape);
  }

  return ANCHOR_SUCCESS;
}

void AnchorWindowWin32::loadCursor(bool visible, eAnchorStandardCursor shape) const
{
  if (!visible)
  {
    while (::ShowCursor(FALSE) >= 0)
      ;
  }
  else
  {
    while (::ShowCursor(TRUE) < 0)
      ;
  }

  HCURSOR cursor = getStandardCursor(shape);
  if (cursor == NULL)
  {
    cursor = getStandardCursor(ANCHOR_StandardCursorDefault);
  }
  ::SetCursor(cursor);
}

eAnchorStatus AnchorWindowWin32::setClientSize(AnchorU32 width, AnchorU32 height)
{
  eAnchorStatus success;
  AnchorRect cBnds, wBnds;
  getClientBounds(cBnds);
  if ((cBnds.getWidth() != (AnchorS32)width) || (cBnds.getHeight() != (AnchorS32)height))
  {
    getWindowBounds(wBnds);
    int cx = wBnds.getWidth() + width - cBnds.getWidth();
    int cy = wBnds.getHeight() + height - cBnds.getHeight();
    success = ::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER) ?
                ANCHOR_SUCCESS :
                ANCHOR_FAILURE;
  }
  else
  {
    success = ANCHOR_SUCCESS;
  }
  return success;
}

eAnchorStatus AnchorWindowWin32::setState(eAnchorWindowState state)
{
  eAnchorWindowState curstate = getState();
  LONG_PTR style = GetWindowLongPtr(m_hWnd, GWL_STYLE) | WS_CAPTION;
  WINDOWPLACEMENT wp;
  wp.length = sizeof(WINDOWPLACEMENT);
  ::GetWindowPlacement(m_hWnd, &wp);

  switch (state)
  {
    case AnchorWindowStateMinimized:
      wp.showCmd = SW_MINIMIZE;
      break;
    case AnchorWindowStateMaximized:
      wp.showCmd = SW_SHOWMAXIMIZED;
      break;
    case AnchorWindowStateFullScreen:
      if (curstate != state && curstate != AnchorWindowStateMinimized)
      {
        m_normal_state = curstate;
      }
      wp.showCmd = SW_SHOWMAXIMIZED;
      wp.ptMaxPosition.x = 0;
      wp.ptMaxPosition.y = 0;
      style &= ~(WS_CAPTION | WS_MAXIMIZE);
      break;
    case AnchorWindowStateNormal:
    default:
      if (curstate == AnchorWindowStateFullScreen &&
          m_normal_state == AnchorWindowStateMaximized)
      {
        wp.showCmd = SW_SHOWMAXIMIZED;
        m_normal_state = AnchorWindowStateNormal;
      }
      else
      {
        wp.showCmd = SW_SHOWNORMAL;
      }
      break;
  }
  ::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);
  /* SetWindowLongPtr Docs: frame changes not visible until SetWindowPos with SWP_FRAMECHANGED. */
  ::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
  return ::SetWindowPlacement(m_hWnd, &wp) == TRUE ? ANCHOR_SUCCESS : ANCHOR_FAILURE;
}

eAnchorWindowState AnchorWindowWin32::getState() const
{
  if (::IsIconic(m_hWnd))
  {
    return AnchorWindowStateMinimized;
  }
  else if (::IsZoomed(m_hWnd))
  {
    LONG_PTR result = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
    return (result & WS_CAPTION) ? AnchorWindowStateMaximized : AnchorWindowStateFullScreen;
  }
  return AnchorWindowStateNormal;
}

eAnchorStatus AnchorWindowWin32::setProgressBar(float progress)
{
  /* #SetProgressValue sets state to #TBPF_NORMAL automatically. */
  if (m_Bar && S_OK == m_Bar->SetProgressValue(m_hWnd, 10000 * progress, 10000))
    return ANCHOR_SUCCESS;

  return ANCHOR_FAILURE;
}

eAnchorStatus AnchorWindowWin32::endProgressBar()
{
  if (m_Bar && S_OK == m_Bar->SetProgressState(m_hWnd, TBPF_NOPROGRESS))
    return ANCHOR_SUCCESS;

  return ANCHOR_FAILURE;
}

AnchorU16 AnchorWindowWin32::getDPIHint()
{
  if (m_user32)
  {
    AnchorGetDpiForWindowCallback fpGetDpiForWindow = (AnchorGetDpiForWindowCallback)::GetProcAddress(m_user32, "GetDpiForWindow");

    if (fpGetDpiForWindow)
    {
      return fpGetDpiForWindow(this->m_hWnd);
    }
  }

  return USER_DEFAULT_SCREEN_DPI;
}
