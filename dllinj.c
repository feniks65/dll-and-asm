#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

// Jakieś tam deklaracje/prototypy funkcji?
DWORD GetProcessIdByName(CHAR* Name);
BOOL FileExists(CHAR* Path);
BOOL InjectDLL(DWORD PID, CHAR* Path);

// Argumenty pominięto choć można je wykorzystać - co kto lubi Razz
INT main()
{
    // Można po PID lub szExeFile - co kto lubi Smile
    // MAX_PATH powinno wystarczyć -> http://en.wikipedia.org/wiki/Filename#Comparison_of_filename_limitations
    CHAR ProcessName[MAX_PATH], LibraryName[MAX_PATH], LibraryPath[MAX_PATH] = {0};
    DWORD PID = 0;

    while(1)
    {
        cout << "Process: ";
        cin >> ProcessName;
        cout << "DLL: ";
        cin >> LibraryName;

        if((PID = GetProcessIdByName(ProcessName)) != 0)
        {
            GetFullPathName(LibraryName, MAX_PATH, LibraryPath, NULL);

            if(FileExists(LibraryPath))
            {
                cout << (InjectDLL(PID, LibraryPath) ? "Wstrzyknieto" : "Nie wstrzyknieto") << endl;
            }
            else
            {
                cout << "Nie znaleziono biblioteki" << endl;
            }
        }
        else
        {
            cout << "Nie znaleziono procesu" << endl;
        }
    }

    return EXIT_SUCCESS;
}

DWORD GetProcessIdByName(CHAR* Name)
{
    PROCESSENTRY32 pe;
    HANDLE Snapshot;
    DWORD ret = 0;

    Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);

    if(Snapshot != INVALID_HANDLE_VALUE)
    {
        if(Process32First(Snapshot, &pe))
        {
            do
            {
                if(!lstrcmpi(Name, pe.szExeFile))
                {
                    ret = pe.th32ProcessID;
                    break;
                }
            }
            while(Process32Next(Snapshot, &pe));
        }

        CloseHandle(Snapshot);
    }

    return ret;
}

BOOL FileExists(CHAR* Path)
{
    DWORD Attributes = GetFileAttributes(Path);

    return (Attributes != INVALID_FILE_ATTRIBUTES && !(Attributes & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL InjectDLL(DWORD PID, CHAR* Path)
{
    HANDLE Process, Thread;
    LPVOID RemoteMemoryAddress;
    LPTHREAD_START_ROUTINE LoadLibraryAddress;
    BOOL ret = FALSE;

    // Dla opornych PROCESS_ALL_ACCESS może się przydać
    if((Process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, PID)) != NULL)
    {
        DWORD MemorySize = strlen(Path) + 1;

        // Dla opornych PAGE_EXECUTE_READWRITE może się przydać
        if((RemoteMemoryAddress = VirtualAllocEx(Process, NULL, MemorySize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READ)) != NULL)
        {
            if(WriteProcessMemory(Process, RemoteMemoryAddress, Path, MemorySize, NULL))
            {
                LoadLibraryAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

                if((Thread = CreateRemoteThread(Process, NULL, 0, LoadLibraryAddress, RemoteMemoryAddress, 0, NULL)) != NULL)
                {
                    ret = TRUE;
                    CloseHandle(Thread);
                }
            }

        }

        CloseHandle(Process);
    }

    return ret;
}