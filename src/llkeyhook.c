// vim:fenc=utf-8 ff=unix ft=c ts=2 sw=2 sts=2 si et :

#include <windows.h>
#include "win.h"
#include "winids.h"
#include "wininput.h"

static HHOOK hKeyboardHook = NULL;
static bool isWindowActive = false;

#define VK_V (0x56)
#define VK_C (0x43)
#define VK_N (0x4E)

LRESULT CALLBACK ll_keyhook_proc(int nCode, WPARAM wParam, LPARAM lParam) {
  KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

  if (nCode < 0 || nCode != HC_ACTION ) {
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
  }

  if (!isWindowActive) {
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
  }

  switch (wParam) {
    case WM_KEYDOWN: {
      if ((GetKeyState(VK_LWIN) & 0x8000) || (GetKeyState(VK_RWIN) & 0x8000)) {
        if (p->vkCode == VK_V) {
          win_paste();
          return 1;
        } else if (p->vkCode == VK_C) {
          term_copy();
          return 1;
        } else if (p->vkCode == VK_N) {
          send_syscommand(IDM_NEW);
          return 1;
        }
      }
      break;
    }
  }

  return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

bool ll_keyhook_install(void) {
  hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, ll_keyhook_proc, GetModuleHandle(NULL), 0);

  if (hKeyboardHook == NULL) {
    return false;
  } else {
    return true;
  }
}

void ll_keyhook_uninstall(void) {
  if (hKeyboardHook == NULL) {
    return;
  }

  UnhookWindowsHookEx(hKeyboardHook);
}

void ll_keyhook_tell_window_state(bool is_active) {
  isWindowActive = is_active; 
}

