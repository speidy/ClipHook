//
// Clipboard format filter hook
// Author: Idan Freiberg (speidy@gmail.com)
//
#include "stdafx.h"
#include "mhook/mhook-lib/mhook.h"

// Define _SetClipboardData so we can dynamically bind to the function
typedef HANDLE(WINAPI* _SetClipboardData)(_In_ UINT uFormat, _In_opt_ HANDLE hMem);

//////////////////////////////////////////////////////////////////////////
// Original function

_SetClipboardData TrueSetClipboardData = (_SetClipboardData)
	GetProcAddress(GetModuleHandle(L"user32"), "SetClipboardData");

//////////////////////////////////////////////////////////////////////////
// Hooked function
// This is the function that will replace SetClipboardData once the hook 
// is in place

HANDLE WINAPI HookSetClipboardData(_In_ UINT uFormat, _In_opt_ HANDLE hMem)
{
	// log clipboard formats to file
	FILE *file = fopen("c:\\windows\\temp\\clipformats.txt", "a");
	if (file != NULL)
	{
		fprintf(file, "Format: %d\n", uFormat);
		fclose(file);
	}

	// Check for valid formats
	if (uFormat != CF_TEXT && uFormat != CF_UNICODETEXT)
	{
		return NULL;
	}
	return TrueSetClipboardData(uFormat, hMem);
}

//////////////////////////////////////////////////////////////////////////
// Entry point

BOOL WINAPI DllMain(
    __in HINSTANCE  hInstance,
    __in DWORD      Reason,
    __in LPVOID     Reserved
    )
{        
    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        Mhook_SetHook((PVOID*)&TrueSetClipboardData, HookSetClipboardData);
        break;

    case DLL_PROCESS_DETACH:
		Mhook_Unhook((PVOID*)&TrueSetClipboardData);
        break;
    }

    return TRUE;
}
