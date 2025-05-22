use std::ptr::null_mut;
use std::mem::transmute;
use windows::Win32::Foundation::*;
use windows::Win32::UI::WindowsAndMessaging::*;
use windows::Win32::System::LibraryLoader::GetModuleHandleW;

unsafe extern "system" fn keyboard_proc(n_code: i32, w_param: WPARAM, l_param: LPARAM) -> LRESULT {
    unsafe {

    if n_code == HC_ACTION as i32 && (w_param.0 == WM_KEYDOWN as usize) {
        let kb_struct = *(l_param.0 as *const KBDLLHOOKSTRUCT);
        let vk_code = kb_struct.vkCode;
        println!("Key Pressed: {}", vk_code); // You can convert this to char later
    }

    return CallNextHookEx(None, n_code, w_param, l_param);
    }
}

fn main() {
    unsafe {
        let h_instance = GetModuleHandleW(None).unwrap();
        let hook = SetWindowsHookExW(WH_KEYBOARD_LL, Some(keyboard_proc), h_instance, 0);

        if let Err(e) = hook {
            println!("Error in hoook {}",e);
            return;
        }

        if hook.as_ref().unwrap().is_invalid() {
            eprintln!("Failed to set hook");
            return;
        }

        let mut msg = MSG::default();
        while GetMessageW(&mut msg, None, 0, 0).into() {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if let Err (e) = UnhookWindowsHookEx(hook.unwrap()) {
            println!("Error when trying to unhook {}",e);
        }
    }
}
