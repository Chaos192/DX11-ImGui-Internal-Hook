#pragma once
#include <pch.h>
#include <helper.h>
#if __has_include("autoexecdef.h") && !defined( Script_Path )
#include "autoexecdef.h"
#endif
#include "script_manager.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwCallReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    if (dwCallReason == DLL_PROCESS_ATTACH)
    {
        ChaosEngine::g_hModule = hModule;
        
        DisableThreadLibraryCalls(hModule);


        HANDLE hThread = CreateThread(0, 0, MainThread_Initialize, ChaosEngine::g_hModule, 0, 0);
        
        if (hThread)
            CloseHandle(hThread);
    }
    
    return TRUE;
}