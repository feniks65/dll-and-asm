#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
            MessageBox(NULL, "DLL_PROCESS_ATTACH", "", MB_OK);
        break;

        case DLL_PROCESS_DETACH:
            MessageBox(NULL, "DLL_PROCESS_DETACH", "", MB_OK);
        break;

        case DLL_THREAD_ATTACH:
            MessageBox(NULL, "DLL_THREAD_ATTACH", "", MB_OK);
        break;

        case DLL_THREAD_DETACH:
            MessageBox(NULL, "DLL_THREAD_DETACH", "", MB_OK);
        break;
    }

    return TRUE;
}