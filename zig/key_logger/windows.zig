const std = @import("std");
const print = std.debug.print;

// @cImport tells us we need to use some C libraries
const win = @cImport({
    @cInclude("windows.h"); // include these C libraries
    @cInclude("winuser.h");
    @cInclude("windef.h");
});

// HHOOK is a windows event, lets program monitor events in system like keyboard input
// Think of it like a  "subscribing to OS-level events" and HOOK is our subscription receipt
// Its a handle to windows hook
var hook: win.HHOOK = null;

// export because the windows hook is gonna call this function.
// callconv stands for "calling convention", tells how a zig function should be called in machine level
// like how arguememtns are passed, how stack is managed, and how is responsible for cleaning it up
// Because you're registering a callback (LowLevelKeyboardProc) that the Windows operating system will call.
// Windows expects it to follow a very specific calling convention.
export fn keyBoardHookFunc(nCode: c_int, wParam: win.WPARAM, lParam: win.LPARAM) callconv(.C) win.LRESULT {
    if (nCode == win.HC_ACTION) {
        const lp: usize = @intCast(lParam);
        const raw_ptr: *const win.KBDLLHOOKSTRUCT = @ptrFromInt(lp);
        //const kb_ptr = @ptrCast(*const win.KBDLLHOOKSTRUCT, raw_ptr);
        if (wParam == win.WM_KEYDOWN or wParam == win.WM_SYSKEYDOWN) {
            print("Key code: {}\n", .{raw_ptr.vkCode});
        }
    }

    return win.CallNextHookEx(hook, nCode, wParam, lParam);
}

pub fn main() !void {
    hook = win.SetWindowsHookExW(win.WH_KEYBOARD_LL, keyBoardHookFunc, win.GetModuleHandleW(null), 0);
    if (hook == null) {
        return error.UnableToSetHook;
    }

    var msg: win.MSG = undefined;

    while (win.GetMessageW(&msg, null, 0, 0) != 0) {
        const v = win.TranslateMessage(&msg);
        _ = v;
        _ = win.DispatchMessageA(&msg);
    }
}
