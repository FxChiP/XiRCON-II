#define WIN32_LEAN_AND_MEAN
#include <windows.h>

BOOL WINAPI
DllMain (HINSTANCE hInst, ULONG ulReason, LPVOID lpReserved)
{
    if (ulReason == DLL_PROCESS_ATTACH) DisableThreadLibraryCalls(hInst);
    return TRUE;
}