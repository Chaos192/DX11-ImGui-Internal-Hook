// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// STANDARD LIBRARIES
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

// DIRECTX
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

//	GAMEPAD
#include <XInput.h>
#pragma comment(lib, "XInput.lib")

//	INTERNET
#include <Wininet.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

// MINHOOK
#include "libs/MinHook/MinHook.h"


// DEAR IMGUI
#include "libs/ImGui/imgui.h"
#include "libs/ImGui/imgui_internal.h"
#include "libs/ImGui/imgui_Impl_dx11.h"
#include "libs/ImGui/imgui_Impl_Win32.h"

#include <Utils/Utils.h>
//
//API
#include "api/api_ChaosEngine.h"
#include "api/api_controls.h"
#include "api/api_game.h"
#include "api/api_imgui.h"
#include "api/script_apis.h"
#include "api/api_mem.h"
#include "api/i_api.h"
#include "api/api_win.h"
#include "callback_manager.h" 
#include <helpers/imgui_prompts.h>

struct Assembly
{
    LPVOID CODECAVE = AllocatePage();
    uint64_t JMP_BACK;
    uint64_t PTR;
};

#endif //PCH_H