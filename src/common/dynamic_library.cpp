#include "dynamic_library.h"
#include <windows.h>

DynLibHandle DynamicLibraryOpen(const char * path, bool showErrors)
{
    if (path == nullptr)
    {
        return nullptr;
    }
    UINT lastErrorMode = SetErrorMode(showErrors ? 0 : SEM_FAILCRITICALERRORS);
    DynLibHandle lib = (DynLibHandle)LoadLibraryA(path);
    SetErrorMode(lastErrorMode);
    return lib;
}

void DynamicLibraryClose(DynLibHandle libHandle)
{
    if (libHandle != nullptr)
    {
        FreeLibrary((HMODULE)libHandle);
    }
}

void * DynamicLibraryGetProc(DynLibHandle libHandle, const char * name)
{
    if (name == nullptr)
    {
        return nullptr;
    }
    return GetProcAddress((HMODULE)libHandle, name);
}