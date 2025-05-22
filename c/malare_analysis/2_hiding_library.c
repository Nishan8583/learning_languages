#include <windows.h>

// We need these definitions because GetProcAddress returns generic FACPROC and we want to use it so we need to cast it to tthe proper function signature first
// I got consfused here, i am not defining HMODULE or FARPROC, thats the return type of respective functions, i am defining pLoadLibraryA, pGetPRocAddress and pMessageBoxA

// I have added the actual function signature from the MSD on top of each function definition

/*
HMODULE LoadLibraryA(
  [in] LPCSTR lpLibFileName
);
*/
typedef HMODULE (WINAPI *pLoadLibraryA)(LPCSTR);

/*
FARPROC GetProcAddress(
  [in] HMODULE hModule,
  [in] LPCSTR  lpProcName
);
*/
typedef FARPROC (WINAPI *pGetProcAddress)(HMODULE,LPCSTR);

/*
int MessageBoxA(
  [in, optional] HWND   hWnd,
  [in, optional] LPCSTR lpText,
  [in, optional] LPCSTR lpCaption,
  [in]           UINT   uType
);
*/
typedef int (WINAPI *pMessageBoxA) (HWND, LPCSTR,LPCSTR,UINT);

int main() {

    // Get the handle to GetModuleHandleA
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");

    // GetProcAddress for LoadLibraryA, casting it to correct function signature so that we can call it
    pLoadLibraryA MyLoadLibrary = (pLoadLibraryA)GetProcAddress(kernel32,"LoadLibraryA");
    // GetProcAddress for GetProcAddress? ChatGPT suggested to getProcAddress of getProcAddress and do it, but i guess we can use it directly
    // I guess if we do what ChatGPT said, we can hide GetProcAddress itself in disassembler ?
    //pGetProcAddress MyGetProcAddress = (pGetProcAddress)GetProcAddress(kernel32,"GetProcAddress");

    // Loading huser32
    HMODULE huser32 = MyLoadLibrary("user32.dll");

    // Get PRocessAddress for MessaageBoxA, casting to to proper function signature
    //pMessageBoxA MyMessageBoxA = (pMessageBoxA)MyGetProcAddress(huser32,"MessageBoxA");

    pMessageBoxA MyMessageBoxA = (pMessageBoxA)GetProcAddress(huser32,"MessageBoxA");


    // Since we caste it, now we can use it
    MyMessageBoxA(NULL,"Hello i loaded messageBoxA dynamically","HELLO",MB_OK);  // MB_OK means the message box will display only an "OK" button.

    return 0;

}